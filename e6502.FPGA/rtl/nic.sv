// Network interface controller register bank + payload DMA.
//
// 64-byte register file at $A100-$A13F. The CPU writes NIC_CMD to request
// socket operations from NovaHost. NovaHost services commands over the debug
// bridge and writes result/status bytes back through dbg_we.
//
// Payload bytes are not read/written from CPU RAM by NovaHost. SEND snapshots
// CPU RAM into the NIC TX buffer before emitting nic_event. RECV is a two-step
// host flow: NovaHost writes the RX buffer through the debug bridge, writes
// NIC_MSGLEN, then writes HOSTCTRL.RX_START to DMA the buffer into CPU RAM.
//
// Register map (mirrors nova.inc / VgcConstants.cs):
//   +00  NIC_CMD       (write non-zero -> nic_event pulse)
//   +01  NIC_STATUS    (global status: ready/data/error)
//   +02  NIC_SLOT
//   +03  NIC_IRQCTRL
//   +04  NIC_IRQSTATUS (CPU read clears)
//   +05  NIC_CMDSEQ    (CPU command nonce; host reads only)
//   +06  NIC_CMDSHADOW (CPU command copy; host reads only)
//   +07  NIC_HOSTCTRL  (debug write: bit0=start RX DMA; self-clears)
//   +08/+09  NIC_RPORTL/H
//   +0A/+0B  NIC_LPORTL/H
//   +10/+11  NIC_DMAL/H
//   +12      NIC_DMALEN
//   +13      NIC_MSGLEN
//   +14      NIC_DMASTATUS
//   +15      NIC_DMAERR
//   +18..1B  NIC_SLOTST0..3
//   +20..3F  NIC_NAMEBUF

