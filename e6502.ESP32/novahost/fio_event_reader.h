// Host-service event reader — async events from the FPGA debug bridge.
//
// The FPGA emits a 2-byte sequence (0xFE, event-type) over the host bridge whenever
// the 6502 writes a non-zero command byte to a host-backed register bank.
// FIO uses event type 0xE0; NIC uses 0xE1. This class detects that sequence
// and dispatches callbacks from FpgaBridge::drain() / recvStatus().
//
// Bytes that don't match the event marker are logged + dropped (they
// indicate a stale command response or a protocol error).

#ifndef NOVAHOST_FIO_EVENT_READER_H
#define NOVAHOST_FIO_EVENT_READER_H

#include <Arduino.h>

class FioEventReader {
public:
    using Handler = void (*)(void* user);

    FioEventReader()
        : _state(STATE_IDLE),
          _fioHandler(nullptr), _fioUser(nullptr),
          _nicHandler(nullptr), _nicUser(nullptr) {}

    void onEvent(Handler h, void* user) { onFioEvent(h, user); }
    void onFioEvent(Handler h, void* user) { _fioHandler = h; _fioUser = user; }
    void onNicEvent(Handler h, void* user) { _nicHandler = h; _nicUser = user; }

    void consume(uint8_t value);
    static void onDrainByteStatic(void* user, uint8_t value) {
        static_cast<FioEventReader*>(user)->consume(value);
    }

private:
    enum State { STATE_IDLE, STATE_AWAIT_TYPE };
    State           _state;
    Handler         _fioHandler;
    void*           _fioUser;
    Handler         _nicHandler;
    void*           _nicUser;

    void dispatch_event(uint8_t eventType);
};

#endif
