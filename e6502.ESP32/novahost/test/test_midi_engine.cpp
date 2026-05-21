#include "../midi_engine.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

static int g_pass = 0;
static int g_fail = 0;

class MemoryInput : public nova_midi::Input {
public:
    explicit MemoryInput(const std::vector<uint8_t>& data) : _data(data) {}

    size_t pos() const override { return _pos; }
    size_t size() const override { return _data.size(); }

    bool seek(size_t pos) override {
        if (pos > _data.size())
            return false;
        _pos = pos;
        return true;
    }

    bool read(uint8_t* dest, size_t len) override {
        if (_pos + len > _data.size())
            return false;
        memcpy(dest, _data.data() + _pos, len);
        _pos += len;
        return true;
    }

private:
    const std::vector<uint8_t>& _data;
    size_t _pos = 0;
};

static void check(const char* name, bool ok) {
    if (ok) {
        printf("  PASS  %s\n", name);
        g_pass++;
    } else {
        printf("  FAIL  %s\n", name);
        g_fail++;
    }
}

static void put_be16(std::vector<uint8_t>& v, uint16_t x) {
    v.push_back((uint8_t)(x >> 8));
    v.push_back((uint8_t)x);
}

static void put_be32(std::vector<uint8_t>& v, uint32_t x) {
    v.push_back((uint8_t)(x >> 24));
    v.push_back((uint8_t)(x >> 16));
    v.push_back((uint8_t)(x >> 8));
    v.push_back((uint8_t)x);
}

static void put_var(std::vector<uint8_t>& v, uint32_t x) {
    uint8_t buf[4];
    int n = 0;
    buf[n++] = (uint8_t)(x & 0x7F);
    while ((x >>= 7) != 0)
        buf[n++] = (uint8_t)((x & 0x7F) | 0x80);
    while (n > 0)
        v.push_back(buf[--n]);
}

static std::vector<uint8_t> wrap_track(const std::vector<uint8_t>& track,
                                       uint16_t format = 0,
                                       uint16_t tracks = 1,
                                       uint16_t ppqn = 480) {
    std::vector<uint8_t> out;
    out.insert(out.end(), { 'M', 'T', 'h', 'd' });
    put_be32(out, 6);
    put_be16(out, format);
    put_be16(out, tracks);
    put_be16(out, ppqn);
    out.insert(out.end(), { 'M', 'T', 'r', 'k' });
    put_be32(out, (uint32_t)track.size());
    out.insert(out.end(), track.begin(), track.end());
    return out;
}

static std::vector<uint8_t> simple_song() {
    std::vector<uint8_t> trk;
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xC0, 0x00 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0x90, 60, 100 });
    put_var(trk, 480);
    trk.insert(trk.end(), { 0x80, 60, 0 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xFF, 0x2F, 0x00 });
    return wrap_track(trk);
}

static std::vector<uint8_t> running_status_song() {
    std::vector<uint8_t> trk;
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xC2, 24 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0x92, 60, 100 });
    put_var(trk, 120);
    trk.insert(trk.end(), { 64, 90 });
    put_var(trk, 120);
    trk.insert(trk.end(), { 0x82, 60, 0 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 64, 0 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xFF, 0x2F, 0x00 });
    return wrap_track(trk);
}

static void test_simple_song() {
    printf("\nTest: simple MIDPLAY timeline\n");
    auto bytes = simple_song();
    nova_midi::Song song;
    char err[96] = {};
    bool ok = nova_midi::build_song(bytes.data(), bytes.size(),
                                    song, err, sizeof(err));
    check("build_song succeeds", ok);
    check("ppqn is preserved", song.ppqn == 480);
    check("one voice selected", song.voice_count == 1);
    check("channel 0 note counted", song.channels[0].note_count == 1);

    bool saw_tempo = false;
    bool saw_on = false;
    bool saw_off = false;
    for (const auto& ev : song.timeline) {
        if (ev.voice == -1 && ev.tick == 0 && ev.bpm == 120)
            saw_tempo = true;
        if (ev.voice == 0 && ev.tick == 0 && ev.note == 60 &&
            ev.velocity == 100 && ev.instrument == 2 &&
            ev.bank == 0 && ev.program == 0)
            saw_on = true;
        if (ev.voice == 0 && ev.tick == 480 && ev.note < 0)
            saw_off = true;
    }
    check("tempo event retained", saw_tempo);
    check("note-on routes to WTS voice 0", saw_on);
    check("note-off releases the same voice", saw_off);
}

static void test_running_status() {
    printf("\nTest: running status + channel analysis\n");
    auto bytes = running_status_song();
    nova_midi::Song song;
    char err[96] = {};
    bool ok = nova_midi::build_song(bytes.data(), bytes.size(),
                                    song, err, sizeof(err));
    check("build_song succeeds", ok);
    check("channel 2 has two note-ons", song.channels[2].note_count == 2);
    check("channel 2 max polyphony is two", song.channels[2].max_polyphony == 2);
    check("guitar program maps to resident square-ish instrument",
          nova_midi::hardware_instrument_for(2, song.channels[2].gm_program) == 2);
}