module nic (
    input  logic        clk,
    input  logic        rst,

    input  logic        cpu_ce,
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,

    input  logic        dbg_we,
    input  logic [5:0]  dbg_addr,
    input  logic [7:0]  dbg_wdata,
    input  logic [5:0]  dbg_raddr,
    output logic [7:0]  dbg_rdata,

    // Private host-visible packet buffers. Debug bridge reads TX bytes after
    // SEND DMA completes and writes RX bytes before HOSTCTRL.RX_START.
    input  logic        dbg_buf_we,
    input  logic        dbg_buf_re,
    input  logic        dbg_buf_sel,    // 0=TX buffer, 1=RX buffer
    input  logic [7:0]  dbg_buf_addr,
    input  logic [7:0]  dbg_buf_wdata,
    output logic [7:0]  dbg_buf_rdata,

    // RAM bus-master port. top.sv grants this port only when blitter/DMA are
    // idle, then stalls the CPU with rdy_out while the transfer runs.
    input  logic        dma_grant,
    output logic        rdy_out,
    output logic [15:0] ram_addr,
    input  logic [7:0]  ram_rdata,
    output logic [7:0]  ram_wdata,
    output logic        ram_we,
    output logic        ram_re,

    output logic        nic_event,
    output logic        irq
);

    localparam logic [15:0] BASE = 16'hA100;
    localparam logic [15:0] LAST = 16'hA13F;
    localparam int          SIZE = 64;

    localparam int OFF_CMD       = 6'h00;
    localparam int OFF_STATUS    = 6'h01;
    localparam int OFF_IRQCTRL   = 6'h03;
    localparam int OFF_IRQSTATUS = 6'h04;
    localparam int OFF_HOSTCTRL  = 6'h07;
    localparam int OFF_DMAL      = 6'h10;
    localparam int OFF_DMAH      = 6'h11;
    localparam int OFF_DMALEN    = 6'h12;
    localparam int OFF_MSGLEN    = 6'h13;
    localparam int OFF_DMASTATUS = 6'h14;
    localparam int OFF_DMAERR    = 6'h15;
    localparam int OFF_SLOTST0   = 6'h18;

    localparam logic [7:0] STATUS_READY = 8'h01;
    localparam logic [7:0] SLOT_SENDREADY = 8'h04;
    localparam logic [7:0] IRQ_EVENT_MASK = 8'h1A; // data-ready, error, remote-closed
    localparam logic [7:0] CMD_SEND = 8'h03;

    localparam logic [7:0] HOSTCTRL_RX_START = 8'h01;
    localparam logic [7:0] DMAST_TX_READY = 8'h01;
    localparam logic [7:0] DMAST_RX_DONE  = 8'h02;
    localparam logic [7:0] DMAST_ERROR    = 8'h40;
    localparam logic [7:0] DMAST_BUSY     = 8'h80;
    localparam logic [7:0] DMAERR_NONE    = 8'h00;
    localparam logic [7:0] DMAERR_RANGE   = 8'h01;
    localparam logic [7:0] DMAERR_BUSY    = 8'h02;

    localparam logic [16:0] RAM_DMA_LIMIT = 17'h0C000; // CPU RAM below ROM

    reg [7:0] bank [0:SIZE-1];
    reg [7:0] tx_buf [0:255];
    reg [7:0] rx_buf [0:255];

    wire sel = (cpu_addr >= BASE) && (cpu_addr <= LAST);
    wire [5:0] cpu_off = cpu_addr[5:0];
    wire cpu_cmd_write = cpu_ce && cpu_we && sel && (cpu_off == OFF_CMD);
    wire cpu_irqstatus_read = cpu_ce && !cpu_we && sel && (cpu_off == OFF_IRQSTATUS);
    wire dbg_hostctrl_write = dbg_we && (dbg_addr == OFF_HOSTCTRL[5:0]);

    wire [3:0] irq_level_bits = {
        |(bank[6'h1B] & IRQ_EVENT_MASK),
        |(bank[6'h1A] & IRQ_EVENT_MASK),
        |(bank[6'h19] & IRQ_EVENT_MASK),
        |(bank[6'h18] & IRQ_EVENT_MASK)
    };
    wire [7:0] irqstatus_read_value = {4'b0000, (bank[OFF_IRQSTATUS][3:0] | irq_level_bits)};

    assign cpu_rdata = sel
        ? ((cpu_off == OFF_IRQSTATUS) ? irqstatus_read_value : bank[cpu_off])
        : 8'h00;
    assign dbg_rdata = bank[dbg_raddr];
    assign irq = |((bank[OFF_IRQSTATUS][3:0] | irq_level_bits) & bank[OFF_IRQCTRL][3:0]);

    always_comb begin
        dbg_buf_rdata = dbg_buf_sel ? rx_buf[dbg_buf_addr] : tx_buf[dbg_buf_addr];
    end

    typedef enum logic [2:0] {
        DMA_IDLE,
        DMA_TX_READ,
        DMA_TX_WAIT,
        DMA_TX_STORE,
        DMA_RX_WRITE
    } dma_state_t;

    dma_state_t dma_state;
    logic [15:0] dma_base;
    logic [8:0]  dma_len;
    logic [8:0]  dma_idx;

    wire [8:0] tx_len_reg = (bank[OFF_DMALEN] == 8'h00) ? 9'd256
                                                        : {1'b0, bank[OFF_DMALEN]};
    wire [8:0] rx_len_reg = (bank[OFF_MSGLEN] == 8'h00) ? 9'd256
                                                        : {1'b0, bank[OFF_MSGLEN]};
    wire [15:0] dma_addr_reg = {bank[OFF_DMAH], bank[OFF_DMAL]};
    wire [15:0] dma_cur_addr = dma_base + {7'd0, dma_idx};

    assign rdy_out = (dma_state == DMA_IDLE);

    always_comb begin
        ram_addr  = dma_cur_addr;
        ram_wdata = 8'h00;
        ram_we    = 1'b0;
        ram_re    = 1'b0;

        if ((dma_state == DMA_TX_READ) || (dma_state == DMA_TX_WAIT)) begin
            ram_re = dma_grant;
        end else if (dma_state == DMA_RX_WRITE) begin
            ram_wdata = rx_buf[dma_idx[7:0]];
            ram_we    = dma_grant;
        end
    end

    function automatic logic range_ok(
        input logic [15:0] base_addr,
        input logic [8:0] len
    );
        logic [16:0] end_addr;
        begin
            end_addr = {1'b0, base_addr} + {8'd0, len};
            range_ok = (len != 9'd0) &&
                       ({1'b0, base_addr} < RAM_DMA_LIMIT) &&
                       (end_addr <= RAM_DMA_LIMIT);
        end
    endfunction

    function automatic logic is_slot_status(input logic [5:0] off);
        begin
            is_slot_status = (off >= OFF_SLOTST0[5:0]) && (off <= (OFF_SLOTST0[5:0] + 6'd3));
        end
    endfunction

    task automatic maybe_latch_irq(
        input logic [5:0] off,
        input logic [7:0] old_value,
        input logic [7:0] new_value
    );
        logic [1:0] slot;
        logic       rising_event;
        begin
            if (is_slot_status(off)) begin
                slot = off[1:0];
                rising_event = |((new_value & IRQ_EVENT_MASK) & ~(old_value & IRQ_EVENT_MASK));
                if (rising_event && bank[OFF_IRQCTRL][slot])
                    bank[OFF_IRQSTATUS][slot] <= 1'b1;
            end
        end
    endtask

    integer i;
    initial begin
        for (i = 0; i < 256; i = i + 1) begin
            tx_buf[i] = 8'h00;
            rx_buf[i] = 8'h00;
        end
        dma_state = DMA_IDLE;
        dma_base = 16'h0000;
        dma_len = 9'd0;
        dma_idx = 9'd0;
        nic_event = 1'b0;
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            for (i = 0; i < SIZE; i = i + 1)
                bank[i] <= 8'h00;
            bank[OFF_STATUS]  <= STATUS_READY;
            bank[6'h18]       <= SLOT_SENDREADY;
            bank[6'h19]       <= SLOT_SENDREADY;
            bank[6'h1A]       <= SLOT_SENDREADY;
            bank[6'h1B]       <= SLOT_SENDREADY;
            dma_state          <= DMA_IDLE;
            dma_base           <= 16'h0000;
            dma_len            <= 9'd0;
            dma_idx            <= 9'd0;
            nic_event          <= 1'b0;
        end else begin
            nic_event <= 1'b0;

            if (dbg_buf_we && dbg_buf_sel)
                rx_buf[dbg_buf_addr] <= dbg_buf_wdata;

            if (dbg_we) begin
                bank[dbg_addr] <= dbg_wdata;
                maybe_latch_irq(dbg_addr, bank[dbg_addr], dbg_wdata);
            end

            if (cpu_irqstatus_read)
                bank[OFF_IRQSTATUS] <= 8'h00;

            if (cpu_ce && cpu_we && sel) begin
                bank[cpu_off] <= cpu_wdata;
                maybe_latch_irq(cpu_off, bank[cpu_off], cpu_wdata);

                if ((cpu_off == OFF_CMD) && (cpu_wdata != 8'h00)) begin
                    if (cpu_wdata == CMD_SEND) begin
                        if (dma_state != DMA_IDLE) begin
                            bank[OFF_DMASTATUS] <= DMAST_ERROR;
                            bank[OFF_DMAERR]    <= DMAERR_BUSY;
                            nic_event           <= 1'b1;
                        end else if (!range_ok(dma_addr_reg, tx_len_reg)) begin
                            bank[OFF_DMASTATUS] <= DMAST_ERROR;
                            bank[OFF_DMAERR]    <= DMAERR_RANGE;
                            nic_event           <= 1'b1;
                        end else begin
                            dma_state           <= DMA_TX_READ;
                            dma_base            <= dma_addr_reg;
                            dma_len             <= tx_len_reg;
                            dma_idx             <= 9'd0;
                            bank[OFF_DMASTATUS] <= DMAST_BUSY;
                            bank[OFF_DMAERR]    <= DMAERR_NONE;
                        end
                    end else begin
                        nic_event <= 1'b1;
                    end
                end
            end

            if (dbg_hostctrl_write && (dbg_wdata & HOSTCTRL_RX_START)) begin
                bank[OFF_HOSTCTRL] <= 8'h00;
                if (dma_state != DMA_IDLE) begin
                    bank[OFF_DMASTATUS] <= DMAST_ERROR;
                    bank[OFF_DMAERR]    <= DMAERR_BUSY;
                end else if (!range_ok(dma_addr_reg, rx_len_reg)) begin
                    bank[OFF_DMASTATUS] <= DMAST_ERROR;
                    bank[OFF_DMAERR]    <= DMAERR_RANGE;
                end else begin
                    dma_state           <= DMA_RX_WRITE;
                    dma_base            <= dma_addr_reg;
                    dma_len             <= rx_len_reg;
                    dma_idx             <= 9'd0;
                    bank[OFF_DMASTATUS] <= DMAST_BUSY;
                    bank[OFF_DMAERR]    <= DMAERR_NONE;
                end
            end

            case (dma_state)
                DMA_IDLE: ;

                DMA_TX_READ: begin
                    if (dma_grant)
                        dma_state <= DMA_TX_WAIT;
                end

                DMA_TX_WAIT: begin
                    if (dma_grant)
                        dma_state <= DMA_TX_STORE;
                end

                DMA_TX_STORE: begin
                    if (dma_grant) begin
                        tx_buf[dma_idx[7:0]] <= ram_rdata;
                        if (dma_idx + 9'd1 == dma_len) begin
                            dma_state           <= DMA_IDLE;
                            bank[OFF_DMASTATUS] <= DMAST_TX_READY;
                            bank[OFF_DMAERR]    <= DMAERR_NONE;
                            nic_event           <= 1'b1;
                        end else begin
                            dma_idx   <= dma_idx + 9'd1;
                            dma_state <= DMA_TX_READ;
                        end
                    end
                end

                DMA_RX_WRITE: begin
                    if (dma_grant) begin
                        if (dma_idx + 9'd1 == dma_len) begin
                            dma_state           <= DMA_IDLE;
                            bank[OFF_DMASTATUS] <= DMAST_RX_DONE;
                            bank[OFF_DMAERR]    <= DMAERR_NONE;
                        end else begin
                            dma_idx <= dma_idx + 9'd1;
                        end
                    end
                end
            endcase
        end
    end

endmodule
