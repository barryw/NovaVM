#ifndef NOVAHOST_BOOT_CONFIG_PARSER_H
#define NOVAHOST_BOOT_CONFIG_PARSER_H

#include <stddef.h>

static constexpr size_t BOOT_CONFIG_MAX_BYTES = 4096;
static constexpr const char* BOOT_SPLASH_DEFAULT_ASSET = "/assets/boot/novavm_logo.nvg";
static constexpr unsigned BOOT_SPLASH_DEFAULT_FADE_IN_MS = 1000;
static constexpr unsigned BOOT_SPLASH_DEFAULT_HOLD_MS = 3000;
static constexpr unsigned BOOT_SPLASH_DEFAULT_FADE_OUT_MS = 1000;
static constexpr unsigned BOOT_SPLASH_DEFAULT_BACKGROUND = 0;
static constexpr unsigned BOOT_SPLASH_DEFAULT_BORDER = 11;

struct BootRuntimeConfig {
    char runtime[32];
    char romPath[128];
    char extRomPath[128];
    bool valid;
};

struct BootSplashConfig {
    char assetPath[128];
    unsigned fadeInMs;
    unsigned holdMs;
    unsigned fadeOutMs;
    unsigned background;
    unsigned border;
    bool enabled;
    bool configured;
};

void initBootSplashConfigDefaults(BootSplashConfig& out);

bool parseBootRuntimeConfigText(const char* text, size_t len,
                                BootRuntimeConfig& out);
bool parseBootSplashConfigText(const char* text, size_t len,
                               BootSplashConfig& out);

#endif
