#include "../boot_config_parser.h"

#include <stdio.h>
#include <string.h>

static int g_pass = 0;
static int g_fail = 0;

static void check(const char* name, bool ok) {
    if (ok) {
        printf("  PASS  %s\n", name);
        g_pass++;
    } else {
        printf("  FAIL  %s\n", name);
        g_fail++;
    }
}

static void check_eq_str(const char* name, const char* got,
                         const char* want) {
    if (strcmp(got, want) == 0) {
        printf("  PASS  %s (=\"%s\")\n", name, got);
        g_pass++;
    } else {
        printf("  FAIL  %s (got \"%s\", want \"%s\")\n",
               name, got, want);
        g_fail++;
    }
}

static void check_eq_uint(const char* name, unsigned got, unsigned want) {
    if (got == want) {
        printf("  PASS  %s (=%u)\n", name, got);
        g_pass++;
    } else {
        printf("  FAIL  %s (got %u, want %u)\n", name, got, want);
        g_fail++;
    }
}

static void test_default_runtime_value_does_not_select_first_rom() {
    printf("\nTest: defaultRuntime value does not shadow language key\n");
    const char* json =
        "{\"vm\":{\"defaultRuntime\":\"novalogo\"},"
        "\"languages\":{"
        "\"novabasic\":{\"rom\":\"/roms/novabasic.bin\","
        "\"extensionRom\":\"/roms/extension.bin\"},"
        "\"novalogo\":{\"rom\":\"/roms/novalogo.bin\"}}}";

    BootRuntimeConfig cfg;
    bool ok = parseBootRuntimeConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check("runtime config is valid", cfg.valid);
    check_eq_str("active runtime", cfg.runtime, "novalogo");
    check_eq_str("active ROM path", cfg.romPath, "/roms/novalogo.bin");
    check_eq_str("active extension path", cfg.extRomPath, "");
}

static void test_default_runtime_falls_back_to_novabasic() {
    printf("\nTest: missing defaultRuntime falls back to novabasic\n");
    const char* json =
        "{\"languages\":{\"novabasic\":{\"rom\":\"/roms/novabasic.bin\","
        "\"extensionRom\":\"/roms/extension.bin\"}}}";

    BootRuntimeConfig cfg;
    bool ok = parseBootRuntimeConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check_eq_str("default runtime", cfg.runtime, "novabasic");
    check_eq_str("default ROM path", cfg.romPath, "/roms/novabasic.bin");
    check_eq_str("default extension path", cfg.extRomPath,
                 "/roms/extension.bin");
}

static void test_default_runtime_is_scoped_to_vm_object() {
    printf("\nTest: defaultRuntime is scoped to vm object\n");
    const char* json =
        "{\"defaultRuntime\":\"novalogo\","
        "\"languages\":{"
        "\"novabasic\":{\"rom\":\"/roms/novabasic.bin\"},"
        "\"novalogo\":{\"rom\":\"/roms/novalogo.bin\"}}}";

    BootRuntimeConfig cfg;
    bool ok = parseBootRuntimeConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check_eq_str("runtime falls back to novabasic", cfg.runtime, "novabasic");
    check_eq_str("selected ROM path", cfg.romPath, "/roms/novabasic.bin");
}

static void test_runtime_search_is_bounded_to_selected_object() {
    printf("\nTest: runtime paths are read only from selected language object\n");
    const char* json =
        "{"
        "\"languages\":{"
        "\"novalogo\":{\"rom\":\"/roms/novalogo.bin\"},"
        "\"other\":{\"rom\":\"/roms/wrong.bin\","
        "\"extensionRom\":\"/roms/wrong_ext.bin\"}"
        "},"
        "\"vm\":{\"defaultRuntime\":\"novalogo\"}"
        "}";

    BootRuntimeConfig cfg;
    bool ok = parseBootRuntimeConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check_eq_str("selected ROM path", cfg.romPath, "/roms/novalogo.bin");
    check_eq_str("missing selected extension stays empty", cfg.extRomPath, "");
}

