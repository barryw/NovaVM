#include "fio_event_reader.h"

// Mirrors debug_bridge.sv:EVENT_MARKER / EVENT_TYPE_FIO.
static constexpr uint8_t EVENT_MARKER   = 0xFE;
static constexpr uint8_t EVENT_TYPE_FIO = 0xE0;
static constexpr uint8_t EVENT_TYPE_NIC = 0xE1;

void FioEventReader::consume(uint8_t value) {
    switch (_state) {
        case STATE_IDLE:
            if (value == EVENT_MARKER) {
                _state = STATE_AWAIT_TYPE;
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
            if (value == EVENT_TYPE_FIO || value == EVENT_TYPE_NIC) {
                dispatch_event(value);
            } else {
                Serial.printf("[fio] unknown event type 0x%02X "
                              "(expected 0x%02X or 0x%02X)\n",
                              (unsigned)value, EVENT_TYPE_FIO,
                              EVENT_TYPE_NIC);
            }
            _state = STATE_IDLE;
            break;
    }
}

void FioEventReader::dispatch_event(uint8_t eventType) {
    if (eventType == EVENT_TYPE_FIO) {
        if (_fioHandler) _fioHandler(_fioUser);
    } else if (eventType == EVENT_TYPE_NIC) {
        if (_nicHandler) _nicHandler(_nicUser);
    }
}
