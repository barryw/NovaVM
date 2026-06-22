// fio_bridge.sv — AXI4-Lite slave letting the Zynq PS act as the NovaVM host.
//
// Replaces the ESP32 NovaHost SPI debug_bridge. The PS (M_AXI_GP0) pokes/peeks
// CPU RAM + the FIO register bank ($B9A0-$B9EF), injects keyboard keys, holds/
// releases the 6502 reset, and learns of fio_event. XRAM (the library shelf) is
// NOT routed through here — the PS writes it DIRECTLY into PS DDR3 (axi_xram maps
// XRAM byte addr -> DDR XRAM_BASE+addr), so module staging is a plain DDR write.
//
// Register map (AXI4-Lite, 32-bit, offsets within the GP0 segment):
//   0x00 W  POKE       wdata = {8'b0, addr[15:0], data[7:0]}  -> 1-cyc dbg_poke
//   0x04 W  PEEK_ADDR  wdata[15:0] = addr (held; continuous peek)
//   0x08 R  PEEK_DATA  -> {24'b0, dbg_peek_data}
//   0x0C W  KEY        wdata[7:0] = key byte -> 1-cyc key_valid
//   0x10 RW CTRL       bit0 = dbg_cpu_reset (defaults 1 = CPU held at config)
//   0x14 R  STATUS     {key_ready, fio_event_sticky};  W bit0 -> clear sticky
//   0x18 R  CPU_PC     {16'b0, dbg_cpu_pc}  (debug)
//
// The CPU is held in reset at PL config (dbg_cpu_reset=1) until the PS stages the
// resident loader and clears CTRL.bit0 — mirroring NovaHost's resetHold/Release.

