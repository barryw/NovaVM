/* nbootcfg.h — /config/boot.json access for the NovaVM Linux host.
 *
 * Direct port of the ULX3S (ESP32 novahost) boot-config handling:
 *   - management_server.cpp: load_drive_mount_config / persist_drive_mount_config
 *     / write_boot_config / read_boot_config_text  (the drive mount table)
 *   - boot_config_parser.cpp + novahost.ino readBootSplashConfig  (vm.bootSplash)
 *
 * The ONLY changes vs. the ULX3S are the forced hardware/platform boundaries:
 *   ArduinoJson (JsonDocument/deserializeJson/serializeJsonPretty) -> cJSON
 *   Arduino SD/File                                                -> POSIX stdio
 * The boot.json schema, keys, defaults and semantics are identical to the ULX3S,
 * so `nova drive mount`, the web-admin and the on-device config all interoperate. */
#ifndef NBOOTCFG_H
#define NBOOTCFG_H
#include <stddef.h>

/* vm.bootSplash — mirrors ESP32 boot_config_parser.h BootSplashConfig + defaults. */
typedef struct {
    char     assetPath[128];
    unsigned fadeInMs;
    unsigned holdMs;
    unsigned fadeOutMs;
    unsigned background;
    unsigned border;
    int      enabled;
    int      configured;
} BootSplashConfig;

/* mounts.<prefix> — prefix is "fd0".."fd3","hd0","hd1". */
int  bootcfg_mount_get(const char *prefix, char *out, size_t out_len); /* 1 if a non-empty path */
int  bootcfg_mount_set(const char *prefix, const char *sd_path);       /* "" unmounts; 1 on success */

/* vm.bootSplash with ULX3S defaults overlaid by boot.json (initBootSplashConfigDefaults). */
void bootcfg_splash(BootSplashConfig *out);

#endif /* NBOOTCFG_H */
