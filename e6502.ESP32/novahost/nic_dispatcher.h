// NicDispatcher — NovaHost backing for the FPGA NIC register bank.
//
// The FPGA emits EVENT_NIC when the 6502 writes a non-zero NIC_CMD at $A100.
// This dispatcher reads the $A100-$A13F register bank, performs the requested
// socket operation with WiFiClient, and writes status/result bytes back through
// the debug bridge.

#ifndef NOVAHOST_NIC_DISPATCHER_H
#define NOVAHOST_NIC_DISPATCHER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "fpga_bridge.h"

class NicDispatcher {
public:
    explicit NicDispatcher(FpgaBridge& bridge) : _bridge(bridge) {}

    static void onNicEventStatic(void* user) {
        static_cast<NicDispatcher*>(user)->mark_pending();
    }

    void mark_pending() { _pending = true; }

    // Poll active sockets and service any pending NIC command.
    void poll();
    void poll_pending();
    void write_debug_json(char* out, size_t out_size) const;

private:
    static constexpr uint16_t BANK_BASE = 0xA100;
    static constexpr int BANK_SIZE = 64;
    static constexpr int MAX_SLOTS = 4;
    static constexpr int MAX_MSG_SIZE = 256;
    static constexpr int MAX_QUEUE = 16;

    static constexpr int OFF_CMD       = 0x00;
    static constexpr int OFF_STATUS    = 0x01;
    static constexpr int OFF_SLOT      = 0x02;
    static constexpr int OFF_IRQSTATUS = 0x04;
    static constexpr int OFF_CMDSEQ    = 0x05;
    static constexpr int OFF_CMDSHADOW = 0x06;
    static constexpr int OFF_HOSTCTRL  = 0x07;
    static constexpr int OFF_RPORTL    = 0x08;
    static constexpr int OFF_RPORTH    = 0x09;
    static constexpr int OFF_DMAL      = 0x10;
    static constexpr int OFF_DMAH      = 0x11;
    static constexpr int OFF_DMALEN    = 0x12;
    static constexpr int OFF_MSGLEN    = 0x13;
    static constexpr int OFF_DMASTATUS = 0x14;
    static constexpr int OFF_DMAERR    = 0x15;
    static constexpr int OFF_SLOTST0   = 0x18;
    static constexpr int OFF_NAME      = 0x20;
    static constexpr int NAME_LIMIT    = 32;

    static constexpr uint8_t CMD_CONNECT    = 0x01;
    static constexpr uint8_t CMD_DISCONNECT = 0x02;
    static constexpr uint8_t CMD_SEND       = 0x03;
    static constexpr uint8_t CMD_RECV       = 0x04;
    static constexpr uint8_t CMD_LISTEN     = 0x05;
    static constexpr uint8_t CMD_ACCEPT     = 0x06;

    static constexpr uint8_t ST_READY    = 0x01;
    static constexpr uint8_t ST_ANYDATA  = 0x02;
    static constexpr uint8_t ST_ANYERROR = 0x80;

    static constexpr uint8_t SLOT_CONNECTED     = 0x01;
    static constexpr uint8_t SLOT_DATAREADY     = 0x02;
    static constexpr uint8_t SLOT_SENDREADY     = 0x04;
    static constexpr uint8_t SLOT_ERROR         = 0x08;
    static constexpr uint8_t SLOT_REMOTE_CLOSED = 0x10;

    static constexpr uint8_t HOSTCTRL_RX_START = 0x01;
    static constexpr uint8_t DMAST_TX_READY    = 0x01;
    static constexpr uint8_t DMAST_RX_DONE     = 0x02;
    static constexpr uint8_t DMAST_ERROR       = 0x40;
    static constexpr uint8_t DMAST_BUSY        = 0x80;

    struct QueuedMessage {
        uint16_t len = 0;
        uint8_t data[MAX_MSG_SIZE];
    };

    struct SlotState {
        WiFiClient client;
        bool connected = false;
        bool error = false;
        bool remoteClosed = false;
        bool rxActive = false;
        uint16_t rxExpected = 0;
        uint16_t rxReceived = 0;
        uint8_t rxBuffer[MAX_MSG_SIZE];
        QueuedMessage queue[MAX_QUEUE];
        uint8_t head = 0;
        uint8_t tail = 0;
        uint8_t count = 0;
        uint8_t lastStatus = 0xFF;
    };

    FpgaBridge& _bridge;
    bool _pending = false;
    bool _handling = false;
    bool _commandClearPending = false;
    bool _messageLengthPending = false;
    uint16_t _pendingMessageLength = 0;
    unsigned long _lastCommandPollMs = 0;
    uint32_t _fallbackCommandPolls = 0;
    uint32_t _fallbackCommands = 0;
    bool _configLoaded = false;
    bool _configLoadedOk = false;
    uint8_t _bank[BANK_SIZE] = {0};
    uint8_t _lastGlobalStatus = 0xFF;
    SlotState _slots[MAX_SLOTS];
    char _gameServerHost[64] = "";
    uint16_t _gameServerPort = 6503;
    uint8_t _lastCommand = 0;
    uint8_t _lastSlot = 0;
    uint16_t _lastPort = 0;
    bool _lastConnectOk = false;
    uint16_t _lastSendLen = 0;
    size_t _lastSendHeaderBytes = 0;
    size_t _lastSendPayloadBytes = 0;
    uint8_t _lastSendFirst[8] = {0};
    uint8_t _lastCommandSeq = 0;
    char _lastRequestedHost[64] = "";
    char _lastResolvedHost[64] = "";
    char _lastError[64] = "";

    void handle_event();
    void handle_connect(uint8_t slot);
    void handle_disconnect(uint8_t slot);
    void handle_send(uint8_t slot);
    void handle_recv(uint8_t slot);
    void handle_unsupported(uint8_t slot, const char* name);

    void poll_command_register();
    void poll_socket(uint8_t slot);
    bool enqueue(uint8_t slot, const uint8_t* data, uint16_t len);
    bool dequeue(uint8_t slot, QueuedMessage& msg);
    void reset_slot(uint8_t slot);

    uint8_t build_slot_status(uint8_t slot) const;
    uint8_t build_global_status() const;
    void sync_status(bool force = false);
    void write_slot_status(uint8_t slot, bool force = false);
    void write_global_status(bool force = false);
    void clear_command();
    void retry_clear_command();
    bool is_new_command(uint8_t cmd, uint8_t seq) const;
    bool write_message_length_immediate(uint16_t len);
    bool wait_dma_complete(uint8_t expectedReadyBit);
    bool set_message_length(uint16_t len);
    void retry_message_length();

    uint8_t selected_slot() const;
    uint16_t remote_port() const;
    uint16_t dma_address() const;
    uint16_t dma_length() const;
    void read_hostname(char* out, size_t out_size) const;
    void resolve_endpoint(const char* requestedHost, uint16_t requestedPort, char* hostOut, size_t hostOutSize, uint16_t& portOut);
    void load_config();
};

#endif
