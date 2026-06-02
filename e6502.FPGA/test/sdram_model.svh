// sdram_model.svh — behavioral MT48LC16M16 with page-mode + timing assertions.
// Include into a testbench; instantiate `sdram_model chip(...)` wired to the sd_* pins.
// Storage is 16-bit words keyed by {bank,row,col}; honours DQM byte masks on write.
// Timing params in sdram_clk cycles @100MHz: tRCD=2, CAS=3, tRP=2, tRAS=5, tRC=7.
// NOTE: returns correct DATA for page-mode (A10=0) reads; the controller owns exact
// timing — but this model *checks* the controller obeys tRCD/CAS/tRP/tRAS/tRC and
// $fatal's otherwise, which is the whole point of the bench.
//
// Command encoding decoded from {cs,ras,cas,we} (matches sdram.v ~line 112):
//   NOP=4'b0111 ACTIVE=4'b0011 READ=4'b0101 WRITE=4'b0100
//   BURST_TERMINATE=4'b0110 PRECHARGE=4'b0010 AUTO_REFRESH=4'b0001
//   LOAD_MODE=4'b0000 INHIBIT=4'b1111
//
// Address decode (matches sdram.v run_addr/sd_ba):
//   On ACTIVE : row = sd_addr[12:0], bank = sd_ba[1:0]
//   On R/W    : col = sd_addr[8:0], A10 = sd_addr[10] (auto-precharge), bank = sd_ba
//   Word address (byte addr a): bank=a[23:22], row=a[21:9], col={a[24],a[8:1]}.
//   Storage key = the 24-bit word address a[24:1].
//
// Byte lanes (matches sdram.v sd_dqm + dout):
//   even byte a[0]=0 -> HIGH lane [15:8]; odd byte a[0]=1 -> LOW lane [7:0].
//   On write the controller drives sd_dqm = {a[0], ~a[0]} (1 = masked / not written),
//   so a[0]=0 unmasks [15:8], a[0]=1 unmasks [7:0]. Model honours both lanes.
//
// Port direction (controller's perspective in sdram.v):
//   sd_data_in  = controller INPUT  (chip -> controller). THIS MODEL DRIVES IT on reads.
//   sd_data_out = controller OUTPUT (controller -> chip). THIS MODEL READS IT on writes.

