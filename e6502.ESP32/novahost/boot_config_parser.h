#ifndef NOVAHOST_BOOT_CONFIG_PARSER_H
#define NOVAHOST_BOOT_CONFIG_PARSER_H

#include <stddef.h>

static constexpr size_t BOOT_CONFIG_MAX_BYTES = 4096;

struct BootRuntimeConfig {
    char runtime[32];
    char romPath[128];
    char extRomPath[128];
    bool valid;
};

bool parseBootRuntimeConfigText(const char* text, size_t len,
                                BootRuntimeConfig& out);

#endif
