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

int main() {
    test_default_runtime_value_does_not_select_first_rom();
    test_default_runtime_falls_back_to_novabasic();
    test_default_runtime_is_scoped_to_vm_object();
    test_runtime_search_is_bounded_to_selected_object();
    test_missing_active_runtime_is_invalid();

    printf("\nBoot config parser tests: %d passed, %d failed\n",
           g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