module fio_bridge (
    input  wire        aclk,
    input  wire        aresetn,

    // ---- AXI4-Lite slave ----
    input  wire [31:0] s_awaddr,
    input  wire        s_awvalid,
    output reg         s_awready,
    input  wire [31:0] s_wdata,
    input  wire [3:0]  s_wstrb,
    input  wire        s_wvalid,
    output reg         s_wready,
    output reg  [1:0]  s_bresp,
    output reg         s_bvalid,
    input  wire        s_bready,
    input  wire [31:0] s_araddr,
    input  wire        s_arvalid,
    output reg         s_arready,
    output reg  [31:0] s_rdata,
    output reg  [1:0]  s_rresp,
    output reg         s_rvalid,
    input  wire        s_rready,

    // ---- debug/host surface to top ----
    output reg         dbg_poke_en,
    output reg  [15:0] dbg_poke_addr,
    output reg  [7:0]  dbg_poke_data,
    output reg  [15:0] dbg_peek_addr,
    output wire        dbg_peek_en,
    input  wire [7:0]  dbg_peek_data,
    output reg         key_valid,
    output reg  [7:0]  key_data,
    input  wire        key_ready,
    // host ROM-load port (PS writes a module image straight into bank-1 ext_rom).
    output reg         dbg_rom_we,      // 1-cycle pulse
    output reg         dbg_rom_idx,     // 0=basic_rom, 1=ext_rom (bank-1 modules)
    output reg  [13:0] dbg_rom_addr,
    output reg  [7:0]  dbg_rom_data,
    output reg         dbg_cpu_reset,
    input  wire        fio_event,
    input  wire [15:0] dbg_cpu_pc,
    input  wire [15:0] dbg_bus,      // misc debug signals (page-in/stream/rdy)
    input  wire [27:0] dbg_aux,      // {stream_words[13:0], stream_left[13:0]}
    // VGC vmem debug read: lets the PS read screen/char/color/gfx/sprite RAM
    // (the VGC's internal dprams, NOT on the 6502 bus) -> serial screen-dump.
    output wire        dbg_vmem_re,
    output reg  [2:0]  dbg_vmem_space,
    output reg  [16:0] dbg_vmem_addr,
    input  wire [7:0]  dbg_vmem_rdata,
    // host audio: the PS streams 48 kHz / 16-bit stereo L-PCM into the HDMI audio
    // FIFO. R_AUDIO (0x30 W) pushes one PCM byte; R_AUDIO_SPACE (0x34 R) returns
    // the FIFO's free byte count so the PS can pace its writes and never overflow
    // (an overflowing write silently drops the sample).
    output reg         audio_we,        // 1-cycle pulse
    output reg  [7:0]  audio_data,
    input  wire [15:0] audio_space,
    // Audio-register event FIFO: each read of R_AUDIO_EVT (0x38) returns
    // {valid, index[7:0], data[7:0]} and pops one entry.
    output reg         audio_evt_rd,    // 1-cycle pop pulse
    input  wire [15:0] audio_evt_data,  // {index[7:0], data[7:0]} at FIFO head
    input  wire        audio_evt_empty
);
    assign dbg_peek_en = 1'b1;   // continuously peek the latched address
    assign dbg_vmem_re = 1'b1;   // continuously read the latched vmem addr/space

    reg fio_event_sticky;

    always_ff @(posedge aclk) begin
        if (!aresetn) begin
            s_awready <= 1'b0; s_wready <= 1'b0; s_bvalid <= 1'b0; s_bresp <= 2'b00;
            s_arready <= 1'b0; s_rvalid <= 1'b0; s_rdata <= 32'd0; s_rresp <= 2'b00;
            dbg_poke_en <= 1'b0; dbg_poke_addr <= 16'd0; dbg_poke_data <= 8'd0;
            dbg_peek_addr <= 16'd0; key_valid <= 1'b0; key_data <= 8'd0;
            dbg_cpu_reset <= 1'b1;            // hold the 6502 until the PS releases it
            fio_event_sticky <= 1'b0;
            dbg_rom_we <= 1'b0; dbg_rom_idx <= 1'b0; dbg_rom_addr <= 14'd0; dbg_rom_data <= 8'd0;
            dbg_vmem_space <= 3'd1; dbg_vmem_addr <= 17'd0;   // default SPACE_CHAR
            audio_we <= 1'b0; audio_data <= 8'd0;
            audio_evt_rd <= 1'b0;
        end else begin
            dbg_poke_en <= 1'b0;             // 1-cycle pulses
            key_valid   <= 1'b0;
            dbg_rom_we  <= 1'b0;
            audio_we    <= 1'b0;
            audio_evt_rd <= 1'b0;
            if (fio_event) fio_event_sticky <= 1'b1;

            // ---- write channel (single outstanding) ----
            if (s_awvalid && s_wvalid && !s_awready && !s_bvalid) begin
                s_awready <= 1'b1;
                s_wready  <= 1'b1;
                case (s_awaddr[5:2])
                    4'h0: begin                       // POKE
                        dbg_poke_en   <= 1'b1;
                        dbg_poke_addr <= s_wdata[23:8];
                        dbg_poke_data <= s_wdata[7:0];
                    end
                    4'h1: dbg_peek_addr <= s_wdata[15:0];          // PEEK_ADDR
                    4'h3: begin key_valid <= 1'b1; key_data <= s_wdata[7:0]; end // KEY
                    4'h4: dbg_cpu_reset <= s_wdata[0];             // CTRL
                    4'h5: if (s_wdata[0]) fio_event_sticky <= 1'b0; // STATUS clear
                    4'h9: begin                                    // ROMW (0x24)
                        dbg_rom_we   <= 1'b1;
                        dbg_rom_data <= s_wdata[7:0];
                        dbg_rom_addr <= s_wdata[21:8];
                        dbg_rom_idx  <= s_wdata[22];
                    end
                    4'hA: begin                                    // VMEM_ADDR (0x28)
                        dbg_vmem_addr  <= s_wdata[16:0];
                        dbg_vmem_space <= s_wdata[19:17];
                    end
                    4'hC: begin audio_we <= 1'b1; audio_data <= s_wdata[7:0]; end // AUDIO (0x30)
                    default: ;
                endcase
                s_bvalid <= 1'b1;
                s_bresp  <= 2'b00;
            end else begin
                s_awready <= 1'b0;
                s_wready  <= 1'b0;
            end
            if (s_bvalid && s_bready) s_bvalid <= 1'b0;

            // ---- read channel ----
            if (s_arvalid && !s_arready && !s_rvalid) begin
                s_arready <= 1'b1;
                case (s_araddr[5:2])
                    4'h2: s_rdata <= {24'd0, dbg_peek_data};            // PEEK_DATA
                    4'h4: s_rdata <= {31'd0, dbg_cpu_reset};            // CTRL
                    4'h5: s_rdata <= {30'd0, key_ready, fio_event_sticky}; // STATUS
                    4'h6: s_rdata <= {16'd0, dbg_cpu_pc};               // CPU_PC
                    4'h7: s_rdata <= {16'd0, dbg_bus};                  // DBG (0x1C)
                    4'h8: s_rdata <= {4'd0, dbg_aux};                   // AUX (0x20) sleft/swords
                    4'hB: s_rdata <= {24'd0, dbg_vmem_rdata};           // VMEM_DATA (0x2C)
                    4'hD: s_rdata <= {16'd0, audio_space};              // AUDIO_SPACE (0x34)
                    // AUDIO_EVT (0x38): {valid, index[7:0], data[7:0]}; pop on read.
                    4'hE: begin s_rdata <= {15'd0, ~audio_evt_empty, audio_evt_data};
                                audio_evt_rd <= ~audio_evt_empty; end
                    default: s_rdata <= 32'hDEAD_0000;
                endcase
                s_rvalid <= 1'b1;
                s_rresp  <= 2'b00;
            end else begin
                s_arready <= 1'b0;
            end
            if (s_rvalid && s_rready) s_rvalid <= 1'b0;
        end
    end
endmodule