module sdram_model (
    input  wire        clk,           // sdram_clk — timing is counted in these cycles
    input  wire [15:0] sd_data_out,   // controller -> chip (write data)
    output reg  [15:0] sd_data_in,    // chip -> controller (read data)
    input  wire [12:0] sd_addr,
    input  wire [1:0]  sd_dqm,
    input  wire [1:0]  sd_ba,
    input  wire        sd_cs,
    input  wire        sd_we,
    input  wire        sd_ras,
    input  wire        sd_cas
);

    // ----- timing parameters (sdram_clk cycles) -----
    localparam int unsigned tRCD = 2;  // ACTIVE -> READ/WRITE
    localparam int unsigned CAS  = 3;  // READ -> data on bus
    localparam int unsigned tRP  = 2;  // PRECHARGE -> re-ACTIVATE (and -> AUTO_REFRESH)
    localparam int unsigned tRAS = 5;  // ACTIVE -> PRECHARGE (row-active minimum)
    localparam int unsigned tRC  = 7;  // ACTIVE -> ACTIVE (full row cycle, incl. refresh)

    // ----- command encoding -----
    localparam logic [3:0] CMD_INHIBIT         = 4'b1111;
    localparam logic [3:0] CMD_NOP             = 4'b0111;
    localparam logic [3:0] CMD_ACTIVE          = 4'b0011;
    localparam logic [3:0] CMD_READ            = 4'b0101;
    localparam logic [3:0] CMD_WRITE           = 4'b0100;
    localparam logic [3:0] CMD_BURST_TERMINATE = 4'b0110;
    localparam logic [3:0] CMD_PRECHARGE       = 4'b0010;
    localparam logic [3:0] CMD_AUTO_REFRESH    = 4'b0001;
    localparam logic [3:0] CMD_LOAD_MODE       = 4'b0000;

    wire [3:0] cmd = {sd_cs, sd_ras, sd_cas, sd_we};

    // ----- sparse storage, keyed by 24-bit word address a[24:1] -----
    logic [15:0] mem [bit [24:0]];

    // ----- per-bank state -----
    logic [12:0] open_row    [0:3];   // currently-open row per bank
    logic        row_active  [0:3];   // bank has an open row
    // free-running cycle clock for timing checks
    longint unsigned cyc;
    longint unsigned t_activate  [0:3];  // cycle stamp of last ACTIVE per bank
    longint unsigned t_precharge [0:3];  // cycle stamp of last PRECHARGE per bank

    // ----- CAS read pipeline: data appears CAS cycles after READ accepted -----
    // depth CAS+1 so a value latched at READ surfaces exactly CAS cycles later.
    logic [15:0] rd_pipe  [0:CAS];
    logic        rd_valid [0:CAS];

    initial begin
        cyc = 0;
        for (int b = 0; b < 4; b++) begin
            row_active[b]  = 1'b0;
            open_row[b]    = 13'd0;
            t_activate[b]  = 0;
            t_precharge[b] = 0;
        end
        for (int i = 0; i <= CAS; i++) begin
            rd_pipe[i]  = 16'h0000;
            rd_valid[i] = 1'b0;
        end
        sd_data_in = 16'h0000;
    end

    // word key from an open bank/row + presented column
    function automatic bit [24:0] word_key(input logic [1:0] ba,
                                            input logic [12:0] row,
                                            input logic [8:0] col);
        // Reconstruct the 24-bit word address a[24:1]:
        //   a[23:22]=bank, a[21:9]=row, a[8:1]=col[7:0], a[24]=col[8].
        // Pack as {a[24], a[23:22], a[21:9], a[8:1]} into a 25-bit key (LSB a[1..]).
        word_key = {col[8], ba, row, col[7:0]};
    endfunction

    always @(posedge clk) begin
        cyc <= cyc + 1;

        // ---- advance the CAS read pipeline ----
        for (int i = CAS; i >= 1; i--) begin
            rd_pipe[i]  <= rd_pipe[i-1];
            rd_valid[i] <= rd_valid[i-1];
        end
        rd_pipe[0]  <= 16'h0000;
        rd_valid[0] <= 1'b0;

        // ---- present read data exactly CAS cycles after the READ ----
        if (rd_valid[CAS])
            sd_data_in <= rd_pipe[CAS];

        // ---- command decode ----
        case (cmd)

            CMD_ACTIVE: begin
                // re-ACTIVATE same bank requires a prior PRECHARGE (no open row), and
                // tRP since that PRECHARGE, and tRC since the last ACTIVATE.
                if (row_active[sd_ba]) begin
                    $error("[sdram_model] ACTIVE on bank %0d while a row is still open (missing PRECHARGE) @cyc %0d",
                           sd_ba, cyc);
                    $fatal(1, "ACTIVE without prior PRECHARGE");
                end
                if (t_precharge[sd_ba] != 0 && (cyc - t_precharge[sd_ba]) < tRP) begin
                    $error("[sdram_model] ACTIVE too soon after PRECHARGE on bank %0d: %0d < tRP=%0d @cyc %0d",
                           sd_ba, (cyc - t_precharge[sd_ba]), tRP, cyc);
                    $fatal(1, "tRP violation (ACTIVE after PRECHARGE)");
                end
                if (t_activate[sd_ba] != 0 && (cyc - t_activate[sd_ba]) < tRC) begin
                    $error("[sdram_model] ACTIVE too soon after ACTIVE on bank %0d: %0d < tRC=%0d @cyc %0d",
                           sd_ba, (cyc - t_activate[sd_ba]), tRC, cyc);
                    $fatal(1, "tRC violation (ACTIVE-to-ACTIVE)");
                end
                open_row[sd_ba]   <= sd_addr;
                row_active[sd_ba] <= 1'b1;
                t_activate[sd_ba] <= cyc;
            end

            CMD_READ: begin
                // a READ requires an open row and at least tRCD since ACTIVATE.
                if (!row_active[sd_ba]) begin
                    $error("[sdram_model] READ on bank %0d with no open row (missing ACTIVE) @cyc %0d",
                           sd_ba, cyc);
                    $fatal(1, "READ without ACTIVE");
                end
                if ((cyc - t_activate[sd_ba]) < tRCD) begin
                    $error("[sdram_model] READ too soon after ACTIVE on bank %0d: %0d < tRCD=%0d @cyc %0d",
                           sd_ba, (cyc - t_activate[sd_ba]), tRCD, cyc);
                    $fatal(1, "tRCD violation (READ after ACTIVE)");
                end
                // page-mode: A10 (sd_addr[10]) must be 0 (no auto-precharge) for our burst.
                // Auto-precharge reads are allowed by the chip but the page-mode engine
                // must not use them; flag it so a stray A10=1 is caught.
                if (sd_addr[10]) begin
                    $error("[sdram_model] page-mode READ with A10=1 (auto-precharge) on bank %0d @cyc %0d",
                           sd_ba, cyc);
                    $fatal(1, "A10=1 on page-mode READ");
                end
                // look up the stored word (default 0 if never written) and launch it
                // down the CAS pipeline.
                begin
                    bit [24:0] k;
                    k = word_key(sd_ba, open_row[sd_ba], sd_addr[8:0]);
                    rd_pipe[0]  <= mem.exists(k) ? mem[k] : 16'h0000;
                    rd_valid[0] <= 1'b1;
                end
            end

            CMD_WRITE: begin
                if (!row_active[sd_ba]) begin
                    $error("[sdram_model] WRITE on bank %0d with no open row (missing ACTIVE) @cyc %0d",
                           sd_ba, cyc);
                    $fatal(1, "WRITE without ACTIVE");
                end
                if ((cyc - t_activate[sd_ba]) < tRCD) begin
                    $error("[sdram_model] WRITE too soon after ACTIVE on bank %0d: %0d < tRCD=%0d @cyc %0d",
                           sd_ba, (cyc - t_activate[sd_ba]), tRCD, cyc);
                    $fatal(1, "tRCD violation (WRITE after ACTIVE)");
                end
                begin
                    bit [24:0]   k;
                    logic [15:0] cur;
                    k   = word_key(sd_ba, open_row[sd_ba], sd_addr[8:0]);
                    cur = mem.exists(k) ? mem[k] : 16'h0000;
                    // DQM: bit high = lane masked (not written).
                    if (!sd_dqm[1]) cur[15:8] = sd_data_out[15:8];
                    if (!sd_dqm[0]) cur[7:0]  = sd_data_out[7:0];
                    mem[k] = cur;
                end
            end

            CMD_PRECHARGE: begin
                // A10 high = precharge all banks; A10 low = precharge sd_ba only.
                // PRECHARGE legal only after tRAS since the bank's ACTIVATE.
                if (sd_addr[10]) begin
                    for (int b = 0; b < 4; b++) begin
                        if (row_active[b]) begin
                            if ((cyc - t_activate[b]) < tRAS) begin
                                $error("[sdram_model] PRECHARGE-ALL too soon after ACTIVE on bank %0d: %0d < tRAS=%0d @cyc %0d",
                                       b, (cyc - t_activate[b]), tRAS, cyc);
                                $fatal(1, "tRAS violation (PRECHARGE-ALL)");
                            end
                            row_active[b]  <= 1'b0;
                            t_precharge[b] <= cyc;
                        end
                    end
                end else begin
                    if (row_active[sd_ba]) begin
                        if ((cyc - t_activate[sd_ba]) < tRAS) begin
                            $error("[sdram_model] PRECHARGE too soon after ACTIVE on bank %0d: %0d < tRAS=%0d @cyc %0d",
                                   sd_ba, (cyc - t_activate[sd_ba]), tRAS, cyc);
                            $fatal(1, "tRAS violation (PRECHARGE)");
                        end
                        row_active[sd_ba]  <= 1'b0;
                        t_precharge[sd_ba] <= cyc;
                    end
                end
            end

            CMD_AUTO_REFRESH: begin
                // AUTO_REFRESH requires all banks precharged (idle).
                for (int b = 0; b < 4; b++) begin
                    if (row_active[b]) begin
                        $error("[sdram_model] AUTO_REFRESH with bank %0d still active (not precharged) @cyc %0d",
                               b, cyc);
                        $fatal(1, "AUTO_REFRESH with an open row");
                    end
                end
            end

            default: ; // NOP / INHIBIT / LOAD_MODE / BURST_TERMINATE — no model action
        endcase
    end

endmodule
