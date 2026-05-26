#include "midi_engine.h"

#include <algorithm>
#include <stdio.h>
#include <string.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif

namespace nova_midi {

#if defined(ARDUINO)

namespace {

void set_error(char* error, size_t error_size, const char* msg) {
    if (!error || error_size == 0)
        return;
    snprintf(error, error_size, "%s", msg);
}

} // namespace

bool build_song(const uint8_t*, size_t, Song&, char* error,
                size_t error_size) {
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

bool build_song(Input&, Song&, char* error, size_t error_size) {
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

bool build_song_plan(Input&, Song&, char* error, size_t error_size) {
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

bool build_song_window(Input&, const Song&, uint32_t, uint32_t,
                       std::vector<TimelineEvent>& timeline,
                       bool& has_more, char* error, size_t error_size,
                       size_t) {
    timeline.clear();
    has_more = false;
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

void reset_song_window_stream() {
}

bool begin_song_window_stream(Input&, const Song&, char* error,
                              size_t error_size) {
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

bool build_song_stream_window(Input&, uint32_t, uint32_t,
                              std::vector<TimelineEvent>& timeline,
                              bool& has_more, char* error,
                              size_t error_size, size_t) {
    timeline.clear();
    has_more = false;
    set_error(error, error_size, "raw MIDI parsing is not available on ESP");
    return false;
}

uint8_t hardware_instrument_for(uint8_t, uint8_t gm_program) {
    return gm_program & 0x7F;
}

} // namespace nova_midi

#else

namespace {

void midi_yield() {
#if defined(ARDUINO)
    yield();
#endif
}

enum class RawKind : uint8_t {
    NoteOff,
    NoteOn,
    ProgramChange,
    Tempo,
};

struct RawEvent {
    uint32_t tick = 0;
    RawKind kind = RawKind::NoteOff;
    uint8_t channel = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
    uint8_t program = 0;
    uint16_t bpm = 0;
};

class Reader {
public:
    Reader(const uint8_t* data, size_t size) : _data(data), _size(size) {}

    size_t pos() const { return _pos; }
    size_t remaining() const { return _pos <= _size ? _size - _pos : 0; }
    bool seek(size_t pos) {
        if (pos > _size) return false;
        _pos = pos;
        return true;
    }
    bool skip(size_t count) {
        if (count > remaining()) return false;
        _pos += count;
        return true;
    }
    bool read_u8(uint8_t& value) {
        if (remaining() < 1) return false;
        value = _data[_pos++];
        return true;
    }
    bool read_be16(uint16_t& value) {
        uint8_t b0, b1;
        if (!read_u8(b0) || !read_u8(b1)) return false;
        value = ((uint16_t)b0 << 8) | b1;
        return true;
    }
    bool read_be32(uint32_t& value) {
        uint8_t b0, b1, b2, b3;
        if (!read_u8(b0) || !read_u8(b1) ||
            !read_u8(b2) || !read_u8(b3)) return false;
        value = ((uint32_t)b0 << 24) | ((uint32_t)b1 << 16) |
                ((uint32_t)b2 << 8) | b3;
        return true;
    }
    bool read_var(uint32_t& value) {
        value = 0;
        for (int i = 0; i < 4; i++) {
            uint8_t b;
            if (!read_u8(b)) return false;
            value = (value << 7) | (uint32_t)(b & 0x7F);
            if ((b & 0x80) == 0) return true;
        }
        return false;
    }
    bool read_fourcc(char out[5]) {
        if (remaining() < 4) return false;
        memcpy(out, _data + _pos, 4);
        out[4] = 0;
        _pos += 4;
        return true;
    }

private:
    const uint8_t* _data;
    size_t _size;
    size_t _pos = 0;
};

class InputReader {
public:
    explicit InputReader(Input& input) : _input(input) {}

    size_t pos() const { return _input.pos(); }
    size_t remaining() const { return _input.remaining(); }
    bool seek(size_t pos) { return _input.seek(pos); }
    bool skip(size_t count) { return _input.skip(count); }
    bool read_u8(uint8_t& value) {
        return _input.read(&value, 1);
    }
    bool read_be16(uint16_t& value) {
        uint8_t b0, b1;
        if (!read_u8(b0) || !read_u8(b1)) return false;
        value = ((uint16_t)b0 << 8) | b1;
        return true;
    }
    bool read_be32(uint32_t& value) {
        uint8_t b0, b1, b2, b3;
        if (!read_u8(b0) || !read_u8(b1) ||
            !read_u8(b2) || !read_u8(b3)) return false;
        value = ((uint32_t)b0 << 24) | ((uint32_t)b1 << 16) |
                ((uint32_t)b2 << 8) | b3;
        return true;
    }
    bool read_var(uint32_t& value) {
        value = 0;
        for (int i = 0; i < 4; i++) {
            uint8_t b;
            if (!read_u8(b)) return false;
            value = (value << 7) | (uint32_t)(b & 0x7F);
            if ((b & 0x80) == 0) return true;
        }
        return false;
    }
    bool read_fourcc(char out[5]) {
        if (remaining() < 4) return false;
        if (!_input.read((uint8_t*)out, 4)) return false;
        out[4] = 0;
        return true;
    }

private:
    Input& _input;
};

void set_error(char* error, size_t error_size, const char* msg) {
    if (!error || error_size == 0) return;
    snprintf(error, error_size, "%s", msg ? msg : "midi parse failed");
}

int midi_param_count(uint8_t status) {
    switch (status & 0xF0) {
        case 0xC0:
        case 0xD0:
            return 1;
        case 0x80:
        case 0x90:
        case 0xA0:
        case 0xB0:
        case 0xE0:
            return 2;
        default:
            return 0;
    }
}

template <typename TReader, typename TSink>
bool parse_track(TReader& r, size_t track_end, TSink& sink,
                 char* error, size_t error_size) {
    uint32_t tick = 0;
    uint8_t running = 0;
    uint16_t poll = 0;

    while (r.pos() < track_end) {
        if ((++poll & 0x3F) == 0)
            midi_yield();

        uint32_t delta = 0;
        if (!r.read_var(delta)) {
            set_error(error, error_size, "bad delta time");
            return false;
        }
        tick += delta;

        uint8_t status = 0;
        if (!r.read_u8(status)) {
            set_error(error, error_size, "missing status byte");
            return false;
        }

        uint8_t data0 = 0;
        bool has_data0 = false;
        if (status < 0x80) {
            if (running == 0) {
                set_error(error, error_size, "running status without status");
                return false;
            }
            data0 = status;
            has_data0 = true;
            status = running;
        } else if (status < 0xF0) {
            running = status;
        }

        if (status == 0xFF) {
            uint8_t meta_type = 0;
            uint32_t len = 0;
            if (!r.read_u8(meta_type) || !r.read_var(len)) {
                set_error(error, error_size, "bad meta event");
                return false;
            }
            if (r.pos() + len > track_end) {
                set_error(error, error_size, "meta event overruns track");
                return false;
            }
            if (meta_type == 0x51 && len == 3) {
                uint8_t b0, b1, b2;
                if (!r.read_u8(b0) || !r.read_u8(b1) || !r.read_u8(b2))
                    return false;
                uint32_t us_per_quarter =
                    ((uint32_t)b0 << 16) | ((uint32_t)b1 << 8) | b2;
                if (us_per_quarter != 0) {
                    if (!sink.on_event({
                        tick,
                        RawKind::Tempo,
                        0,
                        0,
                        0,
                        0,
                        (uint16_t)(60000000UL / us_per_quarter)
                    })) {
                        set_error(error, error_size, "midi event sink failed");
                        return false;
                    }
                }
            } else {
                if (!r.skip(len)) return false;
            }
            if (meta_type == 0x2F) break;
            continue;
        }

        if (status == 0xF0 || status == 0xF7) {
            uint32_t len = 0;
            if (!r.read_var(len) || r.pos() + len > track_end) {
                set_error(error, error_size, "bad sysex event");
                return false;
            }
            if (!r.skip(len)) return false;
            running = 0;
            continue;
        }

        int param_count = midi_param_count(status);
        if (param_count == 0) {
            set_error(error, error_size, "unsupported midi status");
            return false;
        }

        if (!has_data0 && !r.read_u8(data0)) {
            set_error(error, error_size, "missing midi data byte");
            return false;
        }

        uint8_t data1 = 0;
        if (param_count == 2 && !r.read_u8(data1)) {
            set_error(error, error_size, "missing midi data byte");
            return false;
        }

        uint8_t channel = status & 0x0F;
        switch (status & 0xF0) {
            case 0x80:
                if (!sink.on_event({ tick, RawKind::NoteOff, channel, data0, 0, 0, 0 })) {
                    set_error(error, error_size, "midi event sink failed");
                    return false;
                }
                break;
            case 0x90:
                if (!sink.on_event({
                    tick,
                    data1 == 0 ? RawKind::NoteOff : RawKind::NoteOn,
                    channel,
                    data0,
                    data1,
                    0,
                    0
                })) {
                    set_error(error, error_size, "midi event sink failed");
                    return false;
                }
                break;
            case 0xC0:
                if (!sink.on_event({ tick, RawKind::ProgramChange, channel, 0, 0, data0, 0 })) {
                    set_error(error, error_size, "midi event sink failed");
                    return false;
                }
                break;
            default:
                break;
        }
    }

    return r.pos() <= track_end && r.seek(track_end);
}

static constexpr uint16_t MaxStreamingTracks = 48;
static constexpr size_t TrackReadCacheSize = 256;

struct TrackCursor {
    size_t pos = 0;
    size_t end = 0;
    size_t cache_start = 0;
    size_t cache_valid = 0;
    uint32_t tick = 0;
    uint8_t running = 0;
    bool done = true;
    bool pending = false;
    uint16_t order = 0;
    RawEvent event;
    uint8_t cache[TrackReadCacheSize];
};

uint8_t raw_sort_priority(RawKind kind);

bool track_fill_cache(Input& input, TrackCursor& track) {
    if (track.pos >= track.end)
        return false;

    size_t wanted = track.end - track.pos;
    if (wanted > TrackReadCacheSize)
        wanted = TrackReadCacheSize;
    if (!input.seek(track.pos) || !input.read(track.cache, wanted))
        return false;

    track.cache_start = track.pos;
    track.cache_valid = wanted;
    return true;
}

bool track_read_u8(Input& input, TrackCursor& track, uint8_t& value) {
    if (track.pos >= track.end)
        return false;

    if (track.pos < track.cache_start ||
        track.pos >= track.cache_start + track.cache_valid) {
        if (!track_fill_cache(input, track))
            return false;
    }

    value = track.cache[track.pos - track.cache_start];
    track.pos++;
    return true;
}

bool track_skip(Input& input, TrackCursor& track, size_t count) {
    (void)input;
    if (count > track.end - track.pos)
        return false;
    track.pos += count;
    return true;
}

bool track_read_var(Input& input, TrackCursor& track, uint32_t& value) {
    value = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t b;
        if (!track_read_u8(input, track, b))
            return false;
        value = (value << 7) | (uint32_t)(b & 0x7F);
        if ((b & 0x80) == 0)
            return true;
    }
    return false;
}

bool init_streaming_tracks(InputReader& r, size_t size, uint16_t& ppqn,
                           TrackCursor tracks[MaxStreamingTracks],
                           uint16_t& track_count,
                           char* error, size_t error_size) {
    char id[5];
    uint32_t header_len = 0;
    uint16_t format = 0;
    uint16_t declared_tracks = 0;
    uint16_t division = 0;

    track_count = 0;
    if (!r.seek(0) ||
        !r.read_fourcc(id) || strcmp(id, "MThd") != 0 ||
        !r.read_be32(header_len) || header_len < 6 ||
        !r.read_be16(format) || !r.read_be16(declared_tracks) ||
        !r.read_be16(division)) {
        set_error(error, error_size, "not a standard midi file");
        return false;
    }

    if (format > 1) {
        set_error(error, error_size, "midi format is not supported");
        return false;
    }
    if ((division & 0x8000) != 0 || division == 0) {
        set_error(error, error_size, "smpte midi timing is not supported");
        return false;
    }

    ppqn = division;
    if (!r.seek(8 + header_len)) {
        set_error(error, error_size, "bad midi header length");
        return false;
    }

    for (uint16_t t = 0; t < declared_tracks; t++) {
        if (!r.read_fourcc(id)) {
            set_error(error, error_size, "missing track chunk");
            return false;
        }

        uint32_t len = 0;
        if (!r.read_be32(len)) {
            set_error(error, error_size, "bad track length");
            return false;
        }

        size_t track_start = r.pos();
        size_t track_end = track_start + len;
        if (track_end < track_start || track_end > size) {
            set_error(error, error_size, "track overruns file");
            return false;
        }

        if (strcmp(id, "MTrk") == 0) {
            if (track_count >= MaxStreamingTracks) {
                set_error(error, error_size, "too many midi tracks");
                return false;
            }
            tracks[track_count].pos = track_start;
            tracks[track_count].end = track_end;
            tracks[track_count].cache_start = 0;
            tracks[track_count].cache_valid = 0;
            tracks[track_count].tick = 0;
            tracks[track_count].running = 0;
            tracks[track_count].done = false;
            tracks[track_count].pending = false;
            tracks[track_count].order = track_count;
            track_count++;
        }

        if (!r.seek(track_end))
            return false;
    }

    if (track_count == 0) {
        set_error(error, error_size, "midi file contains no tracks");
        return false;
    }
    return true;
}

bool read_next_streaming_event(Input& input, TrackCursor& track,
                               char* error, size_t error_size) {
    track.pending = false;

    while (!track.done && track.pos < track.end) {
        uint32_t delta = 0;
        if (!track_read_var(input, track, delta)) {
            set_error(error, error_size, "bad delta time");
            return false;
        }
        track.tick += delta;

        uint8_t status = 0;
        if (!track_read_u8(input, track, status)) {
            set_error(error, error_size, "missing status byte");
            return false;
        }

        uint8_t data0 = 0;
        bool has_data0 = false;
        if (status < 0x80) {
            if (track.running == 0) {
                set_error(error, error_size, "running status without status");
                return false;
            }
            data0 = status;
            has_data0 = true;
            status = track.running;
        } else if (status < 0xF0) {
            track.running = status;
        }

        if (status == 0xFF) {
            uint8_t meta_type = 0;
            uint32_t len = 0;
            if (!track_read_u8(input, track, meta_type) ||
                !track_read_var(input, track, len)) {
                set_error(error, error_size, "bad meta event");
                return false;
            }
            if (track.pos + len > track.end) {
                set_error(error, error_size, "meta event overruns track");
                return false;
            }
            if (meta_type == 0x51 && len == 3) {
                uint8_t b0, b1, b2;
                if (!track_read_u8(input, track, b0) ||
                    !track_read_u8(input, track, b1) ||
                    !track_read_u8(input, track, b2))
                    return false;
                uint32_t us_per_quarter =
                    ((uint32_t)b0 << 16) | ((uint32_t)b1 << 8) | b2;
                if (us_per_quarter != 0) {
                    track.event = {
                        track.tick,
                        RawKind::Tempo,
                        0,
                        0,
                        0,
                        0,
                        (uint16_t)(60000000UL / us_per_quarter)
                    };
                    track.pending = true;
                    return true;
                }
            } else {
                if (!track_skip(input, track, len))
                    return false;
            }
            if (meta_type == 0x2F) {
                track.done = true;
                track.pos = track.end;
                return true;
            }
            continue;
        }

        if (status == 0xF0 || status == 0xF7) {
            uint32_t len = 0;
            if (!track_read_var(input, track, len) ||
                track.pos + len > track.end) {
                set_error(error, error_size, "bad sysex event");
                return false;
            }
            if (!track_skip(input, track, len))
                return false;
            track.running = 0;
            continue;
        }

        int param_count = midi_param_count(status);
        if (param_count == 0) {
            set_error(error, error_size, "unsupported midi status");
            return false;
        }

        if (!has_data0 && !track_read_u8(input, track, data0)) {
            set_error(error, error_size, "missing midi data byte");
            return false;
        }

        uint8_t data1 = 0;
        if (param_count == 2 && !track_read_u8(input, track, data1)) {
            set_error(error, error_size, "missing midi data byte");
            return false;
        }

        uint8_t channel = status & 0x0F;
        switch (status & 0xF0) {
            case 0x80:
                track.event = { track.tick, RawKind::NoteOff, channel, data0, 0, 0, 0 };
                track.pending = true;
                return true;
            case 0x90:
                track.event = {
                    track.tick,
                    data1 == 0 ? RawKind::NoteOff : RawKind::NoteOn,
                    channel,
                    data0,
                    data1,
                    0,
                    0
                };
                track.pending = true;
                return true;
            case 0xC0:
                track.event = { track.tick, RawKind::ProgramChange, channel, 0, 0, data0, 0 };
                track.pending = true;
                return true;
            default:
                break;
        }
    }

    track.done = true;
    return true;
}

template <typename TSink>
bool parse_streaming_events(Input& input, uint16_t& ppqn, TSink& sink,
                            char* error, size_t error_size) {
    static TrackCursor tracks[MaxStreamingTracks];
    memset(tracks, 0, sizeof(tracks));

    InputReader reader(input);
    uint16_t track_count = 0;
    if (!init_streaming_tracks(reader, input.size(), ppqn, tracks, track_count,
                               error, error_size)) {
        return false;
    }

    for (uint16_t i = 0; i < track_count; i++) {
        if (!read_next_streaming_event(input, tracks[i], error, error_size))
            return false;
    }

    uint16_t poll = 0;
    for (;;) {
        int best = -1;
        for (uint16_t i = 0; i < track_count; i++) {
            if (!tracks[i].pending)
                continue;
            if (best < 0 ||
                tracks[i].event.tick < tracks[best].event.tick ||
                (tracks[i].event.tick == tracks[best].event.tick &&
                 raw_sort_priority(tracks[i].event.kind) <
                    raw_sort_priority(tracks[best].event.kind)) ||
                (tracks[i].event.tick == tracks[best].event.tick &&
                 raw_sort_priority(tracks[i].event.kind) ==
                    raw_sort_priority(tracks[best].event.kind) &&
                 tracks[i].order < tracks[best].order)) {
                best = i;
            }
        }

        if (best < 0)
            break;

        if ((++poll & 0x3F) == 0)
            midi_yield();

        if (!sink.on_event(tracks[best].event)) {
            set_error(error, error_size, "midi event sink failed");
            return false;
        }
        if (sink.stop_requested())
            break;

        if (!read_next_streaming_event(input, tracks[best],
                                       error, error_size)) {
            return false;
        }
    }

    return true;
}

template <typename TReader, typename TSink>
bool parse_raw_events(TReader& r, size_t size, uint16_t& ppqn,
                      TSink& sink,
                      char* error, size_t error_size) {
    char id[5];
    uint32_t header_len = 0;
    uint16_t format = 0;
    uint16_t tracks = 0;
    uint16_t division = 0;

    if (!r.read_fourcc(id) || strcmp(id, "MThd") != 0 ||
        !r.read_be32(header_len) || header_len < 6 ||
        !r.read_be16(format) || !r.read_be16(tracks) || !r.read_be16(division)) {
        set_error(error, error_size, "not a standard midi file");
        return false;
    }

    if (format > 1) {
        set_error(error, error_size, "midi format is not supported");
        return false;
    }
    if ((division & 0x8000) != 0 || division == 0) {
        set_error(error, error_size, "smpte midi timing is not supported");
        return false;
    }

    ppqn = division;
    if (!r.seek(8 + header_len)) {
        set_error(error, error_size, "bad midi header length");
        return false;
    }

    for (uint16_t t = 0; t < tracks; t++) {
        if (!r.read_fourcc(id)) {
            set_error(error, error_size, "missing track chunk");
            return false;
        }
        uint32_t len = 0;
        if (!r.read_be32(len)) {
            set_error(error, error_size, "bad track length");
            return false;
        }
        size_t track_end = r.pos() + len;
        if (track_end > size) {
            set_error(error, error_size, "track overruns file");
            return false;
        }
        if (strcmp(id, "MTrk") != 0) {
            if (!r.seek(track_end)) return false;
            continue;
        }
        if (!parse_track(r, track_end, sink, error, error_size))
            return false;
    }

    return true;
}

uint8_t raw_sort_priority(RawKind kind) {
    switch (kind) {
        case RawKind::Tempo: return 0;
        case RawKind::ProgramChange: return 1;
        case RawKind::NoteOff: return 2;
        case RawKind::NoteOn: return 3;
    }
    return 4;
}

void store_voice_channels(const std::vector<uint8_t>& voices, Song& song) {
    memset(song.voice_channels, 0, sizeof(song.voice_channels));
    for (size_t i = 0; i < voices.size() && i < MaxWtsVoices; i++)
        song.voice_channels[i] = voices[i];
}

std::vector<uint8_t> stored_voice_channels(const Song& song) {
    std::vector<uint8_t> voices;
    for (uint8_t i = 0; i < song.voice_count && i < MaxWtsVoices; i++)
        voices.push_back(song.voice_channels[i]);
    return voices;
}

void analyze_channels(const std::vector<RawEvent>& raw, ChannelInfo channels[16]) {
    for (int i = 0; i < 16; i++) {
        channels[i] = {};
        channels[i].channel = (uint8_t)i;
    }

    bool active[16][128];
    memset(active, 0, sizeof(active));

    for (const RawEvent& ev : raw) {
        if (ev.channel >= 16) continue;
        switch (ev.kind) {
            case RawKind::NoteOn: {
                channels[ev.channel].note_count++;
                if (ev.note < 128)
                    active[ev.channel][ev.note] = true;
                uint8_t count = 0;
                for (int n = 0; n < 128; n++)
                    if (active[ev.channel][n]) count++;
                if (count > channels[ev.channel].max_polyphony)
                    channels[ev.channel].max_polyphony = count;
                break;
            }
            case RawKind::NoteOff:
                if (ev.note < 128)
                    active[ev.channel][ev.note] = false;
                break;
            case RawKind::ProgramChange:
                channels[ev.channel].gm_program = ev.program & 0x7F;
                break;
            case RawKind::Tempo:
                break;
        }
    }
}

std::vector<uint8_t> select_channels(const ChannelInfo channels[16], int max_voices) {
    std::vector<uint8_t> active;
    for (int i = 0; i < 16; i++) {
        if (channels[i].note_count > 0)
            active.push_back((uint8_t)i);
    }

    std::sort(active.begin(), active.end(), [&](uint8_t a, uint8_t b) {
        if (channels[a].note_count != channels[b].note_count)
            return channels[a].note_count > channels[b].note_count;
        return a < b;
    });

    std::vector<uint8_t> result;
    for (uint8_t ch : active) {
        if ((int)result.size() >= max_voices) break;
        result.push_back(ch);
    }

    int remaining = max_voices - (int)result.size();
    for (uint8_t ch : active) {
        if (remaining <= 0) break;
        int extra = std::min(remaining, std::max(0, (int)channels[ch].max_polyphony - 1));
        for (int i = 0; i < extra; i++)
            result.push_back(ch);
        remaining -= extra;
    }

    return result;
}

void build_timeline(const std::vector<RawEvent>& raw,
                    const ChannelInfo channels[16],
                    const std::vector<uint8_t>& voice_to_channel,
                    std::vector<TimelineEvent>& timeline) {
    std::vector<uint8_t> channel_voices[16];
    for (size_t v = 0; v < voice_to_channel.size(); v++) {
        uint8_t ch = voice_to_channel[v];
        if (ch < 16)
            channel_voices[ch].push_back((uint8_t)v);
    }

    int8_t note_to_voice[16][128];
    int16_t voice_note[MaxWtsVoices];
    uint32_t voice_start_tick[MaxWtsVoices];
    uint8_t channel_program[16];
    for (int ch = 0; ch < 16; ch++)
        for (int n = 0; n < 128; n++)
            note_to_voice[ch][n] = -1;
    for (int ch = 0; ch < 16; ch++)
        channel_program[ch] = 0;
    for (int v = 0; v < MaxWtsVoices; v++) {
        voice_note[v] = -1;
        voice_start_tick[v] = 0;
    }

    for (const RawEvent& ev : raw) {
        if (ev.kind == RawKind::Tempo) {
            timeline.push_back({ ev.tick, -1, -1, 0, 0, 0, 0, ev.bpm });
            continue;
        }

        if (ev.channel >= 16)
            continue;

        if (ev.kind == RawKind::ProgramChange) {
            channel_program[ev.channel] = ev.program & 0x7F;
            continue;
        }

        if (ev.note >= 128)
            continue;

        const std::vector<uint8_t>& voices = channel_voices[ev.channel];
        if (voices.empty())
            continue;

        if (ev.kind == RawKind::NoteOn) {
            int8_t existing_voice = note_to_voice[ev.channel][ev.note];
            if (existing_voice >= 0) {
                timeline.push_back({
                    ev.tick,
                    (int8_t)existing_voice,
                    ev.note,
                    ev.velocity,
                    hardware_instrument_for(ev.channel, channel_program[ev.channel]),
                    ev.channel == 9 ? (uint8_t)128 : (uint8_t)0,
                    ev.channel == 9 ? (uint8_t)0 : channel_program[ev.channel],
                    0
                });
                voice_start_tick[existing_voice] = ev.tick;
                continue;
            }

            int assigned_voice = -1;
            for (uint8_t v : voices) {
                if (voice_note[v] < 0) {
                    assigned_voice = v;
                    break;
                }
            }

            if (assigned_voice < 0) {
                uint32_t oldest_tick = UINT32_MAX;
                for (uint8_t v : voices) {
                    if (voice_start_tick[v] < oldest_tick) {
                        oldest_tick = voice_start_tick[v];
                        assigned_voice = v;
                    }
                }
                int16_t stolen_note = voice_note[assigned_voice];
                if (stolen_note >= 0 && stolen_note < 128)
                    note_to_voice[ev.channel][stolen_note] = -1;
            }

            note_to_voice[ev.channel][ev.note] = assigned_voice;
            voice_note[assigned_voice] = ev.note;
            voice_start_tick[assigned_voice] = ev.tick;

            timeline.push_back({
                ev.tick,
                (int8_t)assigned_voice,
                ev.note,
                ev.velocity,
                hardware_instrument_for(ev.channel, channel_program[ev.channel]),
                ev.channel == 9 ? (uint8_t)128 : (uint8_t)0,
                ev.channel == 9 ? (uint8_t)0 : channel_program[ev.channel],
                0
            });
        } else if (ev.kind == RawKind::NoteOff) {
            int8_t released_voice = note_to_voice[ev.channel][ev.note];
            if (released_voice >= 0) {
                note_to_voice[ev.channel][ev.note] = -1;
                voice_note[released_voice] = -1;
                timeline.push_back({
                    ev.tick,
                    (int8_t)released_voice,
                    -1,
                    0,
                    0,
                    0,
                    0,
                    0
                });
            }
        }
    }

    std::sort(timeline.begin(), timeline.end(), [](const TimelineEvent& a,
                                                   const TimelineEvent& b) {
        if (a.tick != b.tick) return a.tick < b.tick;
        if (a.voice != b.voice) return a.voice < b.voice;
        return a.note < b.note;
    });
}

struct RawCollectSink {
    std::vector<RawEvent>& events;

    bool on_event(const RawEvent& ev) {
        events.push_back(ev);
        return true;
    }

    bool stop_requested() const { return false; }
};

struct AnalyzeSink {
    ChannelInfo channels[16];
    bool active[16][128];

    AnalyzeSink() {
        memset(active, 0, sizeof(active));
        for (int i = 0; i < 16; i++) {
            channels[i] = {};
            channels[i].channel = (uint8_t)i;
        }
    }

    bool on_event(const RawEvent& ev) {
        if (ev.channel >= 16)
            return true;

        switch (ev.kind) {
            case RawKind::NoteOn: {
                channels[ev.channel].note_count++;
                if (ev.note < 128)
                    active[ev.channel][ev.note] = true;
                uint8_t count = 0;
                for (int n = 0; n < 128; n++) {
                    if (active[ev.channel][n])
                        count++;
                }
                if (count > channels[ev.channel].max_polyphony)
                    channels[ev.channel].max_polyphony = count;
                break;
            }
            case RawKind::NoteOff:
                if (ev.note < 128)
                    active[ev.channel][ev.note] = false;
                break;
            case RawKind::ProgramChange:
                channels[ev.channel].gm_program = ev.program & 0x7F;
                break;
            case RawKind::Tempo:
                break;
        }
        return true;
    }

    bool stop_requested() const { return false; }
};

class TimelineSink {
public:
    TimelineSink(const ChannelInfo channels[16],
                 const std::vector<uint8_t>& voice_to_channel,
                 std::vector<TimelineEvent>& timeline,
                 uint32_t window_start = 0,
                 uint32_t window_end = UINT32_MAX,
                 size_t max_events = SIZE_MAX)
        : _timeline(timeline),
          _window_start(window_start),
          _window_end(window_end),
          _max_events(max_events) {
        memset(_channel_voice_count, 0, sizeof(_channel_voice_count));
        for (int ch = 0; ch < 16; ch++) {
            for (int n = 0; n < 128; n++)
                _note_to_voice[ch][n] = -1;
            _channel_program[ch] = channels[ch].gm_program & 0x7F;
        }
        for (int v = 0; v < MaxWtsVoices; v++) {
            _voice_note[v] = -1;
            _voice_start_tick[v] = 0;
        }
        for (size_t v = 0; v < voice_to_channel.size() && v < MaxWtsVoices; v++) {
            uint8_t ch = voice_to_channel[v];
            if (ch >= 16)
                continue;
            uint8_t& count = _channel_voice_count[ch];
            if (count < MaxWtsVoices)
                _channel_voices[ch][count++] = (uint8_t)v;
        }
    }

    bool on_event(const RawEvent& ev) {
        if (ev.tick >= _window_end) {
            _has_more = true;
            _stop_requested = true;
            return true;
        }

        if (ev.kind == RawKind::Tempo) {
            return push_event({ ev.tick, -1, -1, 0, 0, 0, 0, ev.bpm });
        }

        if (ev.channel >= 16)
            return true;

        if (ev.kind == RawKind::ProgramChange) {
            _channel_program[ev.channel] = ev.program & 0x7F;
            return true;
        }

        if (ev.note >= 128)
            return true;

        uint8_t voice_count = _channel_voice_count[ev.channel];
        if (voice_count == 0)
            return true;

        if (ev.kind == RawKind::NoteOn)
            return note_on(ev, voice_count);
        if (ev.kind == RawKind::NoteOff)
            return note_off(ev);
        return true;
    }

    bool has_more() const { return _has_more; }
    bool stop_requested() const { return _stop_requested; }

private:
    bool note_on(const RawEvent& ev, uint8_t voice_count) {
        int8_t existing_voice = _note_to_voice[ev.channel][ev.note];
        if (existing_voice >= 0) {
            if (!push_note_on(ev, existing_voice))
                return false;
            _voice_start_tick[existing_voice] = ev.tick;
            return true;
        }

        int assigned_voice = -1;
        for (uint8_t i = 0; i < voice_count; i++) {
            uint8_t v = _channel_voices[ev.channel][i];
            if (_voice_note[v] < 0) {
                assigned_voice = v;
                break;
            }
        }

        if (assigned_voice < 0) {
            uint32_t oldest_tick = UINT32_MAX;
            for (uint8_t i = 0; i < voice_count; i++) {
                uint8_t v = _channel_voices[ev.channel][i];
                if (_voice_start_tick[v] < oldest_tick) {
                    oldest_tick = _voice_start_tick[v];
                    assigned_voice = v;
                }
            }
            int16_t stolen_note = _voice_note[assigned_voice];
            if (stolen_note >= 0 && stolen_note < 128)
                _note_to_voice[ev.channel][stolen_note] = -1;
        }

        _note_to_voice[ev.channel][ev.note] = assigned_voice;
        _voice_note[assigned_voice] = ev.note;
        _voice_start_tick[assigned_voice] = ev.tick;
        return push_note_on(ev, assigned_voice);
    }

    bool note_off(const RawEvent& ev) {
        int8_t released_voice = _note_to_voice[ev.channel][ev.note];
        if (released_voice >= 0) {
            _note_to_voice[ev.channel][ev.note] = -1;
            _voice_note[released_voice] = -1;
            return push_event({
                ev.tick,
                (int8_t)released_voice,
                -1,
                0,
                0,
                0,
                0,
                0
            });
        }
        return true;
    }

    bool push_note_on(const RawEvent& ev, int voice) {
        uint8_t program = _channel_program[ev.channel] & 0x7F;
        return push_event({
            ev.tick,
            (int8_t)voice,
            ev.note,
            ev.velocity,
            hardware_instrument_for(ev.channel, program),
            ev.channel == 9 ? (uint8_t)128 : (uint8_t)0,
            ev.channel == 9 ? (uint8_t)0 : program,
            0
        });
    }

    bool push_event(const TimelineEvent& ev) {
        if (ev.tick < _window_start)
            return true;
        if (_timeline.size() >= _max_events)
            return false;
        _timeline.push_back(ev);
        return true;
    }

    std::vector<TimelineEvent>& _timeline;
    uint32_t _window_start;
    uint32_t _window_end;
    size_t _max_events;
    bool _has_more = false;
    bool _stop_requested = false;
    uint8_t _channel_voices[16][MaxWtsVoices] = {};
    uint8_t _channel_voice_count[16] = {};
    int8_t _note_to_voice[16][128];
    int16_t _voice_note[MaxWtsVoices];
    uint32_t _voice_start_tick[MaxWtsVoices];
    uint8_t _channel_program[16];
};

struct TimelineStreamContext {
    bool active = false;
    bool exhausted = false;
    uint16_t ppqn = 0;
    uint16_t track_count = 0;
    TrackCursor tracks[MaxStreamingTracks];
    uint8_t channel_voices[16][MaxWtsVoices] = {};
    uint8_t channel_voice_count[16] = {};
    int8_t note_to_voice[16][128];
    int16_t voice_note[MaxWtsVoices];
    uint32_t voice_start_tick[MaxWtsVoices];
    uint8_t channel_program[16];
};

static TimelineStreamContext g_timeline_stream;

void reset_timeline_stream_context() {
    memset(&g_timeline_stream, 0, sizeof(g_timeline_stream));
    for (int ch = 0; ch < 16; ch++) {
        for (int n = 0; n < 128; n++)
            g_timeline_stream.note_to_voice[ch][n] = -1;
    }
    for (int v = 0; v < MaxWtsVoices; v++)
        g_timeline_stream.voice_note[v] = -1;
}

bool init_timeline_stream_context(Input& input, const Song& plan,
                                  char* error, size_t error_size) {
    reset_timeline_stream_context();

    if (plan.ppqn == 0 || plan.voice_count == 0 ||
        plan.voice_count > MaxWtsVoices) {
        set_error(error, error_size, "midi playback plan is empty");
        return false;
    }

    InputReader reader(input);
    if (!init_streaming_tracks(reader, input.size(),
                               g_timeline_stream.ppqn,
                               g_timeline_stream.tracks,
                               g_timeline_stream.track_count,
                               error, error_size)) {
        reset_timeline_stream_context();
        return false;
    }

    if (g_timeline_stream.ppqn != plan.ppqn) {
        set_error(error, error_size, "midi changed between parse passes");
        reset_timeline_stream_context();
        return false;
    }

    std::vector<uint8_t> voices = stored_voice_channels(plan);
    for (int ch = 0; ch < 16; ch++)
        g_timeline_stream.channel_program[ch] =
            plan.channels[ch].gm_program & 0x7F;
    for (size_t v = 0; v < voices.size() && v < MaxWtsVoices; v++) {
        uint8_t ch = voices[v];
        if (ch >= 16)
            continue;
        uint8_t& count = g_timeline_stream.channel_voice_count[ch];
        if (count < MaxWtsVoices)
            g_timeline_stream.channel_voices[ch][count++] = (uint8_t)v;
    }

    for (uint16_t i = 0; i < g_timeline_stream.track_count; i++) {
        if (!read_next_streaming_event(input, g_timeline_stream.tracks[i],
                                       error, error_size)) {
            reset_timeline_stream_context();
            return false;
        }
    }

    g_timeline_stream.active = true;
    return true;
}

bool stream_push_event(const TimelineEvent& ev,
                       uint32_t window_start,
                       std::vector<TimelineEvent>& timeline,
                       size_t max_events,
                       char* error, size_t error_size) {
    if (ev.tick < window_start)
        return true;
    if (timeline.size() >= max_events) {
        set_error(error, error_size, "midi window event limit exceeded");
        return false;
    }
    timeline.push_back(ev);
    return true;
}

bool stream_timeline_event(const RawEvent& ev,
                           uint32_t window_start,
                           std::vector<TimelineEvent>& timeline,
                           size_t max_events,
                           char* error, size_t error_size) {
    if (ev.kind == RawKind::Tempo) {
        return stream_push_event(
            { ev.tick, -1, -1, 0, 0, 0, 0, ev.bpm },
            window_start, timeline, max_events, error, error_size);
    }

    if (ev.channel >= 16)
        return true;

    if (ev.kind == RawKind::ProgramChange) {
        g_timeline_stream.channel_program[ev.channel] = ev.program & 0x7F;
        return true;
    }

    if (ev.note >= 128)
        return true;

    uint8_t voice_count = g_timeline_stream.channel_voice_count[ev.channel];
    if (voice_count == 0)
        return true;

    if (ev.kind == RawKind::NoteOn) {
        int8_t existing_voice =
            g_timeline_stream.note_to_voice[ev.channel][ev.note];
        if (existing_voice >= 0) {
            g_timeline_stream.voice_start_tick[existing_voice] = ev.tick;
            return stream_push_event({
                ev.tick,
                existing_voice,
                ev.note,
                ev.velocity,
                hardware_instrument_for(ev.channel,
                    g_timeline_stream.channel_program[ev.channel]),
                ev.channel == 9 ? (uint8_t)128 : (uint8_t)0,
                ev.channel == 9
                    ? (uint8_t)0
                    : g_timeline_stream.channel_program[ev.channel],
                0
            }, window_start, timeline, max_events, error, error_size);
        }

        int assigned_voice = -1;
        for (uint8_t i = 0; i < voice_count; i++) {
            uint8_t v = g_timeline_stream.channel_voices[ev.channel][i];
            if (g_timeline_stream.voice_note[v] < 0) {
                assigned_voice = v;
                break;
            }
        }

        if (assigned_voice < 0) {
            uint32_t oldest_tick = UINT32_MAX;
            for (uint8_t i = 0; i < voice_count; i++) {
                uint8_t v = g_timeline_stream.channel_voices[ev.channel][i];
                if (g_timeline_stream.voice_start_tick[v] < oldest_tick) {
                    oldest_tick = g_timeline_stream.voice_start_tick[v];
                    assigned_voice = v;
                }
            }
            int16_t stolen_note =
                g_timeline_stream.voice_note[assigned_voice];
            if (stolen_note >= 0 && stolen_note < 128)
                g_timeline_stream.note_to_voice[ev.channel][stolen_note] = -1;
        }

        g_timeline_stream.note_to_voice[ev.channel][ev.note] =
            assigned_voice;
        g_timeline_stream.voice_note[assigned_voice] = ev.note;
        g_timeline_stream.voice_start_tick[assigned_voice] = ev.tick;

        return stream_push_event({
            ev.tick,
            (int8_t)assigned_voice,
            ev.note,
            ev.velocity,
            hardware_instrument_for(ev.channel,
                g_timeline_stream.channel_program[ev.channel]),
            ev.channel == 9 ? (uint8_t)128 : (uint8_t)0,
            ev.channel == 9
                ? (uint8_t)0
                : g_timeline_stream.channel_program[ev.channel],
            0
        }, window_start, timeline, max_events, error, error_size);
    }

    if (ev.kind == RawKind::NoteOff) {
        int8_t released_voice =
            g_timeline_stream.note_to_voice[ev.channel][ev.note];
        if (released_voice >= 0) {
            g_timeline_stream.note_to_voice[ev.channel][ev.note] = -1;
            g_timeline_stream.voice_note[released_voice] = -1;
            return stream_push_event({
                ev.tick,
                released_voice,
                -1,
                0,
                0,
                0,
                0,
                0
            }, window_start, timeline, max_events, error, error_size);
        }
    }

    return true;
}

bool build_song_from_raw(uint16_t ppqn, std::vector<RawEvent>& raw, Song& out,
                         char* error, size_t error_size) {
    std::sort(raw.begin(), raw.end(), [](const RawEvent& a, const RawEvent& b) {
        if (a.tick != b.tick) return a.tick < b.tick;
        return raw_sort_priority(a.kind) < raw_sort_priority(b.kind);
    });

    analyze_channels(raw, out.channels);
    std::vector<uint8_t> voices = select_channels(out.channels, MaxWtsVoices);
    if (voices.empty()) {
        set_error(error, error_size, "midi file contains no routed notes");
        return false;
    }

    out.ppqn = ppqn;
    out.voice_count = (uint8_t)voices.size();
    store_voice_channels(voices, out);
    build_timeline(raw, out.channels, voices, out.timeline);
    return !out.timeline.empty();
}

} // namespace

uint8_t hardware_instrument_for(uint8_t channel, uint8_t gm_program) {
    if (channel == 9) return 6;          // Drums: bright/noisy resident wave.
    if (gm_program <= 31) return 2;      // Piano, organ, guitar.
    if (gm_program <= 39) return 3;      // Bass.
    if (gm_program <= 55) return 1;      // Strings.
    if (gm_program <= 63) return 6;      // Brass.
    if (gm_program <= 71) return 5;      // Reed/sax.
    if (gm_program <= 79) return 0;      // Pipe/flute.
    if (gm_program <= 95) return 7;      // Leads/pads.
    return 4;                            // Everything else.
}

bool build_song(const uint8_t* data, size_t size, Song& out,
                char* error, size_t error_size) {
    out = {};
    if (!data || size < 14) {
        set_error(error, error_size, "empty midi file");
        return false;
    }

    uint16_t ppqn = 0;
    std::vector<RawEvent> raw;
    Reader reader(data, size);
    RawCollectSink sink{ raw };
    if (!parse_raw_events(reader, size, ppqn, sink, error, error_size))
        return false;

    return build_song_from_raw(ppqn, raw, out, error, error_size);
}

bool build_song(Input& input, Song& out, char* error, size_t error_size) {
    if (!build_song_plan(input, out, error, error_size))
        return false;

    bool has_more = false;
    return build_song_window(input, out, 0, UINT32_MAX, out.timeline,
                             has_more, error, error_size, SIZE_MAX);
}

bool build_song_plan(Input& input, Song& out, char* error, size_t error_size) {
    out = {};
    if (input.size() < 14) {
        set_error(error, error_size, "empty midi file");
        return false;
    }

    uint16_t ppqn = 0;
    AnalyzeSink analyze;
    if (!parse_streaming_events(input, ppqn, analyze, error, error_size)) {
        return false;
    }

    for (int i = 0; i < 16; i++)
        out.channels[i] = analyze.channels[i];

    std::vector<uint8_t> voices = select_channels(out.channels, MaxWtsVoices);
    if (voices.empty()) {
        set_error(error, error_size, "midi file contains no routed notes");
        return false;
    }

    out.ppqn = ppqn;
    out.voice_count = (uint8_t)voices.size();
    store_voice_channels(voices, out);

    return true;
}

bool build_song_window(Input& input, const Song& plan,
                       uint32_t start_tick, uint32_t end_tick,
                       std::vector<TimelineEvent>& timeline,
                       bool& has_more,
                       char* error, size_t error_size,
                       size_t max_events) {
    has_more = false;
    timeline.clear();

    if (plan.ppqn == 0 || plan.voice_count == 0 ||
        plan.voice_count > MaxWtsVoices) {
        set_error(error, error_size, "midi playback plan is empty");
        return false;
    }
    if (end_tick <= start_tick) {
        set_error(error, error_size, "bad midi playback window");
        return false;
    }

    std::vector<uint8_t> voices = stored_voice_channels(plan);
    uint16_t window_ppqn = 0;
    if (max_events != SIZE_MAX && timeline.capacity() < 256)
        timeline.reserve(256);
    TimelineSink timeline_sink(plan.channels, voices, timeline,
                               start_tick, end_tick, max_events);

    if (!parse_streaming_events(input, window_ppqn, timeline_sink,
                                error, error_size)) {
        return false;
    }

    if (window_ppqn != plan.ppqn) {
        set_error(error, error_size, "midi changed between parse passes");
        return false;
    }

    has_more = timeline_sink.has_more();
    std::sort(timeline.begin(), timeline.end(), [](const TimelineEvent& a,
                                                   const TimelineEvent& b) {
        if (a.tick != b.tick) return a.tick < b.tick;
        if (a.voice != b.voice) return a.voice < b.voice;
        return a.note < b.note;
    });
    if (timeline.empty() && !has_more) {
        set_error(error, error_size, "midi file contains no routed notes");
        return false;
    }
    return true;
}

void reset_song_window_stream() {
    reset_timeline_stream_context();
}

bool begin_song_window_stream(Input& input, const Song& plan,
                              char* error, size_t error_size) {
    return init_timeline_stream_context(input, plan, error, error_size);
}

bool build_song_stream_window(Input& input,
                              uint32_t start_tick, uint32_t end_tick,
                              std::vector<TimelineEvent>& timeline,
                              bool& has_more,
                              char* error, size_t error_size,
                              size_t max_events) {
    timeline.clear();
    has_more = false;

    if (!g_timeline_stream.active) {
        set_error(error, error_size, "midi stream is not initialized");
        return false;
    }
    if (end_tick <= start_tick) {
        set_error(error, error_size, "bad midi playback window");
        return false;
    }
    if (g_timeline_stream.exhausted)
        return true;

    uint16_t poll = 0;
    for (;;) {
        int best = -1;
        for (uint16_t i = 0; i < g_timeline_stream.track_count; i++) {
            TrackCursor& track = g_timeline_stream.tracks[i];
            if (!track.pending)
                continue;
            if (best < 0 ||
                track.event.tick <
                    g_timeline_stream.tracks[best].event.tick ||
                (track.event.tick ==
                     g_timeline_stream.tracks[best].event.tick &&
                 raw_sort_priority(track.event.kind) <
                    raw_sort_priority(
                        g_timeline_stream.tracks[best].event.kind)) ||
                (track.event.tick ==
                     g_timeline_stream.tracks[best].event.tick &&
                 raw_sort_priority(track.event.kind) ==
                    raw_sort_priority(
                        g_timeline_stream.tracks[best].event.kind) &&
                 track.order < g_timeline_stream.tracks[best].order)) {
                best = i;
            }
        }

        if (best < 0) {
            g_timeline_stream.exhausted = true;
            return true;
        }

        TrackCursor& track = g_timeline_stream.tracks[best];
        if (track.event.tick >= end_tick) {
            has_more = true;
            return true;
        }

        if ((++poll & 0x3F) == 0)
            midi_yield();

        if (!stream_timeline_event(track.event, start_tick, timeline,
                                   max_events, error, error_size)) {
            return false;
        }
        if (!read_next_streaming_event(input, track, error, error_size))
            return false;
    }
}

} // namespace nova_midi

#endif