static void test_program_change_uses_event_time_program() {
    printf("\nTest: note-on carries current GM program\n");
    std::vector<uint8_t> trk;
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xC0, 56 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0x90, 60, 100 });
    put_var(trk, 120);
    trk.insert(trk.end(), { 0x80, 60, 0 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xC0, 72 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0x90, 67, 90 });
    put_var(trk, 120);
    trk.insert(trk.end(), { 0x80, 67, 0 });
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xFF, 0x2F, 0x00 });

    auto bytes = wrap_track(trk);
    nova_midi::Song song;
    char err[96] = {};
    bool ok = nova_midi::build_song(bytes.data(), bytes.size(),
                                    song, err, sizeof(err));
    check("build_song succeeds", ok);

    bool saw_brass = false;
    bool saw_flute = false;
    for (const auto& ev : song.timeline) {
        if (ev.voice == 0 && ev.note == 60 && ev.program == 56 && ev.instrument == 6)
            saw_brass = true;
        if (ev.voice == 0 && ev.note == 67 && ev.program == 72 && ev.instrument == 0)
            saw_flute = true;
    }
    check("first note uses brass program", saw_brass);
    check("second note uses later flute program", saw_flute);
}

static void test_rejects_smpte_timing() {
    printf("\nTest: reject SMPTE timing\n");
    std::vector<uint8_t> trk;
    put_var(trk, 0);
    trk.insert(trk.end(), { 0xFF, 0x2F, 0x00 });
    auto bytes = wrap_track(trk, 0, 1, 0xE728);
    nova_midi::Song song;
    char err[96] = {};
    bool ok = nova_midi::build_song(bytes.data(), bytes.size(),
                                    song, err, sizeof(err));
    check("build_song fails", !ok);
    check("error mentions timing", strstr(err, "timing") != nullptr);
}

static void test_stars_and_stripes_if_available() {
    printf("\nTest: Sousa Stars and Stripes fixture when available\n");
    const char* path = "/Users/barry/e6502-programs/Sousa_StarsAndStripesForever.mid";
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("  SKIP  %s not present\n", path);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> bytes((size_t)size);
    size_t got = fread(bytes.data(), 1, bytes.size(), f);
    fclose(f);

    nova_midi::Song song;
    char err[96] = {};
    bool ok = got == bytes.size() &&
        nova_midi::build_song(bytes.data(), bytes.size(), song, err, sizeof(err));
    check("build_song succeeds", ok);
    check("ppqn is 480", song.ppqn == 480);
    check("routes all 8 WTS voices", song.voice_count == nova_midi::MaxWtsVoices);
    check("produces a non-empty hardware timeline", song.timeline.size() > 100);
    if (ok) {
        printf("  INFO  events=%u voices=%u\n",
               (unsigned)song.timeline.size(), (unsigned)song.voice_count);

        MemoryInput input(bytes);
        nova_midi::Song plan;
        char plan_err[96] = {};
        bool plan_ok = nova_midi::build_song_plan(input, plan, plan_err,
                                                  sizeof(plan_err));
        check("streaming plan succeeds", plan_ok);

        MemoryInput full_stream_input(bytes);
        nova_midi::Song full_stream;
        bool full_stream_ok = nova_midi::build_song(full_stream_input,
                                                    full_stream, plan_err,
                                                    sizeof(plan_err));
        check("streaming full build succeeds", full_stream_ok);

        std::vector<nova_midi::TimelineEvent> streamed;
        uint32_t start_tick = 0;
        bool has_more = true;
        while (plan_ok && has_more) {
            std::vector<nova_midi::TimelineEvent> window;
            MemoryInput window_input(bytes);
            has_more = false;
            bool window_ok = nova_midi::build_song_window(
                window_input, plan, start_tick, start_tick + 15360,
                window, has_more, plan_err, sizeof(plan_err));
            check("streaming window succeeds", window_ok);
            if (!window_ok)
                break;
            streamed.insert(streamed.end(), window.begin(), window.end());
            start_tick += 15360;
        }
        printf("  INFO  streamed_events=%u full_stream_events=%u legacy_events=%u\n",
               (unsigned)streamed.size(),
               (unsigned)full_stream.timeline.size(),
               (unsigned)song.timeline.size());
        check("streaming windows preserve event count",
              full_stream_ok && streamed.size() == full_stream.timeline.size());

        MemoryInput stateful_init_input(bytes);
        std::vector<nova_midi::TimelineEvent> stateful;
        bool stateful_ok = nova_midi::begin_song_window_stream(
            stateful_init_input, plan, plan_err, sizeof(plan_err));
        check("stateful stream init succeeds", stateful_ok);
        start_tick = 0;
        has_more = true;
        while (stateful_ok && has_more) {
            std::vector<nova_midi::TimelineEvent> window;
            MemoryInput window_input(bytes);
            has_more = false;
            bool window_ok = nova_midi::build_song_stream_window(
                window_input, start_tick, start_tick + 7680,
                window, has_more, plan_err, sizeof(plan_err), 512);
            check("stateful streaming window succeeds", window_ok);
            if (!window_ok)
                break;
            stateful.insert(stateful.end(), window.begin(), window.end());
            start_tick += 7680;
        }
        nova_midi::reset_song_window_stream();
        printf("  INFO  stateful_events=%u\n", (unsigned)stateful.size());
        check("stateful streaming preserves event count",
              full_stream_ok && stateful.size() == full_stream.timeline.size());
    }
}

int main() {
    printf("=== midi_engine host tests ===\n");
    test_simple_song();
    test_running_status();
    test_program_change_uses_event_time_program();
    test_rejects_smpte_timing();
    test_stars_and_stripes_if_available();
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
