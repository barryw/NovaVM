/* nbootcfg.c — /config/boot.json access (port of the ULX3S novahost handling).
 * ArduinoJson -> cJSON, SD/File -> POSIX stdio; schema + semantics unchanged. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "novavm.h"        /* NOVA_FS_ROOT */
#include "nbootcfg.h"

#define BOOT_JSON      NOVA_FS_ROOT "/config/boot.json"
#define BOOT_JSON_TMP  NOVA_FS_ROOT "/config/boot.json.tmp"

/* read_boot_config_text(): whole file -> malloc'd NUL-terminated buffer, or NULL. */
static char *read_boot_config_text(void) {
    FILE *f = fopen(BOOT_JSON, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0 || n > 1 << 20) { fclose(f); return NULL; }
    char *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[rd] = 0;
    return buf;
}

/* write_boot_config(): serialize -> boot.json.tmp -> atomic rename (ESP32 parity). */
static int write_boot_config(cJSON *doc) {
    char *txt = cJSON_Print(doc);
    if (!txt) return 0;
    mkdir(NOVA_FS_ROOT "/config", 0775);
    FILE *f = fopen(BOOT_JSON_TMP, "wb");
    if (!f) { free(txt); return 0; }
    fputs(txt, f);
    fputc('\n', f);
    fclose(f);
    free(txt);
    if (rename(BOOT_JSON_TMP, BOOT_JSON) != 0) { unlink(BOOT_JSON_TMP); return 0; }
    return 1;
}

/* Integer preference under a named group object in boot.json (e.g.
 * group="input" key="mouseGain", group="audio" key="masterVolume"). This is how
 * the OSD/mouse persist tuning — one config file, not stray *.cfg siblings. */
int bootcfg_int_get(const char *group, const char *key, int def) {
    char *txt = read_boot_config_text();
    if (!txt) return def;
    cJSON *doc = cJSON_Parse(txt);
    free(txt);
    if (!doc) return def;
    int val = def;
    cJSON *g = cJSON_GetObjectItemCaseSensitive(doc, group);
    cJSON *v = g ? cJSON_GetObjectItemCaseSensitive(g, key) : NULL;
    if (cJSON_IsNumber(v)) val = (int)v->valuedouble;
    cJSON_Delete(doc);
    return val;
}

void bootcfg_int_set(const char *group, const char *key, int value) {
    char *txt = read_boot_config_text();
    cJSON *doc = txt ? cJSON_Parse(txt) : NULL;
    free(txt);
    if (!doc) doc = cJSON_CreateObject();
    cJSON *g = cJSON_GetObjectItemCaseSensitive(doc, group);
    if (!g) { g = cJSON_CreateObject(); cJSON_AddItemToObject(doc, group, g); }
    cJSON *v = cJSON_GetObjectItemCaseSensitive(g, key);
    if (v) cJSON_SetNumberValue(v, value);
    else cJSON_AddNumberToObject(g, key, value);
    write_boot_config(doc);
    cJSON_Delete(doc);
}

int bootcfg_mount_get(const char *prefix, char *out, size_t out_len) {
    if (!prefix || !out || out_len == 0) return 0;
    out[0] = 0;
    char *txt = read_boot_config_text();
    if (!txt) return 0;
    cJSON *doc = cJSON_Parse(txt);
    free(txt);
    if (!doc) return 0;
    int ok = 0;
    cJSON *mounts = cJSON_GetObjectItemCaseSensitive(doc, "mounts");
    cJSON *m = mounts ? cJSON_GetObjectItemCaseSensitive(mounts, prefix) : NULL;
    if (cJSON_IsString(m) && m->valuestring && m->valuestring[0]) {
        strncpy(out, m->valuestring, out_len - 1);
        out[out_len - 1] = 0;
        ok = 1;
    }
    cJSON_Delete(doc);
    return ok;
}

int bootcfg_mount_set(const char *prefix, const char *sd_path) {
    if (!prefix) return 0;
    /* read-modify-write: preserve every other key in boot.json (ESP32 behaviour). */
    char *txt = read_boot_config_text();
    cJSON *doc = txt ? cJSON_Parse(txt) : NULL;
    if (txt) free(txt);
    if (!doc) doc = cJSON_CreateObject();
    cJSON *mounts = cJSON_GetObjectItemCaseSensitive(doc, "mounts");
    if (!mounts) {
        mounts = cJSON_CreateObject();
        cJSON_AddItemToObject(doc, "mounts", mounts);
    }
    cJSON_DeleteItemFromObjectCaseSensitive(mounts, prefix);
    cJSON_AddStringToObject(mounts, prefix, sd_path ? sd_path : "");
    int ok = write_boot_config(doc);
    cJSON_Delete(doc);
    return ok;
}

void bootcfg_splash(BootSplashConfig *out) {
    /* initBootSplashConfigDefaults (boot_config_parser.h constants). */
    memset(out, 0, sizeof *out);
    strncpy(out->assetPath, "/assets/boot/novavm_logo.nvg", sizeof out->assetPath - 1);
    out->fadeInMs = 1000;
    out->holdMs = 3000;
    out->fadeOutMs = 1000;
    out->background = 0;
    out->border = 11;
    out->enabled = 1;
    out->configured = 0;

    char *txt = read_boot_config_text();
    if (!txt) return;
    cJSON *doc = cJSON_Parse(txt);
    free(txt);
    if (!doc) return;
    cJSON *vm = cJSON_GetObjectItemCaseSensitive(doc, "vm");
    cJSON *sp = vm ? cJSON_GetObjectItemCaseSensitive(vm, "bootSplash") : NULL;
    if (sp) {
        out->configured = 1;
        cJSON *v;
        if (cJSON_IsBool((v = cJSON_GetObjectItemCaseSensitive(sp, "enabled")))) out->enabled = cJSON_IsTrue(v);
        if (cJSON_IsString((v = cJSON_GetObjectItemCaseSensitive(sp, "asset"))) && v->valuestring) {
            strncpy(out->assetPath, v->valuestring, sizeof out->assetPath - 1);
            out->assetPath[sizeof out->assetPath - 1] = 0;
        }
        if (cJSON_IsNumber((v = cJSON_GetObjectItemCaseSensitive(sp, "fadeInMs"))))  out->fadeInMs  = (unsigned)v->valuedouble;
        if (cJSON_IsNumber((v = cJSON_GetObjectItemCaseSensitive(sp, "holdMs"))))    out->holdMs    = (unsigned)v->valuedouble;
        if (cJSON_IsNumber((v = cJSON_GetObjectItemCaseSensitive(sp, "fadeOutMs")))) out->fadeOutMs = (unsigned)v->valuedouble;
        if (cJSON_IsNumber((v = cJSON_GetObjectItemCaseSensitive(sp, "background")))) out->background = (unsigned)v->valuedouble;
        if (cJSON_IsNumber((v = cJSON_GetObjectItemCaseSensitive(sp, "border"))))    out->border    = (unsigned)v->valuedouble;
    }
    cJSON_Delete(doc);
}
