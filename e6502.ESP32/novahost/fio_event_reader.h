// FIO event reader — async events from the FPGA debug bridge.
//
// The FPGA emits a 2-byte sequence (0xFE, 0xE0) over the UART whenever
// the 6502 writes a non-zero value to FioCmd ($B9A0). This class
// detects that sequence and dispatches a callback. It must be polled
// from the main loop AND called by FpgaBridge::drain() so events
// queued before a CMD don't get dumped into the void.
//
// Bytes that don't match the event marker are logged + dropped (they
// indicate a stale command response or a protocol error).

#ifndef NOVAHOST_FIO_EVENT_READER_H
#define NOVAHOST_FIO_EVENT_READER_H

#include <Arduino.h>
#include <HardwareSerial.h>

class FioEventReader {
public:
    using Handler = void (*)(void* user);

    FioEventReader(HardwareSerial& serial)
        : _serial(serial), _state(STATE_IDLE),
          _handler(nullptr), _user(nullptr) {}

    void onEvent(Handler h, void* user) { _handler = h; _user = user; }

    // Pump any pending bytes from the serial buffer. Dispatches a FIO
    // event when the 0xFE 0xE0 sequence is seen. Logs other bytes via
    // a printf-like callback (Serial.printf for now) so we notice
    // protocol drift.
    //
    // Returns the number of bytes processed.
    int poll();

    void consume(uint8_t value);
    static void onDrainByteStatic(void* user, uint8_t value) {
        static_cast<FioEventReader*>(user)->consume(value);
    }

    // Like poll() but bounded to a small window — used by FpgaBridge
    // before/after CMDs so events aren't lost when the bridge would
    // otherwise drain stale bytes. Stops once the buffer is empty.
    int pump_until_empty() { return poll(); }

private:
    enum State { STATE_IDLE, STATE_AWAIT_TYPE };
    HardwareSerial& _serial;
    State           _state;
    Handler         _handler;
    void*           _user;

    void dispatch_fio_event();
};

#endif
