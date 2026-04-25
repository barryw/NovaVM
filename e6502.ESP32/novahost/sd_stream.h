// SdStream — IStream backed by an Arduino fs::File (SD_MMC or SD).
// Bridges the portable ndi::IStream API to the ESP32's filesystem so
// NdiImage can mount any .ndi file on the SD card without knowing
// anything about Arduino.
//
// Usage (on ESP32):
//   File f = SD_MMC.open("/games/spaceinv.ndi", FILE_READ);
//   SdStream s(f);
//   ndi::NdiImage img;
//   img.open(&s);
//
// The Arduino File API is byte-stream-only, but it does support seek()
// and size(). uint64_t offsets > 4 GB are unsupported by the underlying
// FAT32 spec anyway (4 GB file limit), so we cast to size_t safely.

#ifndef NOVAHOST_SD_STREAM_H
#define NOVAHOST_SD_STREAM_H

#include <FS.h>
#include "ndi_image.h"

class SdStream : public ndi::IStream {
public:
    explicit SdStream(File& f) : _f(f) {}

    bool seek(uint64_t offset) override {
        // Arduino File::seek takes uint32_t (FAT32 4 GB limit). Truncate
        // explicitly so 64-bit offsets > 4 GB fail predictably.
        if (offset > 0xFFFFFFFFULL) return false;
        return _f.seek((uint32_t)offset);
    }

    int read(uint8_t* buf, size_t len) override {
        return _f.read(buf, len);
    }

    int write(const uint8_t* buf, size_t len) override {
        return (int)_f.write(buf, len);
    }

    void flush() override {
        _f.flush();
    }

private:
    File& _f;
};

#endif
