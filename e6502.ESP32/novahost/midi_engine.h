#ifndef NOVAHOST_MIDI_ENGINE_H
#define NOVAHOST_MIDI_ENGINE_H

#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace nova_midi {

static constexpr int MaxWtsVoices = 8;

class Input {
public:
    virtual ~Input() = default;
    virtual size_t pos() const = 0;
    virtual size_t size() const = 0;
    virtual bool seek(size_t pos) = 0;
    virtual bool read(uint8_t* dest, size_t len) = 0;

    size_t remaining() const {
        size_t p = pos();
        size_t s = size();
        return p <= s ? s - p : 0;
    }

    bool skip(size_t count) {
        if (count > remaining())
            return false;
        return seek(pos() + count);
    }
};

struct ChannelInfo {
    uint8_t channel = 0;
    uint16_t note_count = 0;
    uint8_t gm_program = 0;
    uint8_t max_polyphony = 0;
};

struct TimelineEvent {
    uint32_t tick = 0;
    int8_t voice = 0;       // -1 for tempo
    int16_t note = -1;      // -1 for note-off
    uint8_t velocity = 0;
    uint8_t instrument = 0;  // resident-wave fallback instrument
    uint8_t bank = 0;        // 0=melodic, 128=drums
    uint8_t program = 0;     // GM program for sample-bank lookup
    uint16_t bpm = 0;       // only used when voice == -1
};

struct Song {
    uint16_t ppqn = 0;
    uint8_t voice_count = 0;
    ChannelInfo channels[16];
    uint8_t voice_channels[MaxWtsVoices];
    std::vector<TimelineEvent> timeline;
};

bool build_song(const uint8_t* data, size_t size, Song& out,
                char* error, size_t error_size);
bool build_song(Input& input, Song& out, char* error, size_t error_size);
bool build_song_plan(Input& input, Song& out, char* error, size_t error_size);
bool build_song_window(Input& input, const Song& plan,
                       uint32_t start_tick, uint32_t end_tick,
                       std::vector<TimelineEvent>& timeline,
                       bool& has_more,
                       char* error, size_t error_size,
                       size_t max_events = 3072);
void reset_song_window_stream();
bool begin_song_window_stream(Input& input, const Song& plan,
                              char* error, size_t error_size);
bool build_song_stream_window(Input& input,
                              uint32_t start_tick, uint32_t end_tick,
                              std::vector<TimelineEvent>& timeline,
                              bool& has_more,
                              char* error, size_t error_size,
                              size_t max_events = 3072);

uint8_t hardware_instrument_for(uint8_t channel, uint8_t gm_program);

} // namespace nova_midi

#endif