static void test_missing_active_runtime_is_invalid() {
    printf("\nTest: missing active runtime is invalid\n");
    const char* json =
        "{\"vm\":{\"defaultRuntime\":\"novalogo\"},"
        "\"languages\":{\"novabasic\":{\"rom\":\"/roms/novabasic.bin\"}}}";

    BootRuntimeConfig cfg;
    bool ok = parseBootRuntimeConfigText(json, strlen(json), cfg);
    check("parse fails", !ok);
    check("runtime config is not valid", !cfg.valid);
    check_eq_str("requested runtime preserved for diagnostics",
                 cfg.runtime, "novalogo");
}

static void test_boot_splash_config_reads_vm_section() {
    printf("\nTest: boot splash config reads vm.bootSplash\n");
    const char* json =
        "{\"vm\":{\"bootSplash\":{"
        "\"enabled\":true,"
        "\"asset\":\"/assets/boot/custom.nvg\","
        "\"fadeInMs\":250,"
        "\"holdMs\":750,"
        "\"fadeOutMs\":125,"
        "\"background\":2,"
        "\"border\":4"
        "}}}";

    BootSplashConfig cfg;
    bool ok = parseBootSplashConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check("splash section was configured", cfg.configured);
    check("splash is enabled", cfg.enabled);
    check_eq_str("splash asset path", cfg.assetPath,
                 "/assets/boot/custom.nvg");
    check_eq_uint("fade in ms", cfg.fadeInMs, 250);
    check_eq_uint("hold ms", cfg.holdMs, 750);
    check_eq_uint("fade out ms", cfg.fadeOutMs, 125);
    check_eq_uint("background color", cfg.background, 2);
    check_eq_uint("border color", cfg.border, 4);
}

static void test_boot_splash_defaults_when_section_missing() {
    printf("\nTest: missing boot splash section keeps defaults\n");
    const char* json =
        "{\"vm\":{\"defaultRuntime\":\"novabasic\"},"
        "\"languages\":{\"novabasic\":{\"rom\":\"/roms/novabasic.bin\"}}}";

    BootSplashConfig cfg;
    bool ok = parseBootSplashConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check("splash section was not configured", !cfg.configured);
    check("splash defaults to enabled", cfg.enabled);
    check_eq_str("default splash asset", cfg.assetPath,
                 BOOT_SPLASH_DEFAULT_ASSET);
    check_eq_uint("default fade in", cfg.fadeInMs,
                  BOOT_SPLASH_DEFAULT_FADE_IN_MS);
    check_eq_uint("default hold", cfg.holdMs,
                  BOOT_SPLASH_DEFAULT_HOLD_MS);
    check_eq_uint("default fade out", cfg.fadeOutMs,
                  BOOT_SPLASH_DEFAULT_FADE_OUT_MS);
}

static void test_boot_splash_disabled_is_preserved() {
    printf("\nTest: boot splash disabled flag is preserved\n");
    const char* json = "{\"vm\":{\"bootSplash\":{\"enabled\":false}}}";

    BootSplashConfig cfg;
    bool ok = parseBootSplashConfigText(json, strlen(json), cfg);
    check("parse succeeds", ok);
    check("splash section was configured", cfg.configured);
    check("splash is disabled", !cfg.enabled);
    check_eq_str("disabled splash still has fallback asset", cfg.assetPath,
                 BOOT_SPLASH_DEFAULT_ASSET);
}

int main() {
    test_default_runtime_value_does_not_select_first_rom();
    test_default_runtime_falls_back_to_novabasic();
    test_default_runtime_is_scoped_to_vm_object();
    test_runtime_search_is_bounded_to_selected_object();
    test_missing_active_runtime_is_invalid();
    test_boot_splash_config_reads_vm_section();
    test_boot_splash_defaults_when_section_missing();
    test_boot_splash_disabled_is_preserved();

    printf("\nBoot config parser tests: %d passed, %d failed\n",
           g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
