#include "fio_event_reader.h"

// Mirrors debug_bridge.sv:EVENT_MARKER / EVENT_TYPE_FIO.
static constexpr uint8_t EVENT_MARKER   = 0xFE;
static constexpr uint8_t EVENT_TYPE_FIO = 0xE0;

int FioEventReader::poll() {
    int processed = 0;
    while (_serial.available()) {
        int b = _serial.read();
        if (b < 0) break;
        processed++;
        consume((uint8_t)b);
    }
    return processed;
}

void FioEventReader::consume(uint8_t value) {
    switch (_state) {
        case STATE_IDLE:
            if (value == EVENT_MARKER) {
                _state = STATE_AWAIT_TYPE;
                unsigned long deadline = millis() + 5;
                while (!_serial.available() && millis() < deadline) {
                    delay(1);
                }
                if (_serial.available()) {
                    int next = _serial.read();
                    if (next >= 0) {
                        consume((uint8_t)next);
                    }
                }
            } else {
                // Stale CMD response byte left over from a timed-out
                // request. Drop it — there's no good way to dispatch
                // an orphaned response, and this matches the previous
                // drain() behaviour.
                Serial.printf("[fio] stale rx 0x%02X dropped\n",
                              (unsigned)value);
            }
            break;

        case STATE_AWAIT_TYPE:
            if (value == EVENT_TYPE_FIO) {
                dispatch_fio_event();
            } else {
                Serial.printf("[fio] unknown event type 0x%02X "
                              "(expected 0x%02X)\n",
                              (unsigned)value, EVENT_TYPE_FIO);
            }
            _state = STATE_IDLE;
            break;
    }
}

void FioEventReader::dispatch_fio_event() {
    if (_handler) _handler(_user);
}
