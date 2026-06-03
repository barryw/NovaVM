// Top-level: 6502 CPU + VGC + Blitter + DMA + SID + 64KB RAM + 512KB XRAM
// VGC snoops bus for register/charout writes, generates video
// Blitter performs 2D DMA across all memory spaces ($BA83-$BA9B plus $BAA2), stalls CPU via RDY
// Hosted music visualizer/status registers live at $BA50-$BA62
// DMA performs 1D bulk copy/fill ($BA63-$BA75), stalls CPU via RDY
// Blitter and DMA are mutually exclusive via RDY stall; top.sv muxes their
// memory-port outputs into a single bus-master signal set (bm_*).
// SID: dual 6581/8580 sound chips, runtime model/clock select via $D440
// XMC bank switching: 4x256-byte windows at $BC00-$BFFF into 512KB XRAM
//
// All large memories use dpram instances for BRAM inference during synthesis.

module top (
    input  logic        clk,        // pixel clock (~25 MHz)
    input  logic        rst,

    // Keyboard input
    input  logic        key_valid,
    input  logic [7:0]  key_data,
    output logic        key_ready,

    // Board input state, active-high and already debounced by fpga_top.
    input  logic [7:0]  board_buttons,
    input  logic [7:0]  board_switches,

    // FPGA-side USB HID diagnostics, synchronized by fpga_top.
    input  logic [7:0]  usb_hid_status,
    input  logic [7:0]  usb_hid_device_type,
    input  logic [7:0]  usb_hid_last_scan,
    input  logic [7:0]  usb_hid_last_ascii,
    input  logic [7:0]  usb_hid_report_count,
    input  logic [7:0]  usb_hid_key_count,
    input  logic [7:0]  usb_hid_core_status,
    input  logic [63:0] usb_hid_regs,

    input  logic        irq_n,
    input  logic        nmi_n,

    // Video output
    output logic [3:0]  vid_r,
    output logic [3:0]  vid_g,
    output logic [3:0]  vid_b,
    output logic        vid_hsync,
    output logic        vid_vsync,
    output logic        vid_de,

    // Audio output (signed 18-bit, active each frame)
    output logic signed [17:0] audio_l,
    output logic signed [17:0] audio_r,

    // Debug/simulation interface
    input  logic        dbg_peek_en,
    input  logic [15:0] dbg_peek_addr,
    output logic [7:0]  dbg_peek_data,
    input  logic        dbg_poke_en,
    input  logic [15:0] dbg_poke_addr,
    input  logic [7:0]  dbg_poke_data,
    input  logic        dbg_pause,        // halt CPU when high
    input  logic        dbg_nic_buf_we,
    input  logic        dbg_nic_buf_re,
    input  logic        dbg_nic_buf_sel,
    input  logic [7:0]  dbg_nic_buf_addr,
    input  logic [7:0]  dbg_nic_buf_data,
    output logic [7:0]  dbg_nic_buf_rdata,
    input  logic        dbg_vmem_we,
    input  logic        dbg_vmem_re,
    input  logic [2:0]  dbg_vmem_space,
    input  logic [16:0] dbg_vmem_addr,
    input  logic [7:0]  dbg_vmem_data,
    output logic [7:0]  dbg_vmem_rdata,
    // ROM-load port — write one byte at a time into either ROM bank.
    // idx=0 → basic_rom, idx=1 → ext_rom. Used by NovaHost to stream the
    // EhBASIC + extension binaries into BRAM at boot.
    input  logic        dbg_rom_we,
    input  logic        dbg_rom_idx,
    input  logic [13:0] dbg_rom_addr,
    input  logic [7:0]  dbg_rom_data,
    input  logic        dbg_cpu_reset,     // ESP32-driven soft reset, ORs with rst
    input  logic        dbg_system_reset,  // ESP32-driven custom-chip reset, ORs with rst
    input  logic        dbg_cpu_resume,    // one-cycle pulse; releases CPU STP halt
    // Debug-bridge SDRAM port B path. NovaHost uses this for boot preload and
    // direct XRAM file streaming.
    input  logic        brg_sdram_b_we,
    input  logic        brg_sdram_b_oe,
    input  logic [24:0] brg_sdram_b_addr,
    input  logic [7:0]  brg_sdram_b_din,
    input  logic        host_wts_event_we,
    input  logic [7:0]  host_wts_event_data,
    output logic        host_wts_event_ready,
    output logic [15:0] dbg_cpu_pc,
    output logic [7:0]  dbg_cpu_a,
    output logic [7:0]  dbg_cpu_x,
    output logic [7:0]  dbg_cpu_y,
    output logic [7:0]  dbg_cpu_sp,
    output logic [7:0]  dbg_cpu_flags,
    output logic [5:0]  dbg_cpu_state,
    output logic [7:0]  dbg_cpu_ir,
    output logic [15:0] dbg_cpu_addr,
    output logic [7:0]  dbg_cpu_din,
    output logic [7:0]  dbg_cpu_dout,
    output logic        dbg_cpu_we,
    output logic        dbg_cpu_rdy,
    output logic        dbg_cpu_irq,
    output logic        dbg_cpu_nmi,
    output logic        dbg_cpu_waiting,
    output logic        dbg_cpu_stopped,

    // SDRAM port A — driven by the XRAM-over-SDRAM wrapper. fpga_top
    // connects these to sdram.v's port A; in sim the test harness
    // instantiates its own sdram + mock chip.
    input  logic        sdram_clk,
    output logic [24:0] sdram_addrA,
    output logic [7:0]  sdram_dinA,
    output logic        sdram_weA,
    output logic        sdram_oeA,
    input  logic [7:0]  sdram_doutA,
    input  logic        sdram_doneA,

    // SDRAM port B — driven by the SID curve reader (Phase 2.5 Step 3).
    output logic [24:0] sdram_addrB,
    output logic [7:0]  sdram_dinB,
    output logic        sdram_weB,
    output logic        sdram_oeB,
    input  logic [7:0]  sdram_doutB,

    // Host-service events pulse one clock when the CPU writes a non-zero
    // command byte. fpga_top.sv routes these to the debug bridge, which emits
    // async events to NovaHost.
    output logic        fio_event,
    output logic        nic_event
`ifndef SYNTHESIS
    ,
    // Simulation-only VRAM peek path. Excluded from synthesis so the
    // hardware map still exposes only the VGC VRAM port.
    input  logic        dbg_vram_read_en,
    input  logic [2:0]  dbg_vram_space,
    input  logic [16:0] dbg_vram_addr,
    output logic [7:0]  dbg_vram_rdata
`endif
);

    localparam ROM_BASE  = 16'hC000;
    localparam CHARIN    = 16'hA00F;

    // ROM bank switching
    localparam REG_ROMSWAP    = 16'hA03F;
    localparam ROMSWAP_NCC    = 8'h01;
    localparam ROMSWAP_BASIC  = 8'h02;
    localparam ROMSWAP_NCCEDIT = 8'h03;
    localparam ROMSWAP_EXT    = 8'h04;
    localparam ROMSWAP_LOGO   = 8'h05;

    // XMC address map
    localparam XMC_REG_BASE = 16'hBA00;  // XMC registers $BA00-$BA3F
    localparam XMC_REG_END  = 16'hBA3F;
    localparam XMC_WIN_BASE = 16'hBC00;  // 4x256-byte windows
    localparam XMC_WIN_END  = 16'hBFFF;
    localparam MUSIC_REG_BASE = 16'hBA50; // Hosted music status/notes $BA50-$BA62
    localparam MUSIC_REG_END  = 16'hBA62;
    localparam MUSIC_REG_COUNT = MUSIC_REG_END - MUSIC_REG_BASE + 1;
    localparam MATH_BASE    = 16'hBB20;  // Math coprocessor $BB20-$BB4F
    localparam MATH_END     = 16'hBB4F;
    // SID address map
    localparam SID1_BASE       = 16'hD400;  // SID1 registers $D400-$D41F
    localparam SID1_END        = 16'hD41F;
    localparam SID2_BASE       = 16'hD420;  // SID2 registers $D420-$D43F
    localparam SID2_END        = 16'hD43F;
    localparam SID2_MIRROR_BASE = 16'hD500; // Legacy SID2 mirror $D500-$D51F
    localparam SID2_MIRROR_END  = 16'hD51F;
    localparam SID_CFG         = 16'hD440;  // bit0=mode (0=6581, 1=8580), bit1=clock (0=PAL, 1=NTSC), bit7=HDMI audio test tone
    // Wavetable Synthesizer register bank
    localparam WTS_BASE        = 16'hA140;
    localparam WTS_END         = 16'hA1FF;
    // Board input registers ($BA9C-$BAA1). Live state is read-only; IRQ
    // control/status registers let software opt into change-triggered IRQs.
    localparam BOARD_INPUT_BUTTONS        = 16'hBA9C;
    localparam BOARD_INPUT_SWITCHES       = 16'hBA9D;
    localparam BOARD_INPUT_IRQ_ENABLE     = 16'hBA9E;
    localparam BOARD_INPUT_IRQ_STATUS     = 16'hBA9F;
    localparam BOARD_INPUT_BUTTON_CHANGES = 16'hBAA0;
    localparam BOARD_INPUT_SWITCH_CHANGES = 16'hBAA1;
    localparam BOARD_INPUT_BASE           = BOARD_INPUT_BUTTONS;
    localparam BOARD_INPUT_END            = BOARD_INPUT_SWITCH_CHANGES;
    localparam [7:0] BOARD_BUTTON_MASK    = 8'h7F;
    localparam [7:0] BOARD_SWITCH_MASK    = 8'h0F;
    localparam [7:0] BOARD_INPUT_IRQ_BUTTONS  = 8'h01;
    localparam [7:0] BOARD_INPUT_IRQ_SWITCHES = 8'h02;
    localparam [7:0] BOARD_INPUT_IRQ_MASK     = 8'h03;
    // USB HID diagnostics ($BAA3-$BAAF). These are intentionally read-only
    // and primarily for board bring-up on the ULX3S US2 low/full-speed USB
    // HID host path.
    localparam USB_HID_STATUS              = 16'hBAA3;
    localparam USB_HID_DEVICE_TYPE         = 16'hBAA4;
    localparam USB_HID_LAST_SCAN           = 16'hBAA5;
    localparam USB_HID_LAST_ASCII          = 16'hBAA6;
    localparam USB_HID_REPORT_COUNT        = 16'hBAA7;
    localparam USB_HID_KEY_COUNT           = 16'hBAA8;
    localparam USB_HID_CORE_STATUS         = 16'hBAA9;
    localparam USB_HID_REGS_BASE           = 16'hBAAA;
    localparam USB_HID_BASE                = USB_HID_STATUS;
    localparam USB_HID_END                 = 16'hBAAF;

    // CPU bus
    wire [15:0] cpu_addr;
    wire [7:0]  cpu_dout;
    wire        cpu_we;
    logic [7:0] cpu_din;
    wire [15:0] cpu_dbg_pc;
    wire  [7:0] cpu_dbg_a, cpu_dbg_x, cpu_dbg_y, cpu_dbg_s, cpu_dbg_flags;
    wire  [5:0] cpu_dbg_state;
    wire  [7:0] cpu_dbg_ir;
    wire        cpu_dbg_waiting, cpu_dbg_stopped;
    wire        vgc_sys_reset_req;
    wire [7:0]  wts_cpu_rdata;
    wire [7:0]  wts_dbg_rdata;
    wire [24:0] wts_addrB;
    wire [7:0]  wts_dinB;
    wire        wts_weB;
    wire        wts_oeB;
    wire signed [17:0] wts_audio_l;
    wire signed [17:0] wts_audio_r;

    // A VGC SYSRESET command is a machine reset, not just a custom-chip
    // scrub request. Latch it long enough that the CPU and every custom
    // block see a clean reset pulse before execution restarts at $FFFC.
    localparam int VGC_SYS_RESET_HOLD_CYCLES = 8;
    logic [$clog2(VGC_SYS_RESET_HOLD_CYCLES+1)-1:0] vgc_sys_reset_hold = '0;
    wire        vgc_sys_reset_active = vgc_sys_reset_req | (vgc_sys_reset_hold != '0);
    wire        custom_rst = rst | dbg_system_reset | vgc_sys_reset_active;
    wire        cpu_reset = rst | dbg_cpu_reset | dbg_system_reset | vgc_sys_reset_active;

    always_ff @(posedge clk) begin
        if (rst || dbg_system_reset) begin
            vgc_sys_reset_hold <= '0;
        end else if (vgc_sys_reset_req) begin
            vgc_sys_reset_hold <= VGC_SYS_RESET_HOLD_CYCLES[$bits(vgc_sys_reset_hold)-1:0];
        end else if (vgc_sys_reset_hold != '0) begin
            vgc_sys_reset_hold <= vgc_sys_reset_hold - 1'b1;
        end
    end

    wire [7:0] board_buttons_masked = board_buttons & BOARD_BUTTON_MASK;
    wire [7:0] board_switches_masked = board_switches & BOARD_SWITCH_MASK;
    wire       dbg_poke_board_input = dbg_poke_en &&
                                      (dbg_poke_addr >= BOARD_INPUT_BASE) &&
                                      (dbg_poke_addr <= BOARD_INPUT_END);
    wire       dbg_peek_board_input = dbg_peek_en &&
                                      (dbg_peek_addr >= BOARD_INPUT_BASE) &&
                                      (dbg_peek_addr <= BOARD_INPUT_END);
    wire       dbg_poke_usb_hid = dbg_poke_en &&
                                  (dbg_poke_addr >= USB_HID_BASE) &&
                                  (dbg_poke_addr <= USB_HID_END);

    logic [7:0] board_input_irq_enable;
    logic [7:0] board_input_irq_status;
    logic [7:0] board_input_button_changes;
    logic [7:0] board_input_switch_changes;
    logic [7:0] board_input_last_buttons;
    logic [7:0] board_input_last_switches;

    logic [7:0] board_input_irq_enable_next;
    logic [7:0] board_input_irq_status_next;
    logic [7:0] board_input_button_changes_next;
    logic [7:0] board_input_switch_changes_next;
    logic [7:0] board_input_last_buttons_next;
    logic [7:0] board_input_last_switches_next;

    wire       board_input_cpu_write = cpu_we && cpu_active &&
                                      (cpu_addr >= BOARD_INPUT_BASE) &&
                                      (cpu_addr <= BOARD_INPUT_END);
    wire       board_input_write = dbg_poke_board_input | board_input_cpu_write;
    wire [15:0] board_input_write_addr = dbg_poke_board_input ? dbg_poke_addr : cpu_addr;
    wire [7:0]  board_input_write_data = dbg_poke_board_input ? dbg_poke_data : cpu_dout;
    wire [7:0]  board_input_button_delta = board_buttons_masked ^ board_input_last_buttons;
    wire [7:0]  board_input_switch_delta = board_switches_masked ^ board_input_last_switches;
    wire        board_input_irq = ((board_input_irq_enable &
                                    board_input_irq_status &
                                    BOARD_INPUT_IRQ_MASK) != 8'h00);

    always_comb begin
        board_input_irq_enable_next = board_input_irq_enable;
        board_input_irq_status_next = board_input_irq_status & BOARD_INPUT_IRQ_MASK;
        board_input_button_changes_next = board_input_button_changes & BOARD_BUTTON_MASK;
        board_input_switch_changes_next = board_input_switch_changes & BOARD_SWITCH_MASK;
        board_input_last_buttons_next = board_input_last_buttons;
        board_input_last_switches_next = board_input_last_switches;

        if (board_input_write) begin
            unique case (board_input_write_addr)
                BOARD_INPUT_IRQ_ENABLE:
                    board_input_irq_enable_next = board_input_write_data & BOARD_INPUT_IRQ_MASK;
                BOARD_INPUT_IRQ_STATUS:
                    board_input_irq_status_next =
                        board_input_irq_status_next & ~(board_input_write_data & BOARD_INPUT_IRQ_MASK);
                BOARD_INPUT_BUTTON_CHANGES:
                    board_input_button_changes_next =
                        board_input_button_changes_next & ~(board_input_write_data & BOARD_BUTTON_MASK);
                BOARD_INPUT_SWITCH_CHANGES:
                    board_input_switch_changes_next =
                        board_input_switch_changes_next & ~(board_input_write_data & BOARD_SWITCH_MASK);
                default: begin end
            endcase
        end

        if (board_input_button_delta != 8'h00) begin
            board_input_button_changes_next =
                board_input_button_changes_next | (board_input_button_delta & BOARD_BUTTON_MASK);
            board_input_irq_status_next =
                board_input_irq_status_next | BOARD_INPUT_IRQ_BUTTONS;
            board_input_last_buttons_next = board_buttons_masked;
        end

        if (board_input_switch_delta != 8'h00) begin
            board_input_switch_changes_next =
                board_input_switch_changes_next | (board_input_switch_delta & BOARD_SWITCH_MASK);
            board_input_irq_status_next =
                board_input_irq_status_next | BOARD_INPUT_IRQ_SWITCHES;
            board_input_last_switches_next = board_switches_masked;
        end
    end

    always_ff @(posedge clk) begin
        if (custom_rst) begin
            board_input_irq_enable <= 8'h00;
            board_input_irq_status <= 8'h00;
            board_input_button_changes <= 8'h00;
            board_input_switch_changes <= 8'h00;
            board_input_last_buttons <= board_buttons_masked;
            board_input_last_switches <= board_switches_masked;
        end else begin
            board_input_irq_enable <= board_input_irq_enable_next;
            board_input_irq_status <= board_input_irq_status_next;
            board_input_button_changes <= board_input_button_changes_next;
            board_input_switch_changes <= board_input_switch_changes_next;
            board_input_last_buttons <= board_input_last_buttons_next;
            board_input_last_switches <= board_input_last_switches_next;
        end
    end

    function automatic logic [7:0] board_input_read_data(input logic [15:0] addr);
        begin
            unique case (addr)
                BOARD_INPUT_BUTTONS:
                    board_input_read_data = board_buttons_masked;
                BOARD_INPUT_SWITCHES:
                    board_input_read_data = board_switches_masked;
                BOARD_INPUT_IRQ_ENABLE:
                    board_input_read_data = board_input_irq_enable & BOARD_INPUT_IRQ_MASK;
                BOARD_INPUT_IRQ_STATUS:
                    board_input_read_data = board_input_irq_status & BOARD_INPUT_IRQ_MASK;
                BOARD_INPUT_BUTTON_CHANGES:
                    board_input_read_data = board_input_button_changes & BOARD_BUTTON_MASK;
                BOARD_INPUT_SWITCH_CHANGES:
                    board_input_read_data = board_input_switch_changes & BOARD_SWITCH_MASK;
                default:
                    board_input_read_data = 8'h00;
            endcase
        end
    endfunction

    function automatic logic [7:0] usb_hid_read_data(input logic [15:0] addr);
        begin
            unique case (addr)
                USB_HID_STATUS:
                    usb_hid_read_data = usb_hid_status;
                USB_HID_DEVICE_TYPE:
                    usb_hid_read_data = usb_hid_device_type;
                USB_HID_LAST_SCAN:
                    usb_hid_read_data = usb_hid_last_scan;
                USB_HID_LAST_ASCII:
                    usb_hid_read_data = usb_hid_last_ascii;
                USB_HID_REPORT_COUNT:
                    usb_hid_read_data = usb_hid_report_count;
                USB_HID_KEY_COUNT:
                    usb_hid_read_data = usb_hid_key_count;
                USB_HID_CORE_STATUS:
                    usb_hid_read_data = usb_hid_core_status;
                16'hBAAA:
                    usb_hid_read_data = usb_hid_regs[7:0];    // VID low
                16'hBAAB:
                    usb_hid_read_data = usb_hid_regs[15:8];   // VID high
                16'hBAAC:
                    usb_hid_read_data = usb_hid_regs[23:16];  // PID low
                16'hBAAD:
                    usb_hid_read_data = usb_hid_regs[31:24];  // PID high
                16'hBAAE:
                    usb_hid_read_data = usb_hid_regs[39:32];  // interface class
                16'hBAAF:
                    usb_hid_read_data = usb_hid_regs[55:48];  // interface protocol
                default:
                    usb_hid_read_data = 8'h00;
            endcase
        end
    endfunction

    // =========================================================================
    // 64KB main memory — dpram for BRAM inference
    // Port A: CPU read/write (+ blitter write when CPU stalled)
    // Port B: blitter reads / debug reads
    // =========================================================================
`ifdef SYNTHESIS
    // --- SYNTHESIS: use dpram instances for BRAM inference ---

    // CPU clock enable — halve to ~12.5 MHz for timing closure.
    // The DI→state→AB combinational path exceeds 25 MHz.
    logic cpu_ce;
    // xram_stall holds cpu_ce low whenever an XMC-window access is pending
    // or in flight in the xram_sdram wrapper, so the CPU doesn't advance
    // past the access before its data is ready.
    wire xram_stall;
    always_ff @(posedge clk) begin
        if (cpu_reset)      cpu_ce <= 0;
        else if (xram_stall) cpu_ce <= 0;
        else                cpu_ce <= ~cpu_ce;
    end

    // Held CPU address: freeze during stall cycle so dpram keeps
    // outputting the correct data (no overwrite by the new address).
    logic [15:0] held_cpu_addr;
    always_ff @(posedge clk) begin
        if (cpu_ce)
            held_cpu_addr <= cpu_addr;
    end
    // Use held address for all dpram reads and decode during stall
    wire [15:0] mem_addr = cpu_ce ? cpu_addr : held_cpu_addr;
    // MMIO read data is sampled on the cpu_ce=0 half-cycle; drive peripheral
    // read decoders from the registered address to avoid a RAM->CPU->MMIO
    // combinational feedback path while keeping writes on the live CPU bus.
    wire [15:0] mmio_read_addr = held_cpu_addr;

    // Main RAM: 64KB
    logic [15:0] ram_a_addr;
    logic [7:0]  ram_a_din;
    logic        ram_a_we;
    logic [7:0]  ram_a_dout;
    logic [15:0] ram_b_addr;
    logic [7:0]  ram_b_dout;

    dpram #(.WIDTH(8), .DEPTH(65536)) main_ram (
        .clk(clk),
        .addr_a(ram_a_addr), .din_a(ram_a_din), .we_a(ram_a_we), .dout_a(ram_a_dout),
        .addr_b(ram_b_addr), .dout_b(ram_b_dout)
    );

    // EhBASIC ROM: 16KB. Initialized from the bitstream so the FPGA can
    // self-boot if NovaHost is absent/stale; NovaHost may still overwrite it
    // at boot via dbg_rom_we (idx=0) before releasing CPU reset.
    logic [13:0] brom_b_addr;
    logic [7:0]  brom_b_dout;
    wire         brom_we = dbg_rom_we && (dbg_rom_idx == 1'b0);

    dpram #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/ehbasic.hex")) basic_rom_inst (
        .clk(clk),
        .addr_a(dbg_rom_addr), .din_a(dbg_rom_data), .we_a(brom_we), .dout_a(),
        .addr_b(brom_b_addr), .dout_b(brom_b_dout)
    );

    // Extension ROM: 16KB. Bitstream initialized, runtime-overwritable.
    //
    // Task 11b: ext_rom is now a DUAL-CLOCK BRAM (dpram_dc). Port A (write) is
    // clocked by sdram_clk so the Tier-2 page-in engine (page_dma, Task 11c —
    // an sdram_clk-domain module) can write it directly with no data-path CDC.
    // Port B (CPU read) stays on the pixel clk. The ECP5 DP16KD natively
    // supports independent per-port clocks, so this is one block RAM.
    logic [13:0] erom_b_addr;
    logic [7:0]  erom_b_dout;

    // --- Boot-bridge ROM-load CDC: pixel clk -> sdram_clk -------------------
    // debug_bridge runs on the pixel clk, so dbg_rom_we/idx/addr/data arrive in
    // the pixel domain. ext_rom port A is now on sdram_clk, so the boot writes
    // must cross domains. The boot stream is slow (one byte per many cycles at
    // 115200/SPI), so dbg_rom_addr/data are quasi-static while dbg_rom_we is
    // asserted. We synchronize the *level* of the extension-ROM write-enable
    // into sdram_clk with a 2-FF synchronizer + rising-edge detect to emit a
    // single 1-sdram_clk write pulse, sampling addr/data alongside (they are
    // stable). Same 2-FF (* async_reg *) idiom as debug_sdram_port_b_cdc.sv.
    wire        erom_we_pix = dbg_rom_we && (dbg_rom_idx == 1'b1);

    // Reset synchronizer into sdram_clk (custom_rst is a pixel-clk signal).
    (* async_reg = "true" *) reg [1:0] erom_rst_sync_sd = 2'b11;
    always @(posedge sdram_clk) begin
        erom_rst_sync_sd <= {erom_rst_sync_sd[0], custom_rst};
    end
    wire erom_rst_sd = erom_rst_sync_sd[1];

    // 2-FF level synchronizer for the write-enable + edge detect.
    (* async_reg = "true" *) reg [1:0] erom_we_sync_sd;
    reg                                erom_we_seen_sd;   // prev synced level
    reg                                erom_boot_we_sd;   // 1-cycle write pulse
    reg [13:0]                         erom_boot_addr_sd; // sampled @ rising edge
    reg [7:0]                          erom_boot_data_sd;
    always @(posedge sdram_clk) begin
        if (erom_rst_sd) begin
            erom_we_sync_sd   <= 2'b00;
            erom_we_seen_sd   <= 1'b0;
            erom_boot_we_sd   <= 1'b0;
            erom_boot_addr_sd <= 14'd0;
            erom_boot_data_sd <= 8'd0;
        end else begin
            erom_we_sync_sd <= {erom_we_sync_sd[0], erom_we_pix};
            erom_we_seen_sd <= erom_we_sync_sd[1];
            // Rising edge of the synchronized write-enable -> one write pulse.
            // addr/data are stable in the pixel domain while we is high, and
            // their bits have had >=2 sdram_clk edges of settle time, so they
            // are safe to sample here.
            if (erom_we_sync_sd[1] && !erom_we_seen_sd) begin
                erom_boot_we_sd   <= 1'b1;
                erom_boot_addr_sd <= dbg_rom_addr;
                erom_boot_data_sd <= dbg_rom_data;
            end else begin
                erom_boot_we_sd   <= 1'b0;
            end
        end
    end

    // ext_rom port-A write-source mux (now entirely in the sdram_clk domain):
    // page_dma (pgd_*) vs the sdram_clk-synchronized boot write. Both feed
    // port A on sdram_clk. The two sources never overlap; pgd_active selects.
    // pgd_* are tied off here (Task 11c/11d drive them); with pgd_active=0 the
    // muxes reduce to the boot-bridge path, so behaviour is the boot-only path
    // exactly as before — just now landing via the sdram_clk write port.
    wire        pgd_active    = 1'b0;
    wire        pgd_erom_we   = 1'b0;
    wire [13:0] pgd_erom_addr = 14'd0;
    wire [7:0]  pgd_erom_data = 8'd0;

    wire        erom_we_muxed   = pgd_active ? pgd_erom_we   : erom_boot_we_sd;
    wire [13:0] erom_addr_muxed = pgd_active ? pgd_erom_addr : erom_boot_addr_sd;
    wire [7:0]  erom_data_muxed = pgd_active ? pgd_erom_data : erom_boot_data_sd;

    dpram_dc #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/extension.hex")) ext_rom_inst (
        // Port A: write side on sdram_clk
        .clk_a(sdram_clk),
        .addr_a(erom_addr_muxed), .din_a(erom_data_muxed), .we_a(erom_we_muxed), .dout_a(),
        // Port B: CPU read side on pixel clk
        .clk_b(clk),
        .addr_b(erom_b_addr), .dout_b(erom_b_dout)
    );

    // XRAM: 512KB backed by SDRAM via xram_sdram wrapper. Phase 2.5 Step 2
    // replaces the old 4KB BRAM stub; interface signals keep their dpram
    // names at this layer so the CPU/blitter decode logic is unchanged.
    localparam XRAM_SIZE = 524288;       // 512 KB — full SDRAM-backed region
    logic [18:0] xram_a_addr;
    logic [7:0]  xram_a_din;
    logic        xram_a_we;
    logic        xram_a_re;
    logic [7:0]  xram_a_dout;
    logic        xram_busy;

    xram_sdram xram_sdram_inst (
        .clk        (clk),
        .sdram_clk  (sdram_clk),
        .rst        (custom_rst),
        .req_addr   (xram_a_addr),
        .req_din    (xram_a_din),
        .req_we     (xram_a_we),
        .req_re     (xram_a_re),
        .req_dout   (xram_a_dout),
        .busy       (xram_busy),
        .sdram_addr (sdram_addrA),
        .sdram_din  (sdram_dinA),
        .sdram_we   (sdram_weA),
        .sdram_oe   (sdram_oeA),
        .sdram_dout (sdram_doutA),
        .sdram_done (sdram_doneA)
    );

    logic ext_rom_active;

    initial begin
        ext_rom_active = 0;
    end

    // XMC register file ($BA00-$BA3F)
    logic [7:0] xmc_regs [0:63];

    localparam XMC_BANKS_REG = 6'h0D;
    localparam XMC_WINCTL_REG = 6'h16;
    localparam XMC_USEDL_REG  = 6'h0E;
    localparam XMC_USEDH_REG  = 6'h0F;
    localparam XMC_FREEL_REG  = 6'h10;
    localparam XMC_FREEH_REG  = 6'h11;
    initial begin
        for (int i = 0; i < 64; i++) xmc_regs[i] = 0;
        xmc_regs[XMC_BANKS_REG] = 8'd8;
        xmc_regs[XMC_WINCTL_REG] = 8'h0F;
    end

    // NovaHost publishes active SID/MIDI state here for assembly UIs.
    // Keep it off main RAM so frequent visualizer pokes cannot perturb
    // the CPU instruction/data read port while music is playing.
    logic [7:0] music_regs [0:MUSIC_REG_COUNT-1];
    initial begin
        for (int i = 0; i < MUSIC_REG_COUNT; i++)
            music_regs[i] = 8'h00;
    end

    // XMC address decode — use mem_addr (frozen during CPU stall)
    wire xmc_reg_sel = (mem_addr >= XMC_REG_BASE && mem_addr <= XMC_REG_END);
    wire xmc_win_sel = (mem_addr >= XMC_WIN_BASE && mem_addr <= XMC_WIN_END);
    wire [5:0]  xmc_reg_off = mem_addr[5:0];
    wire [1:0]  xmc_page    = mem_addr[9:8];
    wire [7:0]  xmc_offset  = mem_addr[7:0];

    // Each 256-byte XMC window is page-aligned. The low base register is
    // writable/readable for software symmetry, but address translation uses
    // the CPU address low byte as the window offset.
    logic [7:0] xmc_win_mid;
    logic [7:0] xmc_win_high;

    always_comb begin
        unique case (xmc_page)
            2'd0: begin xmc_win_mid = xmc_regs[6'h19]; xmc_win_high = xmc_regs[6'h1A]; end
            2'd1: begin xmc_win_mid = xmc_regs[6'h1C]; xmc_win_high = xmc_regs[6'h1D]; end
            2'd2: begin xmc_win_mid = xmc_regs[6'h1F]; xmc_win_high = xmc_regs[6'h20]; end
            2'd3: begin xmc_win_mid = xmc_regs[6'h22]; xmc_win_high = xmc_regs[6'h23]; end
        endcase
    end

    wire [18:0] xmc_addr_raw = {xmc_win_high[2:0], xmc_win_mid, xmc_offset};
    // Full 19-bit XMC address maps directly into the 512 KB SDRAM region.
    wire [18:0] xmc_addr = xmc_addr_raw;
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read — pipelined for dpram 1-cycle latency
    // mem_addr is held stable during CPU stall cycles (cpu_ce=0)
    // so dpram outputs don't get overwritten by the new address.
    // =========================================================================
    wire cpu_in_rom = (mem_addr >= ROM_BASE);
    wire music_reg_sel = (mem_addr >= MUSIC_REG_BASE && mem_addr <= MUSIC_REG_END);
    wire [$clog2(MUSIC_REG_COUNT)-1:0] music_reg_off = mem_addr - MUSIC_REG_BASE;
    wire dma_reg_sel = (mem_addr >= 16'hBA63 && mem_addr <= 16'hBA75);
    wire blt_reg_sel = (mem_addr >= 16'hBA83 && mem_addr <= 16'hBA9B) ||
                       (mem_addr == 16'hBAA2);
    wire math_reg_sel = (mem_addr >= MATH_BASE && mem_addr <= MATH_END);
    wire nic_reg_sel = (mem_addr >= 16'hA100 && mem_addr <= 16'hA13F);
    wire wts_reg_sel = (mem_addr >= WTS_BASE && mem_addr <= WTS_END);
    wire fio_reg_sel = (mem_addr >= 16'hB9A0 && mem_addr <= 16'hB9EF);
    wire board_input_sel = (mem_addr >= BOARD_INPUT_BASE && mem_addr <= BOARD_INPUT_END);
    wire usb_hid_sel = (mem_addr >= USB_HID_BASE && mem_addr <= USB_HID_END);
    wire vgc_read_sel = (mem_addr >= 16'hA000 && mem_addr <= 16'hA01F) ||
                        (mem_addr >= 16'hA040 && mem_addr <= 16'hA0BF) ||
                        (mem_addr >= 16'hA0E0 && mem_addr <= 16'hA0ED) ||
                        (mem_addr >= 16'hA0F0 && mem_addr <= 16'hA0FF) ||
                        // Direct screen window + plane-select — carve from RAM, route to VGC.
                        (mem_addr >= 16'hA200 && mem_addr <= 16'hB1A0);

    // Register the decode signals for next-cycle mux
    logic r_xmc_win_sel, r_xmc_win_enabled, r_xmc_reg_sel;
    logic r_dma_reg_sel, r_blt_reg_sel, r_math_reg_sel;
    logic r_music_reg_sel;
    logic r_cpu_in_rom, r_ext_rom_active;
    logic r_vgc_read_sel;
    logic r_fio_reg_sel;
    logic r_nic_reg_sel;
    logic r_wts_reg_sel;
    logic r_board_input_sel;
    logic r_usb_hid_sel;
    logic [5:0] r_xmc_reg_off;

    // Register decode signals every cycle — they align with dpram outputs
    // since both are captured from the same cpu_addr at the same posedge.
    always_ff @(posedge clk) begin
        r_xmc_win_sel     <= xmc_win_sel;
        r_xmc_win_enabled <= xmc_win_enabled;
        r_xmc_reg_sel     <= xmc_reg_sel;
        r_music_reg_sel   <= music_reg_sel;
        r_dma_reg_sel     <= dma_reg_sel;
        r_blt_reg_sel     <= blt_reg_sel;
        r_math_reg_sel    <= math_reg_sel;
        r_cpu_in_rom      <= cpu_in_rom;
        r_ext_rom_active  <= ext_rom_active;
        r_vgc_read_sel    <= vgc_read_sel;
        r_fio_reg_sel     <= fio_reg_sel;
        r_nic_reg_sel     <= nic_reg_sel;
        r_wts_reg_sel     <= wts_reg_sel;
        r_board_input_sel <= board_input_sel;
        r_usb_hid_sel     <= usb_hid_sel;
        r_xmc_reg_off     <= xmc_reg_off;
    end

    // Present addresses to dpram read ports.
    // Normally the CPU's mem_addr drives port B for instruction fetch.
    // When a debug peek targets the ROM range, route the debug address in
    // instead so peek returns the actual ROM byte rather than whatever the
    // CPU happens to be fetching.
    wire dbg_rom_read = dbg_peek_en && (dbg_peek_addr >= ROM_BASE);
    assign brom_b_addr = dbg_rom_read ? dbg_peek_addr[13:0] : mem_addr[13:0];
    assign erom_b_addr = dbg_rom_read ? dbg_peek_addr[13:0] : mem_addr[13:0];

    // XRAM: drive the xram_sdram wrapper. The wrapper is edge-triggered:
    // req_re / req_we must be asserted for exactly one pixel clock per
    // transaction. Some 6502 addressing modes keep the same XMC address on the
    // bus for multiple CPU states (for example ($zp),Y store does a read cycle
    // before the write). Track the fulfilled CPU bus cycle by state/address/WE
    // so each real CPU cycle fires once, even when the address stays in-window.
    logic xmc_cycle_done;
    logic xmc_pending;
    logic [5:0] xmc_last_state;
    logic [15:0] xmc_last_addr;
    logic xmc_last_we;

    wire  xmc_access     = xmc_win_sel && xmc_win_enabled;
    wire  xmc_cpu_access = xmc_access && cpu_active;
    wire  xmc_same_cycle =
        xmc_cycle_done &&
        (xmc_last_state == cpu_dbg_state) &&
        (xmc_last_addr  == mem_addr) &&
        (xmc_last_we    == cpu_we);
    wire  xmc_new_cycle  = xmc_cpu_access && !xmc_same_cycle && !xmc_pending;
    wire  xmc_need_fire  = xmc_pending || xmc_new_cycle;
    wire  xmc_fire       = xmc_need_fire && !xram_busy;
    wire  xmc_cpu_read   = xmc_fire && !cpu_we;
    wire  xmc_cpu_write  = xmc_fire &&  cpu_we;

    // Bus-master XRAM write (blitter or DMA). Previously used a `bm_xram_armed`
    // single-shot latch that cleared on fire and re-armed only when `bm_xram_we`
    // dropped. That worked for COPY (which cycles through S_READ between
    // bytes, dropping we=0) but BROKE FILL: fill-mode stays in S_WRITE
    // continuously with we=1 across bytes, so armed stayed 0 after the first
    // byte and every subsequent byte was dropped. XRESET's 1280-byte XRAM
    // fill left pages 1-4 uncleared, which then tripped `xmc_alloc`'s
    // directory-entry scan into ERR_NOSPACE and returned FCER from BASIC.
    //
    // xram_sdram.sv already protects against double-latch: it only accepts
    // req_we in S_IDLE (busy=0). Holding we=1 during busy is harmless —
    // the wrapper ignores it. With the blitter/DMA back-pressure stall
    // (blitter.sv / dma.sv hold we/addr/data stable while xram_busy=1),
    // exactly one byte lands per intended write. Removing the arm latch.
    wire  bm_xram_fire = bm_xram_we && !xram_busy;

    always_ff @(posedge clk) begin
        if (custom_rst) begin
            xmc_cycle_done <= 1'b0;
            xmc_pending    <= 1'b0;
            xmc_last_state <= 6'd0;
            xmc_last_addr  <= 16'd0;
            xmc_last_we    <= 1'b0;
        end else begin
            if (!xmc_access) begin
                xmc_cycle_done <= 1'b0;
                xmc_pending    <= 1'b0;
            end else begin
                if (xmc_new_cycle && xram_busy)
                    xmc_pending <= 1'b1;

                if (xmc_fire) begin
                    xmc_cycle_done <= 1'b1;
                    xmc_pending    <= 1'b0;
                    xmc_last_state <= cpu_dbg_state;
                    xmc_last_addr  <= mem_addr;
                    xmc_last_we    <= cpu_we;
                end
            end
        end
    end

    // CPU-side stall: hold cpu_ce low any time a CPU XMC access is
    // outstanding (needed but not fired, or fired and still busy).
    assign xram_stall = xmc_access && (xram_busy || xmc_need_fire);

    // Bus-master read fire gate — same shape as bm_xram_fire. Separate so
    // a read during write-busy doesn't accidentally convert to a write.
    wire bm_xram_read_fire = bm_xram_re && !xram_busy;

    always_comb begin
        // CPU path has priority; bus master gets the bus when CPU isn't firing.
        if (xmc_cpu_read || xmc_cpu_write) begin
            xram_a_addr = xmc_addr;
            xram_a_din  = cpu_dout;
            xram_a_we   = xmc_cpu_write;
            xram_a_re   = xmc_cpu_read;
        end else if (bm_xram_fire) begin
            xram_a_addr = bm_xram_addr[18:0];
            xram_a_din  = bm_xram_wdata;
            xram_a_we   = 1'b1;
            xram_a_re   = 1'b0;
        end else if (bm_xram_read_fire) begin
            xram_a_addr = bm_xram_addr[18:0];
            xram_a_din  = 8'd0;
            xram_a_we   = 1'b0;
            xram_a_re   = 1'b1;
        end else begin
            xram_a_addr = 19'd0;
            xram_a_din  = 8'd0;
            xram_a_we   = 1'b0;
            xram_a_re   = 1'b0;
        end
    end

    // Registered XMC register read (for cpu_din mux)
    logic [7:0] r_xmc_reg_data;
    always_ff @(posedge clk)
        r_xmc_reg_data <= xmc_regs[xmc_reg_off];

    logic [7:0] r_music_reg_data;
    always_ff @(posedge clk) begin
        if (music_reg_sel)
            r_music_reg_data <= music_regs[music_reg_off];
        else
            r_music_reg_data <= 8'h00;
    end

    // Registered bus-master/math read data. These LOGO-critical peripherals
    // decode from mem_addr, which is held stable across the cpu_ce=0
    // half-cycle; capture every pixel clock so the staged cpu_din mux sees
    // the peripheral result on the CPU's next enabled edge.
    logic [7:0] r_blt_cpu_rdata;
    always_ff @(posedge clk)
        r_blt_cpu_rdata <= blt_cpu_rdata;

    // Registered DMA register read. DMA is not on the LOGO turtle path, so
    // keep its original cpu_ce-gated capture to avoid adding fanout to the
    // already-tight pixel-clock route.
    logic [7:0] r_dma_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_dma_cpu_rdata <= dma_cpu_rdata;

    logic [7:0] r_math_cpu_rdata;
    always_ff @(posedge clk)
        r_math_cpu_rdata <= math_cpu_rdata;

    // Registered VGC read data. VGC now holds read data from a registered
    // address slice, so capture every pixel clock; this lets the cpu_ce=0 half
    // cycle carry the peripheral result into the CPU's next enabled edge.
    logic [7:0] r_vgc_cpu_rdata;
    always_ff @(posedge clk)
        r_vgc_cpu_rdata <= vgc_cpu_rdata;

    // ==========================================================================
    // Network Interface Controller register bank ($A100-$A13F). CPU writes to
    // NIC_CMD pulse nic_event; NovaHost services the command via debug-bridge
    // peeks/pokes and updates slot/global status. The IRQ output is OR'd into
    // the CPU IRQ line below.
    // ==========================================================================
    wire [7:0] nic_cpu_rdata;
    wire [7:0] nic_dbg_rdata;
    wire       nic_irq;
    wire       nic_rdy;
    wire       nic_dma_grant;
    wire [15:0] nic_ram_addr;
    wire [7:0]  nic_ram_wdata;
    wire        nic_ram_we;
    wire        nic_ram_re;
    wire [7:0]  nic_ram_rdata;
    wire       dbg_poke_nic = dbg_poke_en &&
                              (dbg_poke_addr >= 16'hA100) &&
                              (dbg_poke_addr <= 16'hA13F);
    wire       dbg_peek_nic = dbg_peek_en &&
                              (dbg_peek_addr >= 16'hA100) &&
                              (dbg_peek_addr <= 16'hA13F);

    nic nic_inst (
        .clk       (clk),
        .rst       (custom_rst),
        .cpu_ce    (cpu_ce),
        .cpu_addr  (cpu_addr),
        .cpu_wdata (cpu_dout),
        .cpu_we    (cpu_we),
        .cpu_rdata (nic_cpu_rdata),
        .dbg_we    (dbg_poke_nic),
        .dbg_addr  (dbg_poke_addr[5:0]),
        .dbg_wdata (dbg_poke_data),
        .dbg_raddr (dbg_peek_addr[5:0]),
        .dbg_rdata (nic_dbg_rdata),
        .dbg_buf_we    (dbg_nic_buf_we),
        .dbg_buf_re    (dbg_nic_buf_re),
        .dbg_buf_sel   (dbg_nic_buf_sel),
        .dbg_buf_addr  (dbg_nic_buf_addr),
        .dbg_buf_wdata (dbg_nic_buf_data),
        .dbg_buf_rdata (dbg_nic_buf_rdata),
        .dma_grant (nic_dma_grant),
        .rdy_out   (nic_rdy),
        .ram_addr  (nic_ram_addr),
        .ram_rdata (nic_ram_rdata),
        .ram_wdata (nic_ram_wdata),
        .ram_we    (nic_ram_we),
        .ram_re    (nic_ram_re),
        .nic_event (nic_event),
        .irq       (nic_irq)
    );

    logic [7:0] r_nic_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_nic_cpu_rdata <= nic_cpu_rdata;

    // Registered WTS read data. WTS holds read data from a registered address
    // slice, so capture every pixel clock; this lets the cpu_ce=0 half cycle
    // carry the peripheral result into the CPU's next enabled edge.
    logic [7:0] r_wts_cpu_rdata;
    always_ff @(posedge clk)
        r_wts_cpu_rdata <= wts_cpu_rdata;

    logic [7:0] r_board_input_data;
    always_ff @(posedge clk) begin
        if (board_input_sel)
            r_board_input_data <= board_input_read_data(mem_addr);
        else
            r_board_input_data <= 8'h00;
    end

    logic [7:0] r_usb_hid_data;
    always_ff @(posedge clk) begin
        if (usb_hid_sel)
            r_usb_hid_data <= usb_hid_read_data(mem_addr);
        else
            r_usb_hid_data <= 8'h00;
    end

    // ==========================================================================
    // File I/O register bank ($B9A0-$B9EF). CPU writes captured here;
    // fio_event pulses on CPU write of non-zero to $B9A0 (FioCmd).
    // Debug-bridge pokes to this range land in fio.sv via dbg_poke
    // (different offset calculation), NOT in main RAM.
    // ==========================================================================
    wire [7:0] fio_cpu_rdata;
    wire [7:0] fio_dbg_rdata;
    wire       dbg_poke_fio = dbg_poke_en &&
                              (dbg_poke_addr >= 16'hB9A0) &&
                              (dbg_poke_addr <= 16'hB9EF);
    wire       dbg_poke_music = dbg_poke_en &&
                                (dbg_poke_addr >= MUSIC_REG_BASE) &&
                                (dbg_poke_addr <= MUSIC_REG_END);
    wire       dbg_peek_music = dbg_peek_en &&
                                (dbg_peek_addr >= MUSIC_REG_BASE) &&
                                (dbg_peek_addr <= MUSIC_REG_END);
    wire [$clog2(MUSIC_REG_COUNT)-1:0] dbg_music_reg_off =
        dbg_poke_en ? (dbg_poke_addr - MUSIC_REG_BASE) :
                      (dbg_peek_addr - MUSIC_REG_BASE);
    wire       dbg_poke_vgc = dbg_poke_en &&
                              (((dbg_poke_addr >= 16'hA000) && (dbg_poke_addr <= 16'hA01F)) ||
                               ((dbg_poke_addr >= 16'hA040) && (dbg_poke_addr <= 16'hA0BF)) ||
                               ((dbg_poke_addr >= 16'hA0E0) && (dbg_poke_addr <= 16'hA0ED)) ||
                               ((dbg_poke_addr >= 16'hA0F0) && (dbg_poke_addr <= 16'hA0FF)));
    wire       dbg_poke_blt = dbg_poke_en &&
                              (((dbg_poke_addr >= 16'hBA83) &&
                                (dbg_poke_addr <= 16'hBA9B)) ||
                               (dbg_poke_addr == 16'hBAA2));
    wire       dbg_peek_blt = dbg_peek_en &&
                              (((dbg_peek_addr >= 16'hBA83) &&
                                (dbg_peek_addr <= 16'hBA9B)) ||
                               (dbg_peek_addr == 16'hBAA2));
    wire       dbg_poke_math = dbg_poke_en &&
                               (dbg_poke_addr >= MATH_BASE) &&
                               (dbg_poke_addr <= MATH_END);
    wire       dbg_peek_math = dbg_peek_en &&
                               (dbg_peek_addr >= MATH_BASE) &&
                               (dbg_peek_addr <= MATH_END);
    wire       dbg_poke_wts = dbg_poke_en &&
                              (dbg_poke_addr >= WTS_BASE) &&
                              (dbg_poke_addr <= WTS_END);
    wire       dbg_peek_wts = dbg_peek_en &&
                              (dbg_peek_addr >= WTS_BASE) &&
                              (dbg_peek_addr <= WTS_END);
    wire [7:0] dbg_board_input_data = board_input_read_data(dbg_peek_addr);
    wire       dbg_peek_usb_hid = dbg_peek_en &&
                                  (dbg_peek_addr >= USB_HID_BASE) &&
                                  (dbg_peek_addr <= USB_HID_END);
    wire [7:0] dbg_usb_hid_data = usb_hid_read_data(dbg_peek_addr);

    fio fio_inst (
        .clk       (clk),
        .rst       (custom_rst),
        .cpu_ce    (cpu_ce),
        .cpu_addr  (cpu_addr),
        .cpu_wdata (cpu_dout),
        .cpu_we    (cpu_we),
        .cpu_rdata (fio_cpu_rdata),
        .dbg_we    (dbg_poke_fio),
        .dbg_addr  (dbg_poke_addr[6:0] - 7'h20),   // $B9A0[6:0]=$20
        .dbg_wdata (dbg_poke_data),
        .dbg_raddr (dbg_peek_addr[6:0] - 7'h20),
        .dbg_rdata (fio_dbg_rdata),
        .fio_event (fio_event)
    );

    logic [7:0] r_fio_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_fio_cpu_rdata <= fio_cpu_rdata;

    // Bus-master register reads share one slot in the cpu_din mux chain so
    // DMA doesn't add a LUT level to the already-long ram_a_dout path.
    // DMA ($BA63-$BA75) and blitter ($BA83-$BA9B plus $BAA2) disjoint, so the sub-mux
    // is a simple 2:1 that resolves in parallel with the main chain.
    wire        r_bm_reg_sel   = r_dma_reg_sel | r_blt_reg_sel;
    wire [7:0]  r_bm_cpu_rdata = r_dma_reg_sel ? r_dma_cpu_rdata
                                               : r_blt_cpu_rdata;

    // CPU read data mux — two-level grouped design to keep the critical
    // path short. The previous 10-branch priority chain put ram_a_dout
    // (the else-case fallthrough) through all 10 muxes in series — ~10
    // LUT levels, the worst clk_pixel path. Splitting into three parallel
    // group muxes + one 3:1 final cuts that to ~6 LUT levels.
    //
    // Selects are mutually exclusive (disjoint address ranges), so the
    // within-group priority order is for readability only.
    //
    // Group A — XMC window/regs, music status, bus-master, and math.
    wire [7:0] xmc_group_data =
        (r_xmc_win_sel && r_xmc_win_enabled) ? xram_a_dout  :
         r_xmc_win_sel                        ? 8'hFF        :
         r_xmc_reg_sel                        ? r_xmc_reg_data :
         r_music_reg_sel                      ? r_music_reg_data :
         r_bm_reg_sel                         ? r_bm_cpu_rdata :
         r_board_input_sel                    ? r_board_input_data :
         r_usb_hid_sel                        ? r_usb_hid_data :
                                                r_math_cpu_rdata;
    wire xmc_group_sel = r_xmc_win_sel | r_xmc_reg_sel | r_music_reg_sel |
                         r_bm_reg_sel | r_board_input_sel | r_usb_hid_sel |
                         r_math_reg_sel;

    // Group B — low-memory custom chip register space. SID register reads DELIBERATELY fall
    // through to the ROM / RAM group: $D400-$D43F overlaps BASIC ROM
    // (EhBASIC's array-handling code — LAB_1F45/1F48/1F7B — happens to
    // land at $D3FF-$D430), so an unconditional SID-read intercept
    // returns SID register values for legitimate opcode fetches into
    // that ROM block. CPU executes SID-reg-as-opcode, BRKs, and drops
    // back to Ready. The symptom is silent array failure (DIM/PRINT
    // returns no output) and caused the math.array-operations and
    // xram STASH/FETCH integration test failures. Removing SID from the
    // read mux makes BASIC PEEK($D400) return the ROM byte — the same
    // trade-off Avalonia makes. SID is still writable, per-register
    // shadow is still in sid_chip.sv for future use via an alternate
    // non-ROM-overlapping mirror.
    // $D440 (SID config) is also inside ROM — ROM byte at that offset
    // is $A2 = LDX #$00, part of LAB_1F7C (array-index multiply). An
    // interception would replace a live instruction with the config
    // register's stored byte. Fall through to ROM for all SID addresses.
    wire [7:0] io_group_data = r_nic_reg_sel ? r_nic_cpu_rdata :
                               r_wts_reg_sel ? r_wts_cpu_rdata :
                               r_fio_reg_sel ? r_fio_cpu_rdata :
                                               r_vgc_cpu_rdata;
    wire io_group_sel        = r_vgc_read_sel | r_fio_reg_sel |
                               r_nic_reg_sel | r_wts_reg_sel;

    // Group C — ROM (active bank) + main RAM (default fallback).
    wire [7:0] mem_group_data =
        r_cpu_in_rom ? (r_ext_rom_active ? erom_b_dout : brom_b_dout)
                     : ram_a_dout;

    // Level-2 select: one 3-way mux. ram_a_dout critical path is now
    // mem_group_data → level-2 only (~2 LUT levels) instead of walking
    // a 10-deep else-if tail.
    always_comb begin
        if      (xmc_group_sel) cpu_din = xmc_group_data;
        else if (io_group_sel)  cpu_din = io_group_data;
        else                    cpu_din = mem_group_data;
    end

    // =========================================================================
    // Main RAM port A: CPU reads/writes + blitter writes
    // When blitter is writing to RAM, CPU is stalled — port A available
    // =========================================================================
    always_comb begin
        ram_a_addr = mem_addr;  // default: held address for reads
        ram_a_din  = cpu_dout;
        ram_a_we   = 1'b0;

        if (bm_ram_we && bm_ram_addr < ROM_BASE) begin
            // Bus master (blitter or DMA) write — CPU stalled
            ram_a_addr = bm_ram_addr;
            ram_a_din  = bm_ram_wdata;
            ram_a_we   = 1'b1;
        end else if (dbg_poke_en && !dbg_poke_vgc && !dbg_poke_nic && !dbg_poke_wts && !dbg_poke_fio && !dbg_poke_music && !dbg_poke_blt && !dbg_poke_math && !dbg_poke_board_input && !dbg_poke_usb_hid && dbg_poke_addr < ROM_BASE) begin
            // Debug poke
            ram_a_addr = dbg_poke_addr;
            ram_a_din  = dbg_poke_data;
            ram_a_we   = 1'b1;
        end else if (cpu_we && cpu_active && cpu_addr < ROM_BASE &&
                     !xmc_win_sel && !xmc_reg_sel && !vgc_read_sel &&
                     !nic_reg_sel && !wts_reg_sel && !fio_reg_sel &&
                     !music_reg_sel && !dma_reg_sel && !blt_reg_sel &&
                     !board_input_sel && !usb_hid_sel && !math_reg_sel) begin
            // CPU write to RAM
            ram_a_we = 1'b1;
        end
    end

    // Keyboard input — handled entirely by VGC's sfifo (256-byte ring
    // buffer in rtl/thirdparty/sfifo.v). Previously duplicated here with a
    // single-entry latch that had the same drop-rate bug as vgc.sv's pre-
    // fifo char_in_reg. CHARIN reads now flow through r_vgc_read_sel →
    // vgc_cpu_rdata → char_in_reg which reads the sfifo head.

    // =========================================================================
    // Main RAM port B: blitter reads / debug reads
    // Priority: blitter/DMA > debug
    // =========================================================================
    always_comb begin
        if (bm_ram_read_active)
            ram_b_addr = bm_ram_addr;
        else
            ram_b_addr = dbg_peek_addr;
    end

    // Bus-master RAM read: blitter or DMA presents ram_addr during their
    // S_READ state. dpram port B output (ram_b_dout) is available next cycle.
    // Both FSMs have S_READ→S_WRITE pipelines that consume data in S_WRITE.
    wire bm_ram_read_active = ((blt_rdy == 1'b0) && !blt_ram_we)
                           || ((dma_rdy == 1'b0) && !dma_ram_we)
                           || (nic_dma_grant && nic_ram_re);
    wire [7:0] blt_ram_rdata  = ram_b_dout;
    wire [7:0] blt_xram_rdata = xram_a_dout;
    wire [7:0] dma_ram_rdata  = ram_b_dout;
    wire [7:0] dma_xram_rdata = xram_a_dout;
    assign nic_ram_rdata = ram_b_dout;

    // =========================================================================
    // Memory writes — non-RAM (XMC regs, ROM bank switching)
    // =========================================================================
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            for (int i = 0; i < 64; i++) xmc_regs[i] <= 8'h00;
            xmc_regs[XMC_BANKS_REG] <= 8'd8;
            xmc_regs[XMC_WINCTL_REG] <= 8'h0F;
            xmc_regs[XMC_FREEL_REG] <= 8'h00;
            xmc_regs[XMC_FREEH_REG] <= 8'h04;
            for (int i = 0; i < MUSIC_REG_COUNT; i++)
                music_regs[i] <= 8'h00;
            ext_rom_active <= 0;
        end else begin
        if (cpu_we && cpu_active) begin
            if (xmc_reg_sel) begin
                if (xmc_reg_off != XMC_BANKS_REG)
                    xmc_regs[xmc_reg_off] <= cpu_dout;
            end
        end

        if (dbg_poke_music)
            music_regs[dbg_music_reg_off] <= dbg_poke_data;

        // ROM bank switching: write to $A03F toggles active ROM bank.
        // The FPGA has one primary ROM bank plus one extension bank; NovaHost
        // chooses which runtime occupies the primary bank at boot. Runtime
        // values for BASIC/NCC/LOGO all mean "return to primary".
        if (cpu_we && cpu_active && cpu_addr == REG_ROMSWAP) begin
            if (cpu_dout == ROMSWAP_EXT)
                ext_rom_active <= 1;
            else if (cpu_dout == ROMSWAP_BASIC || cpu_dout == ROMSWAP_NCC ||
                     cpu_dout == ROMSWAP_NCCEDIT || cpu_dout == ROMSWAP_LOGO)
                ext_rom_active <= 0;
        end

        end
    end

    // =========================================================================
    // Keyboard — write to RAM via port A needs special handling
    // Since we can't write to dpram directly from here, we snoop key input
    // and write CHARIN via a registered path
    // =========================================================================
    // Key input buffer — small register, written to RAM on next cycle
    logic [7:0] key_buf;
    logic       key_buf_valid;
    logic       clear_input;

    initial begin
        key_buf = 0;
        key_buf_valid = 0;
        clear_input = 0;
    end

    // For keyboard CHARIN: the CPU reads $A00F through the VGC, not through
    // main RAM. The VGC has its own char_in_reg. In top.sv the old code wrote
    // key_data to ram[CHARIN] and cleared it on CPU read — but the CPU read
    // path for $A00F goes through the VGC (it's in VGC address space $A000-$A01F).
    // So ram[CHARIN] was only used for the "is there a key" check.
    // With dpram we can't easily do this. Instead, keyboard input is handled
    // entirely by the VGC's char_in_reg. The ram[CHARIN] write was redundant.
    // We keep the clear_input logic for behavioral compatibility with simulation.
    // (On FPGA, the CPU reads CHARIN from VGC, not from RAM.)

`else
    // --- SIMULATION: keep original register arrays for Verilator compatibility ---

    logic [7:0] ram [0:65535];
    logic [7:0] basic_rom [0:16383];
    logic [7:0] ext_rom [0:16383];
    logic       ext_rom_active;

    localparam XRAM_SIZE = 524288;
    logic [7:0] xram [0:XRAM_SIZE-1];
    wire xram_busy = 1'b0;
    wire dbg_poke_vgc = dbg_poke_en &&
                        (((dbg_poke_addr >= 16'hA000) && (dbg_poke_addr <= 16'hA01F)) ||
                         ((dbg_poke_addr >= 16'hA040) && (dbg_poke_addr <= 16'hA0BF)) ||
                         ((dbg_poke_addr >= 16'hA0E0) && (dbg_poke_addr <= 16'hA0ED)) ||
                         ((dbg_poke_addr >= 16'hA0F0) && (dbg_poke_addr <= 16'hA0FF)));
    wire dbg_poke_blt = dbg_poke_en &&
                        (((dbg_poke_addr >= 16'hBA83) &&
                          (dbg_poke_addr <= 16'hBA9B)) ||
                         (dbg_poke_addr == 16'hBAA2));
    wire dbg_peek_blt = dbg_peek_en &&
                        (((dbg_peek_addr >= 16'hBA83) &&
                          (dbg_peek_addr <= 16'hBA9B)) ||
                         (dbg_peek_addr == 16'hBAA2));
    wire dbg_poke_math = dbg_poke_en &&
                         (dbg_poke_addr >= MATH_BASE) &&
                         (dbg_poke_addr <= MATH_END);
    wire dbg_peek_math = dbg_peek_en &&
                         (dbg_peek_addr >= MATH_BASE) &&
                         (dbg_peek_addr <= MATH_END);
    wire dbg_poke_wts = dbg_poke_en &&
                        (dbg_poke_addr >= WTS_BASE) &&
                        (dbg_poke_addr <= WTS_END);
    wire dbg_peek_wts = dbg_peek_en &&
                        (dbg_peek_addr >= WTS_BASE) &&
                        (dbg_peek_addr <= WTS_END);
    wire dbg_poke_music = dbg_poke_en &&
                          (dbg_poke_addr >= MUSIC_REG_BASE) &&
                          (dbg_poke_addr <= MUSIC_REG_END);
    wire dbg_peek_music = dbg_peek_en &&
                          (dbg_peek_addr >= MUSIC_REG_BASE) &&
                          (dbg_peek_addr <= MUSIC_REG_END);
    wire [7:0] dbg_board_input_data = board_input_read_data(dbg_peek_addr);
    wire [$clog2(MUSIC_REG_COUNT)-1:0] dbg_music_reg_off =
        dbg_poke_en ? (dbg_poke_addr - MUSIC_REG_BASE) :
                      (dbg_peek_addr - MUSIC_REG_BASE);

    initial begin
        for (int i = 0; i < 65536; i++)
            ram[i] = 8'h00;
        for (int i = 0; i < 16384; i++) begin
            basic_rom[i] = 8'h00;
            ext_rom[i] = 8'h00;
        end
        $readmemh("rom/ehbasic.hex", basic_rom, 0, 16'h3FFF);
        $readmemh("rom/extension.hex", ext_rom, 0, 16'h3FFF);
        ext_rom_active = 0;
    end

    initial begin
        for (int i = 0; i < XRAM_SIZE; i++)
            xram[i] = 8'h00;
    end

    // XMC register file ($BA00-$BA3F)
    logic [7:0] xmc_regs [0:63];

    localparam XMC_BANKS_REG = 6'h0D;
    localparam XMC_WINCTL_REG = 6'h16;
    localparam XMC_USEDL_REG  = 6'h0E;
    localparam XMC_USEDH_REG  = 6'h0F;
    localparam XMC_FREEL_REG  = 6'h10;
    localparam XMC_FREEH_REG  = 6'h11;

    initial begin
        for (int i = 0; i < 64; i++) xmc_regs[i] = 0;
        xmc_regs[XMC_BANKS_REG] = 8'd8;
        xmc_regs[XMC_WINCTL_REG] = 8'h0F;
    end

    logic [7:0] music_regs [0:MUSIC_REG_COUNT-1];
    initial begin
        for (int i = 0; i < MUSIC_REG_COUNT; i++)
            music_regs[i] = 8'h00;
    end

    // XMC address decode
    wire xmc_reg_sel = (cpu_addr >= XMC_REG_BASE && cpu_addr <= XMC_REG_END);
    wire xmc_win_sel = (cpu_addr >= XMC_WIN_BASE && cpu_addr <= XMC_WIN_END);
    wire [5:0]  xmc_reg_off = cpu_addr[5:0];
    wire [1:0]  xmc_page    = cpu_addr[9:8];
    wire [7:0]  xmc_offset  = cpu_addr[7:0];

    // Keep the simulation-only memory model aligned with the synthesized
    // SDRAM-backed path: windows are page-aligned and ignore WnAL.
    logic [7:0] xmc_win_mid;
    logic [7:0] xmc_win_high;

    always_comb begin
        unique case (xmc_page)
            2'd0: begin xmc_win_mid = xmc_regs[6'h19]; xmc_win_high = xmc_regs[6'h1A]; end
            2'd1: begin xmc_win_mid = xmc_regs[6'h1C]; xmc_win_high = xmc_regs[6'h1D]; end
            2'd2: begin xmc_win_mid = xmc_regs[6'h1F]; xmc_win_high = xmc_regs[6'h20]; end
            2'd3: begin xmc_win_mid = xmc_regs[6'h22]; xmc_win_high = xmc_regs[6'h23]; end
        endcase
    end

    wire [18:0] xmc_addr_raw = {xmc_win_high[2:0], xmc_win_mid, xmc_offset};
    wire [18:0] xmc_addr = xmc_addr_raw & (XRAM_SIZE - 1);
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];
    wire [15:0] mmio_read_addr = cpu_addr;

    // =========================================================================
    // Memory read mux — registered (Arlet 6502 expects DI one cycle after AB)
    // =========================================================================
    wire cpu_in_rom = (cpu_addr >= ROM_BASE);
    wire music_reg_sel = (cpu_addr >= MUSIC_REG_BASE && cpu_addr <= MUSIC_REG_END);
    wire [$clog2(MUSIC_REG_COUNT)-1:0] music_reg_off = cpu_addr - MUSIC_REG_BASE;
    wire dma_reg_sel = (cpu_addr >= 16'hBA63 && cpu_addr <= 16'hBA75);
    wire blt_reg_sel = (cpu_addr >= 16'hBA83 && cpu_addr <= 16'hBA9B) ||
                       (cpu_addr == 16'hBAA2);
    wire math_reg_sel = (cpu_addr >= MATH_BASE && cpu_addr <= MATH_END);
    wire wts_reg_sel = (cpu_addr >= WTS_BASE && cpu_addr <= WTS_END);
    wire board_input_sel = (cpu_addr >= BOARD_INPUT_BASE && cpu_addr <= BOARD_INPUT_END);
    wire usb_hid_sel = (cpu_addr >= USB_HID_BASE && cpu_addr <= USB_HID_END);
    wire vgc_read_sel = (cpu_addr >= 16'hA000 && cpu_addr <= 16'hA01F) ||
                        (cpu_addr >= 16'hA040 && cpu_addr <= 16'hA0BF) ||
                        (cpu_addr >= 16'hA0E0 && cpu_addr <= 16'hA0ED) ||
                        (cpu_addr >= 16'hA0F0 && cpu_addr <= 16'hA0FF) ||
                        // Direct screen window + plane-select — carve from RAM, route to VGC.
                        (cpu_addr >= 16'hA200 && cpu_addr <= 16'hB1A0);

    always_ff @(posedge clk) begin
        if (xmc_win_sel && xmc_win_enabled)
            cpu_din <= xram[xmc_addr];
        else if (xmc_win_sel && !xmc_win_enabled)
            cpu_din <= 8'hFF;
        else if (xmc_reg_sel)
            cpu_din <= xmc_regs[xmc_reg_off];
        else if (music_reg_sel)
            cpu_din <= music_regs[music_reg_off];
        else if (dma_reg_sel)
            cpu_din <= dma_cpu_rdata;
        else if (blt_reg_sel)
            cpu_din <= blt_cpu_rdata;
        else if (math_reg_sel)
            cpu_din <= math_cpu_rdata;
        else if (wts_reg_sel)
            cpu_din <= wts_cpu_rdata;
        else if (board_input_sel)
            cpu_din <= board_input_read_data(cpu_addr);
        else if (usb_hid_sel)
            cpu_din <= usb_hid_read_data(cpu_addr);
        else if (vgc_read_sel)
            cpu_din <= vgc_cpu_rdata;
        else if (cpu_in_rom)
            cpu_din <= ext_rom_active ? ext_rom[cpu_addr - ROM_BASE]
                                     : basic_rom[cpu_addr - ROM_BASE];
        else
            cpu_din <= ram[cpu_addr];
    end

    // =========================================================================
    // Memory writes
    // =========================================================================
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            for (int i = 0; i < 64; i++) xmc_regs[i] <= 8'h00;
            xmc_regs[XMC_BANKS_REG] <= 8'd8;
            xmc_regs[XMC_WINCTL_REG] <= 8'h0F;
            xmc_regs[XMC_FREEL_REG] <= 8'h00;
            xmc_regs[XMC_FREEH_REG] <= 8'h04;
            for (int i = 0; i < MUSIC_REG_COUNT; i++)
                music_regs[i] <= 8'h00;
            ext_rom_active <= 0;
        end else begin
        if (cpu_we) begin
            if (xmc_win_sel && xmc_win_enabled)
                xram[xmc_addr] <= cpu_dout;
            else if (xmc_reg_sel) begin
                if (xmc_reg_off != XMC_BANKS_REG)
                    xmc_regs[xmc_reg_off] <= cpu_dout;
            end else if (cpu_addr < ROM_BASE && !vgc_read_sel && !math_reg_sel &&
                         !wts_reg_sel && !board_input_sel && !usb_hid_sel &&
                         !music_reg_sel)
                ram[cpu_addr] <= cpu_dout;
        end

        // Bus-master (blitter/DMA) RAM/XRAM writes
        if (bm_ram_we && bm_ram_addr < ROM_BASE)
            ram[bm_ram_addr] <= bm_ram_wdata;
        if (bm_xram_we)
            xram[bm_xram_addr] <= bm_xram_wdata;

        // Debug ROM-load writes (mirrors the SYNTHESIS dpram port A path)
        if (dbg_rom_we) begin
            if (dbg_rom_idx == 1'b0)
                basic_rom[dbg_rom_addr] <= dbg_rom_data;
            else
                ext_rom[dbg_rom_addr] <= dbg_rom_data;
        end

        // ROM bank switching
        if (cpu_we && cpu_addr == REG_ROMSWAP) begin
            if (cpu_dout == ROMSWAP_EXT)
                ext_rom_active <= 1;
            else if (cpu_dout == ROMSWAP_BASIC || cpu_dout == ROMSWAP_NCC ||
                     cpu_dout == ROMSWAP_NCCEDIT || cpu_dout == ROMSWAP_LOGO)
                ext_rom_active <= 0;
        end

        end
    end

    // =========================================================================
    // Keyboard
    // =========================================================================
    always_ff @(posedge clk) begin
        if (key_valid)
            ram[CHARIN] <= key_data;
    end

    logic clear_input;
    always_ff @(posedge clk) begin
        clear_input <= (!cpu_we && cpu_addr == CHARIN && ram[CHARIN] != 0);
        if (clear_input && !key_valid)
            ram[CHARIN] <= 0;
    end

    // Combinational RAM/XRAM reads for blitter and DMA (sim only).
    // The bus-master is whichever of blitter/DMA is currently active; its
    // addr drives these reads. The idle master's addr is 0 and sees ram[0],
    // which the idle-state machine ignores.
    wire [7:0] blt_ram_rdata  = ram[bm_ram_addr];
    wire [7:0] blt_xram_rdata = xram[bm_xram_addr];
    wire [7:0] dma_ram_rdata  = ram[bm_ram_addr];
    wire [7:0] dma_xram_rdata = xram[bm_xram_addr];
    // Sim path: xram_sdram_inst isn't instantiated here (`ifdef SYNTHESIS`
    // branch owns it), so port A must be zeroed. Port B is always driven
    // by sid_curve_reader which sits outside this ifdef.
    assign sdram_addrA = 25'd0;
    assign sdram_dinA  = 8'd0;
    assign sdram_weA   = 1'b0;
    assign sdram_oeA   = 1'b0;
    assign fio_event   = 1'b0;
    assign nic_event   = 1'b0;
    wire nic_irq       = 1'b0;
    wire nic_rdy       = 1'b1;
    wire nic_dma_grant;
    wire [15:0] nic_ram_addr = 16'h0000;
    wire [7:0]  nic_ram_wdata = 8'h00;
    wire        nic_ram_we = 1'b0;
    wire        nic_ram_re = 1'b0;
    wire [7:0]  nic_ram_rdata = 8'h00;
    assign dbg_nic_buf_rdata = 8'h00;

`endif

    // cpu_active: high on cycles when the CPU is actually executing.
`ifdef SYNTHESIS
    wire cpu_active = cpu_ce;
`else
    wire cpu_active = 1'b1;
`endif

    // =========================================================================
    // Blitter + DMA — share the same memory ports; mutually exclusive
    // via CPU RDY stall, muxed at top-level into bm_* signals.
    // =========================================================================
    wire [15:0] blt_ram_addr;
    wire [7:0]  blt_ram_wdata;
    wire        blt_ram_we;
    wire [18:0] blt_xram_addr;
    wire [7:0]  blt_xram_wdata;
    wire        blt_xram_we;
    wire        blt_xram_re;
    wire [2:0]  blt_vgc_space;
    wire [16:0] blt_vgc_addr;     // shared VGC-space address bus
    wire [7:0]  blt_vgc_wdata;
    wire        blt_vgc_we;
    wire        blt_vgc_re;
    wire [7:0]  blt_cpu_rdata;
    wire        blt_rdy;
    wire        vgc_video_blit_safe;

    wire [15:0] dma_ram_addr;
    wire [7:0]  dma_ram_wdata;
    wire        dma_ram_we;
    wire [18:0] dma_xram_addr;
    wire [7:0]  dma_xram_wdata;
    wire        dma_xram_we;
    wire        dma_xram_re;
    wire [2:0]  dma_vgc_space;
    wire [16:0] dma_vgc_addr;
    wire [7:0]  dma_vgc_wdata;
    wire        dma_vgc_we;
    wire        dma_vgc_re;
    wire [7:0]  dma_cpu_rdata;
    wire        dma_rdy;

    wire [7:0]  math_cpu_rdata;
    wire        math_rdy;

    // Bus-master mux — DMA takes priority when busy, blitter otherwise.
    // Both are idle (rdy=1) → signals default to 0 in each module.
    wire        dma_busy = !dma_rdy;
    wire        blt_busy = !blt_rdy;
    wire        nic_busy = !nic_rdy;
    assign nic_dma_grant = nic_busy && !dma_busy && !blt_busy;
    wire [15:0] bm_ram_addr   = dma_busy ? dma_ram_addr :
                                blt_busy ? blt_ram_addr : nic_ram_addr;
    wire [7:0]  bm_ram_wdata  = dma_busy ? dma_ram_wdata :
                                blt_busy ? blt_ram_wdata : nic_ram_wdata;
    wire        bm_ram_we     = dma_busy ? dma_ram_we :
                                blt_busy ? blt_ram_we :
                                nic_dma_grant ? nic_ram_we : 1'b0;
    wire [18:0] bm_xram_addr  = dma_busy ? dma_xram_addr  :
                                blt_busy ? blt_xram_addr  : 19'd0;
    wire [7:0]  bm_xram_wdata = dma_busy ? dma_xram_wdata :
                                blt_busy ? blt_xram_wdata : 8'd0;
    wire        bm_xram_we    = dma_busy ? dma_xram_we    :
                                blt_busy ? blt_xram_we    : 1'b0;
    wire        bm_xram_re    = dma_busy ? dma_xram_re    :
                                blt_busy ? blt_xram_re    : 1'b0;
    wire [2:0]  bm_vgc_space  = dma_busy ? dma_vgc_space  :
                                blt_busy ? blt_vgc_space  : 3'd0;
    wire [16:0] bm_vgc_addr   = dma_busy ? dma_vgc_addr   :
                                blt_busy ? blt_vgc_addr   : 17'd0;
    wire [7:0]  bm_vgc_wdata  = dma_busy ? dma_vgc_wdata  :
                                blt_busy ? blt_vgc_wdata  : 8'd0;
    wire        bm_vgc_we     = dma_busy ? dma_vgc_we     :
                                blt_busy ? blt_vgc_we     : 1'b0;
    wire        bm_vgc_re     = dma_busy ? dma_vgc_re     :
                                blt_busy ? blt_vgc_re     : 1'b0;
    // VGC rdata + memory reads feed both masters (the idle one ignores them)
    wire [7:0]  bm_vgc_rdata;
    wire [7:0]  blt_vgc_rdata = bm_vgc_rdata;
    wire [7:0]  dma_vgc_rdata = bm_vgc_rdata;

    wire [15:0] blt_bus_addr  = dbg_poke_blt ? dbg_poke_addr : cpu_addr;
    wire [15:0] blt_bus_raddr = dbg_peek_blt ? dbg_peek_addr : mmio_read_addr;
    wire [7:0]  blt_bus_wdata = dbg_poke_blt ? dbg_poke_data : cpu_dout;
    wire        blt_bus_we    = dbg_poke_blt ? 1'b1 : (cpu_we & cpu_active);
    wire        blt_bus_re    = dbg_peek_blt ? 1'b1 : 1'b0;

    blitter blt_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .cpu_addr   (blt_bus_addr),
        .cpu_raddr  (blt_bus_raddr),
        .cpu_wdata  (blt_bus_wdata),
        .cpu_we     (blt_bus_we),
        .cpu_rdata  (blt_cpu_rdata),
        .cpu_re     (blt_bus_re),
        .rdy_out    (blt_rdy),
        .ram_addr   (blt_ram_addr),
        .ram_rdata  (blt_ram_rdata),
        .ram_wdata  (blt_ram_wdata),
        .ram_we     (blt_ram_we),
        .xram_addr  (blt_xram_addr),
        .xram_rdata (blt_xram_rdata),
        .xram_wdata (blt_xram_wdata),
        .xram_we    (blt_xram_we),
        .xram_re    (blt_xram_re),
        .xram_busy  (xram_busy),
        .vgc_space  (blt_vgc_space),
        .vgc_addr   (blt_vgc_addr),
        .vgc_rdata  (blt_vgc_rdata),
        .vgc_wdata  (blt_vgc_wdata),
        .vgc_we     (blt_vgc_we),
        .vgc_re     (blt_vgc_re),
        .video_write_safe(vgc_video_blit_safe)
    );

    dma dma_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_dout),
        .cpu_we     (cpu_we & cpu_active),
        .cpu_rdata  (dma_cpu_rdata),
        .cpu_re     (1'b0),
        .rdy_out    (dma_rdy),
        .ram_addr   (dma_ram_addr),
        .ram_rdata  (dma_ram_rdata),
        .ram_wdata  (dma_ram_wdata),
        .ram_we     (dma_ram_we),
        .xram_addr  (dma_xram_addr),
        .xram_rdata (dma_xram_rdata),
        .xram_wdata (dma_xram_wdata),
        .xram_we    (dma_xram_we),
        .xram_re    (dma_xram_re),
        .xram_busy  (xram_busy),
        .vgc_space  (dma_vgc_space),
        .vgc_addr   (dma_vgc_addr),
        .vgc_rdata  (dma_vgc_rdata),
        .vgc_wdata  (dma_vgc_wdata),
        .vgc_we     (dma_vgc_we),
        .vgc_re     (dma_vgc_re),
        .video_write_safe(vgc_video_blit_safe)
    );

    wire [15:0] math_bus_addr  = dbg_poke_math ? dbg_poke_addr : cpu_addr;
    wire [15:0] math_bus_raddr = dbg_peek_math ? dbg_peek_addr : mmio_read_addr;
    wire [7:0]  math_bus_wdata = dbg_poke_math ? dbg_poke_data : cpu_dout;
    wire        math_bus_we    = dbg_poke_math ? 1'b1 : (cpu_we & cpu_active);
    wire        math_bus_re    = (!dbg_peek_math && !dbg_poke_math && !cpu_we && cpu_active);

    math_copro math_inst (
        .clk       (clk),
        .rst       (custom_rst),
        .cpu_addr  (math_bus_addr),
        .cpu_raddr (math_bus_raddr),
        .cpu_wdata (math_bus_wdata),
        .cpu_we    (math_bus_we),
        .cpu_re    (math_bus_re),
        .cpu_rdata (math_cpu_rdata),
        .rdy_out   (math_rdy)
    );

    // =========================================================================
    // WTS — 8-voice wavetable/sample synthesizer. NovaHost writes parsed
    // soundbank samples into SDRAM and drives this register bank with MIDI events.
    // =========================================================================
    wire wts_cpu_ce =
`ifdef SYNTHESIS
        cpu_ce;
`else
        1'b1;
`endif
    wire wts_cpu_we = cpu_we & cpu_active;

    wts_chip wts_inst (
        .clk         (clk),
        .rst         (custom_rst | dbg_cpu_reset),
        .cpu_ce      (wts_cpu_ce),
        .cpu_addr    (cpu_addr),
        .cpu_wdata   (cpu_dout),
        .cpu_we      (wts_cpu_we),
        .cpu_rdata   (wts_cpu_rdata),
        .dbg_we      (dbg_poke_wts),
        .dbg_addr    (dbg_poke_addr[7:0] - WTS_BASE[7:0]),
        .dbg_wdata   (dbg_poke_data),
        .dbg_raddr   (dbg_peek_addr[7:0] - WTS_BASE[7:0]),
        .dbg_rdata   (wts_dbg_rdata),
        .host_event_we    (host_wts_event_we),
        .host_event_data  (host_wts_event_data),
        .host_event_ready (host_wts_event_ready),
        .sdram_addrB (wts_addrB),
        .sdram_dinB  (wts_dinB),
        .sdram_weB   (wts_weB),
        .sdram_oeB   (wts_oeB),
        .sdram_doutB (sdram_doutB),
        .audio_l     (wts_audio_l),
        .audio_r     (wts_audio_r)
    );

    // =========================================================================
    // SID — dual legacy 6581/8580-compatible sound chips
    // =========================================================================

    // Fractional clock enable from the 27 MHz pixel clock. SID tune frequency
    // registers assume a real C64 clock, not clk_pixel / 25.
    localparam [25:0] SID_PIXEL_HZ = 26'd27000000;
    localparam [25:0] SID_PAL_HZ   = 26'd985248;
    localparam [25:0] SID_NTSC_HZ  = 26'd1022727;

    logic       sid_clock_ntsc;
    logic [25:0] sid_clk_acc;
    logic       sid_ce_1m;
    wire [25:0] sid_clk_step = sid_clock_ntsc ? SID_NTSC_HZ : SID_PAL_HZ;
    wire [26:0] sid_clk_next = {1'b0, sid_clk_acc} + {1'b0, sid_clk_step};

    always_ff @(posedge clk) begin
        if (custom_rst) begin
            sid_clk_acc <= 26'd0;
            sid_ce_1m   <= 0;
        end else if (sid_clk_next >= {1'b0, SID_PIXEL_HZ}) begin
            sid_clk_acc <= sid_clk_next - {1'b0, SID_PIXEL_HZ};
            sid_ce_1m   <= 1'b1;
        end else begin
            sid_clk_acc <= sid_clk_next[25:0];
            sid_ce_1m   <= 1'b0;
        end
    end

    // SID config register ($D440):
    //   bit 0 = SID model (0 = 6581, 1 = 8580)
    //   bit 1 = SID clock (0 = PAL 985248 Hz, 1 = NTSC 1022727 Hz)
    //   bit 7 = HDMI audio test tone, used to prove the board emits audio
    wire dbg_poke_sid1 = dbg_poke_en &&
                         (dbg_poke_addr >= SID1_BASE && dbg_poke_addr <= SID1_END);
    wire dbg_poke_sid2 = dbg_poke_en &&
                         ((dbg_poke_addr >= SID2_BASE && dbg_poke_addr <= SID2_END) ||
                          (dbg_poke_addr >= SID2_MIRROR_BASE && dbg_poke_addr <= SID2_MIRROR_END));
    wire dbg_poke_sid_cfg = dbg_poke_en && (dbg_poke_addr == SID_CFG);
    wire dbg_poke_sid = dbg_poke_sid1 | dbg_poke_sid2 | dbg_poke_sid_cfg;
    wire sid_cpu_we = cpu_we && cpu_active && !dbg_poke_sid;
    wire [15:0] sid_bus_addr = dbg_poke_sid ? dbg_poke_addr : cpu_addr;
    wire [7:0]  sid_bus_dout = dbg_poke_sid ? dbg_poke_data : cpu_dout;

    logic sid_mode_8580;
    logic hdmi_audio_test_enable;
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            sid_mode_8580 <= 1'b0;
            sid_clock_ntsc <= 1'b0;
            hdmi_audio_test_enable <= 1'b0;
        end else if ((sid_cpu_we && cpu_addr == SID_CFG) || dbg_poke_sid_cfg) begin
            sid_mode_8580 <= sid_bus_dout[0];
            sid_clock_ntsc <= sid_bus_dout[1];
            hdmi_audio_test_enable <= sid_bus_dout[7];
        end
    end

    // Chip select and address decode
    wire sid1_cs = dbg_poke_sid1 ||
                   (sid_cpu_we && (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END));
    wire sid2_cs = dbg_poke_sid2 ||
                   (sid_cpu_we &&
                    ((cpu_addr >= SID2_BASE && cpu_addr <= SID2_END) ||
                     (cpu_addr >= SID2_MIRROR_BASE && cpu_addr <= SID2_MIRROR_END)));
    wire [7:0] sid1_dout;
    wire [7:0] sid2_dout;
    logic signed [17:0] sid1_audio_mono;
    logic signed [17:0] sid2_audio_mono;

    // SID filter curve pulled from SDRAM (Phase 2.5 Step 3).
    wire [10:0] sid1_filter_fc;
    wire [10:0] sid2_filter_fc;
    wire [15:0] sid1_filter_f0;
    wire [15:0] sid2_filter_f0;

    sid_chip sid_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_mode_8580),
        .cs         (sid1_cs),
        .we         (1'b1),
        .addr       (sid_bus_addr[4:0]),
        .din        (sid_bus_dout),
        .dout       (sid1_dout),
        .audio_out  (sid1_audio_mono),
        .filter_fc_out(sid1_filter_fc),
        .filter_f0_in (sid1_filter_f0)
    );

    sid_chip sid2_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_mode_8580),
        .cs         (sid2_cs),
        .we         (1'b1),
        .addr       (sid_bus_addr[4:0]),
        .din        (sid_bus_dout),
        .dout       (sid2_dout),
        .audio_out  (sid2_audio_mono),
        .filter_fc_out(sid2_filter_fc),
        .filter_f0_in (sid2_filter_f0)
    );

    function automatic logic signed [17:0] clip_sid_mix(input logic signed [18:0] sample);
        begin
            if (sample > 19'sd131071)
                clip_sid_mix = 18'sd131071;
            else if (sample < -19'sd131072)
                clip_sid_mix = -18'sd131072;
            else
                clip_sid_mix = sample[17:0];
        end
    endfunction

    wire signed [18:0] sid_audio_sum = {sid1_audio_mono[17], sid1_audio_mono} +
                                       {sid2_audio_mono[17], sid2_audio_mono};
    wire signed [17:0] sid_audio_mix = clip_sid_mix(sid_audio_sum);
    wire signed [18:0] music_audio_sum_l = {sid_audio_mix[17], sid_audio_mix} +
                                           {wts_audio_l[17], wts_audio_l};
    wire signed [18:0] music_audio_sum_r = {sid_audio_mix[17], sid_audio_mix} +
                                           {wts_audio_r[17], wts_audio_r};
    wire signed [17:0] music_audio_l = clip_sid_mix(music_audio_sum_l);
    wire signed [17:0] music_audio_r = clip_sid_mix(music_audio_sum_r);

    // A simple square-wave diagnostic lets us prove HDMI audio output without
    // depending on SID programming, envelope state, or game/application code.
    localparam [15:0] HDMI_TEST_TONE_HALF_PERIOD = 16'd30682; // ~440 Hz at 27 MHz
    logic [15:0] hdmi_test_tone_div;
    logic        hdmi_test_tone_high;
    always_ff @(posedge clk) begin
        if (custom_rst || !hdmi_audio_test_enable) begin
            hdmi_test_tone_div <= 16'd0;
            hdmi_test_tone_high <= 1'b0;
        end else if (hdmi_test_tone_div == HDMI_TEST_TONE_HALF_PERIOD) begin
            hdmi_test_tone_div <= 16'd0;
            hdmi_test_tone_high <= ~hdmi_test_tone_high;
        end else begin
            hdmi_test_tone_div <= hdmi_test_tone_div + 1'd1;
        end
    end

    wire signed [17:0] hdmi_test_audio = hdmi_test_tone_high ? 18'sd65536 : -18'sd65536;
    assign audio_l = hdmi_audio_test_enable ? hdmi_test_audio : music_audio_l;
    assign audio_r = hdmi_audio_test_enable ? hdmi_test_audio : music_audio_r;

    // SDRAM port B is shared by the SID filter-curve reader and WTS sample
    // fetches. WTS gets priority because active sample playback must not miss
    // the 48 kHz audio cadence; the SID curve reader simply retries on later
    // pixel clocks as cutoff values change.
    wire [24:0] curve_addrB;
    wire        curve_weB;
    wire [7:0]  curve_dinB;
    wire        curve_oeB;

    sid_curve_reader curve_reader_inst (
        .clk         (clk),
        .rst         (custom_rst | dbg_cpu_reset),
        .sid1_Fc     (sid1_filter_fc),
        .sid1_f0     (sid1_filter_f0),
        .sid2_Fc     (sid2_filter_fc),
        .sid2_f0     (sid2_filter_f0),
        .sdram_addrB (curve_addrB),
        .sdram_weB   (curve_weB),
        .sdram_dinB  (curve_dinB),
        .sdram_oeB   (curve_oeB),
        .sdram_doutB (sdram_doutB)
    );

    wire wts_sdram_active = wts_oeB | wts_weB;
    assign sdram_addrB = wts_sdram_active ? wts_addrB : curve_addrB;
    assign sdram_weB   = wts_sdram_active ? wts_weB   : curve_weB;
    assign sdram_dinB  = wts_sdram_active ? wts_dinB  : curve_dinB;
    assign sdram_oeB   = wts_sdram_active ? wts_oeB   : curve_oeB;

    // =========================================================================
    // CPU
    // =========================================================================
    wire vgc_rdy;
    wire vgc_irq;
    wire cpu_irq = ~irq_n | vgc_irq | nic_irq | board_input_irq;
    wire cpu_nmi = ~nmi_n;
    wire cpu_rdy = blt_rdy & dma_rdy & math_rdy & nic_rdy & vgc_rdy & ~dbg_pause & cpu_active;

    cpu cpu_inst (
        .clk    (clk),
        .reset  (cpu_reset),
        .AB     (cpu_addr),
        .DI     (cpu_din),
        .DO     (cpu_dout),
        .WE     (cpu_we),
        .IRQ    (cpu_irq),
        .NMI    (cpu_nmi),
        .RDY    (cpu_rdy),
        .dbg_resume (dbg_cpu_resume),
        .dbg_waiting(cpu_dbg_waiting),
        .dbg_stopped(cpu_dbg_stopped),
        .dbg_pc    (cpu_dbg_pc),
        .dbg_a     (cpu_dbg_a),
        .dbg_x     (cpu_dbg_x),
        .dbg_y     (cpu_dbg_y),
        .dbg_s     (cpu_dbg_s),
        .dbg_flags (cpu_dbg_flags),
        .dbg_state (cpu_dbg_state),
        .dbg_ir    (cpu_dbg_ir)
    );

    // =========================================================================
    // VGC — snoops bus, generates video
    // =========================================================================
    wire [7:0]  vgc_cpu_rdata;
    wire [7:0]  vgc_dbg_rdata;
    wire        vgc_dbg_owns;

    vgc vgc_inst (
        .clk            (clk),
        .rst            (custom_rst),
        .video_rst      (rst),
        .cpu_ce         (cpu_active),
        .cpu_addr       (cpu_addr),
        .cpu_raddr      (mmio_read_addr),
        .cpu_wdata      (cpu_dout),
        .cpu_rdata      (vgc_cpu_rdata),
        .cpu_we         (cpu_we),
        .cpu_re         (~cpu_we),
        .key_valid      (key_valid),
        .key_data       (key_data),
        .key_ready      (key_ready),
        .blt_space      (bm_vgc_space),
        .blt_addr       (bm_vgc_addr),
        .blt_rdata      (bm_vgc_rdata),
        .blt_wdata      (bm_vgc_wdata),
        .blt_we         (bm_vgc_we),
        .blt_re         (bm_vgc_re),
        .video_blit_safe(vgc_video_blit_safe),
        .dbg_addr       (dbg_peek_addr),
        .dbg_rdata      (vgc_dbg_rdata),
        .dbg_we         (dbg_poke_vgc),
        .dbg_waddr      (dbg_poke_addr),
        .dbg_wdata      (dbg_poke_data),
        .dbg_vmem_we    (dbg_vmem_we),
        .dbg_vmem_re    (dbg_vmem_re),
        .dbg_vmem_space (dbg_vmem_space),
        .dbg_vmem_addr  (dbg_vmem_addr),
        .dbg_vmem_wdata (dbg_vmem_data),
        .dbg_vmem_rdata (dbg_vmem_rdata),
        .vid_r          (vid_r),
        .vid_g          (vid_g),
        .vid_b          (vid_b),
        .vid_hsync      (vid_hsync),
        .vid_vsync      (vid_vsync),
        .vid_de         (vid_de),
        .irq_out        (vgc_irq),
        .rdy_out        (vgc_rdy),
        .sys_reset_req  (vgc_sys_reset_req)
`ifndef SYNTHESIS
        ,
        .dbg_vram_read_en(dbg_vram_read_en),
        .dbg_vram_space  (dbg_vram_space),
        .dbg_vram_addr   (dbg_vram_addr),
        .dbg_vram_rdata  (dbg_vram_rdata)
`endif
    );

    // =========================================================================
    // Debug/simulation interface
    // =========================================================================

`ifdef SYNTHESIS
    // On FPGA: debug peek mirrors the CPU-visible memory map.
    // Compute VGC ownership here in top.sv — cross-module ref failed (ea510bd
    // class), output port also failed to drive through synth. Duplicating the
    // small range-decode here is reliable.
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_vgc_regs  = (dbg_peek_addr >= 16'hA000 && dbg_peek_addr <= 16'hA01F) ||
                         (dbg_peek_addr >= 16'hA0F0 && dbg_peek_addr <= 16'hA0FF);
    wire dbg_vgc_spr   = (dbg_peek_addr >= 16'hA040 && dbg_peek_addr <= 16'hA0BF);
    wire dbg_vgc_vram  = (dbg_peek_addr >= 16'hA0E0 && dbg_peek_addr <= 16'hA0E4);
    wire dbg_vgc_dim   = (dbg_peek_addr == 16'hA0E5);
    wire dbg_vgc_text  = (dbg_peek_addr >= 16'hA0E6 && dbg_peek_addr <= 16'hA0EC);
    wire dbg_fio       = (dbg_peek_addr >= 16'hB9A0 && dbg_peek_addr <= 16'hB9EF);
    assign vgc_dbg_owns = dbg_vgc_regs | dbg_vgc_spr | dbg_vgc_vram | dbg_vgc_dim | dbg_vgc_text;

    // Math register reads are combinational on the selected address. Keep that
    // decode out of the final debug peek mux; the bridge already waits long
    // enough for this one-cycle staging register before sampling dbg_peek_data.
    logic [7:0] dbg_math_rdata;
    always_ff @(posedge clk) begin
        if (dbg_peek_math)
            dbg_math_rdata <= math_cpu_rdata;
    end

    // For debug reads, RAM port B serves as the read port.
    // ROM reads use the same brom_b/erom_b ports (address set combinationally).
    // SID overlaps ROM and is write-only in the CPU-visible map, so debug reads
    // fall through to ROM/RAM instead of sampling live SID register shadows.
    // Keep the debug peek mux shallow. A nested priority chain makes the RAM
    // fallback walk through every MMIO branch before it reaches dbg_peek_data,
    // which turns debug readback into a clk_pixel critical path.
    wire       dbg_mmio_owns = vgc_dbg_owns | dbg_peek_nic | dbg_peek_wts |
                               dbg_fio | dbg_peek_music | dbg_peek_blt |
                               dbg_peek_board_input | dbg_peek_usb_hid |
                               dbg_peek_math;
    wire [7:0] dbg_mmio_data = vgc_dbg_owns  ? vgc_dbg_rdata  :
                               dbg_peek_nic  ? nic_dbg_rdata  :
                               dbg_peek_wts  ? wts_dbg_rdata  :
                               dbg_fio       ? fio_dbg_rdata  :
                               dbg_peek_music ? music_regs[dbg_music_reg_off] :
                               dbg_peek_blt  ? blt_cpu_rdata  :
                               dbg_peek_board_input ? dbg_board_input_data :
                               dbg_peek_usb_hid ? dbg_usb_hid_data :
                               dbg_peek_math ? dbg_math_rdata :
                                               8'h00;
    wire [7:0] dbg_mem_data  = dbg_in_rom ? (ext_rom_active ? erom_b_dout : brom_b_dout)
                                          : ram_b_dout;
    wire [7:0] dbg_peek_data_mux = dbg_peek_en ? (dbg_mmio_owns ? dbg_mmio_data : dbg_mem_data)
                                                : 8'h00;

    // Keep debug peeks off the main timing path. The bridge already waits for
    // BRAM read latency before sampling, so this register is protocol-neutral.
    always_ff @(posedge clk) begin
        dbg_peek_data <= dbg_peek_data_mux;
    end
`else
    // Simulation: full debug with combinational reads — ownership decoded
    // directly in top.sv (symmetric with synthesis path)
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_vgc_regs  = (dbg_peek_addr >= 16'hA000 && dbg_peek_addr <= 16'hA01F) ||
                         (dbg_peek_addr >= 16'hA0F0 && dbg_peek_addr <= 16'hA0FF);
    wire dbg_vgc_spr   = (dbg_peek_addr >= 16'hA040 && dbg_peek_addr <= 16'hA0BF);
    wire dbg_vgc_vram  = (dbg_peek_addr >= 16'hA0E0 && dbg_peek_addr <= 16'hA0E4);
    wire dbg_vgc_dim   = (dbg_peek_addr == 16'hA0E5);
    wire dbg_vgc_text  = (dbg_peek_addr >= 16'hA0E6 && dbg_peek_addr <= 16'hA0EC);
    assign vgc_dbg_owns = dbg_vgc_regs | dbg_vgc_spr | dbg_vgc_vram | dbg_vgc_dim | dbg_vgc_text;
    wire [7:0] dbg_rom_read_data = ext_rom_active ? ext_rom[dbg_peek_addr - ROM_BASE]
                                                  : basic_rom[dbg_peek_addr - ROM_BASE];
    wire [7:0] dbg_peek_data_mux = dbg_peek_en ? (vgc_dbg_owns     ? vgc_dbg_rdata :
                                                  dbg_peek_blt     ? blt_cpu_rdata :
                                                  dbg_peek_math    ? math_cpu_rdata :
                                                  dbg_peek_wts     ? wts_dbg_rdata :
                                                  dbg_peek_music   ? music_regs[dbg_music_reg_off] :
                                                  dbg_peek_board_input ? dbg_board_input_data :
                                                  dbg_peek_usb_hid ? dbg_usb_hid_data :
                                                  dbg_in_rom       ? dbg_rom_read_data :
                                                                     ram[dbg_peek_addr]) : 8'h00;

    always_ff @(posedge clk) begin
        dbg_peek_data <= dbg_peek_data_mux;
    end

    // Poke: write to main RAM on clock edge (ROM-protected)
    always_ff @(posedge clk) begin
        if (dbg_poke_music)
            music_regs[dbg_music_reg_off] <= dbg_poke_data;

        if (dbg_poke_en && !dbg_poke_vgc && !dbg_poke_blt && !dbg_poke_math && !dbg_poke_wts && !dbg_poke_music && !dbg_poke_board_input && !dbg_poke_usb_hid && dbg_poke_addr < ROM_BASE)
            ram[dbg_poke_addr] <= dbg_poke_data;
    end
`endif

    // CPU state — use proper debug output ports on the cpu module instead of
    // cross-module references which don't reliably survive synthesis.
    assign dbg_cpu_pc    = cpu_dbg_pc;
    assign dbg_cpu_a     = cpu_dbg_a;
    assign dbg_cpu_x     = cpu_dbg_x;
    assign dbg_cpu_y     = cpu_dbg_y;
    assign dbg_cpu_sp    = cpu_dbg_s;
    assign dbg_cpu_flags = cpu_dbg_flags;
    assign dbg_cpu_state = cpu_dbg_state;
    assign dbg_cpu_ir    = cpu_dbg_ir;
    assign dbg_cpu_addr  = cpu_addr;
    assign dbg_cpu_din   = cpu_din;
    assign dbg_cpu_dout  = cpu_dout;
    assign dbg_cpu_we    = cpu_we;
    assign dbg_cpu_rdy   = cpu_rdy;
    assign dbg_cpu_irq   = cpu_irq;
    assign dbg_cpu_nmi   = cpu_nmi;
    assign dbg_cpu_waiting = cpu_dbg_waiting;
    assign dbg_cpu_stopped = cpu_dbg_stopped;

endmodule
