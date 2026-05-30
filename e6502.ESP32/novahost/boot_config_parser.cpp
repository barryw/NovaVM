#include "boot_config_parser.h"

#include <stdio.h>
#include <string.h>

namespace {

const char* skip_ws(const char* p, const char* end) {
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
        p++;
    return p;
}

const char* skip_json_string(const char* p, const char* end) {
    if (p >= end || *p != '"')
        return nullptr;

    p++;
    bool escaped = false;
    while (p < end) {
        char c = *p++;
        if (escaped) {
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"')
            return p;
    }

    return nullptr;
}

bool json_string_equals(const char* p, const char* end, const char* key,
                        const char** after) {
    if (p >= end || *p != '"')
        return false;

    p++;
    const char* k = key;
    while (p < end) {
        char c = *p++;
        if (c == '\\')
            return false;
        if (c == '"') {
            if (*k == '\0') {
                *after = p;
                return true;
            }
            return false;
        }
        if (*k == '\0' || c != *k)
            return false;
        k++;
    }

    return false;
}

const char* find_property_value(const char* begin, const char* end,
                                const char* key) {
    const char* p = begin;
    while (p < end) {
        if (*p != '"') {
            p++;
            continue;
        }

        const char* after = nullptr;
        if (json_string_equals(p, end, key, &after)) {
            const char* colon = skip_ws(after, end);
            if (colon < end && *colon == ':')
                return skip_ws(colon + 1, end);
        }

        const char* next = skip_json_string(p, end);
        if (!next)
            return nullptr;
        p = next;
    }

    return nullptr;
}

bool copy_json_string_value(const char* p, const char* end, char* out,
                            size_t out_len) {
    if (!out || out_len == 0 || p >= end || *p != '"')
        return false;

    p++;
    size_t used = 0;
    while (p < end) {
        char c = *p++;
        if (c == '"') {
            out[used] = '\0';
            return true;
        }
        if (c == '\\') {
            if (p >= end)
                return false;
            c = *p++;
            switch (c) {
                case '"':  c = '"';  break;
                case '\\': c = '\\'; break;
                case '/':  c = '/';  break;
                case 'b':  c = '\b'; break;
                case 'f':  c = '\f'; break;
                case 'n':  c = '\n'; break;
                case 'r':  c = '\r'; break;
                case 't':  c = '\t'; break;
                default:   return false;
            }
        }
        if (used + 1 >= out_len)
            return false;
        out[used++] = c;
    }

    return false;
}

const char* find_object_end(const char* p, const char* end) {
    if (p >= end || *p != '{')
        return nullptr;

    int depth = 0;
    bool in_string = false;
    bool escaped = false;
    while (p < end) {
        char c = *p++;
        if (in_string) {
            if (escaped) {
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == '"') {
                in_string = false;
            }
            continue;
        }

        if (c == '"') {
            in_string = true;
        } else if (c == '{') {
            depth++;
        } else if (c == '}') {
            depth--;
            if (depth == 0)
                return p;
        }
    }

    return nullptr;
}

bool find_object_value(const char* begin, const char* end, const char* key,
                       const char*& object_begin, const char*& object_end) {
    const char* value = find_property_value(begin, end, key);
    if (!value || value >= end || *value != '{')
        return false;

    const char* close = find_object_end(value, end);
    if (!close)
        return false;

    object_begin = value;
    object_end = close;
    return true;
}

bool find_string_property(const char* begin, const char* end, const char* key,
                          char* out, size_t out_len) {
    const char* value = find_property_value(begin, end, key);
    return value && copy_json_string_value(value, end, out, out_len);
}

} // namespace

bool parseBootRuntimeConfigText(const char* text, size_t len,
                                BootRuntimeConfig& out) {
    memset(&out, 0, sizeof(out));
    snprintf(out.runtime, sizeof(out.runtime), "%s", "novabasic");

    if (!text || len == 0)
        return false;

    const char* begin = text;
    const char* end = text + len;

    const char* vm_begin = nullptr;
    const char* vm_end = nullptr;
    if (find_object_value(begin, end, "vm", vm_begin, vm_end)) {
        char runtime[sizeof(out.runtime)] = {};
        if (find_string_property(vm_begin, vm_end, "defaultRuntime",
                                 runtime, sizeof(runtime)) &&
            runtime[0] != '\0') {
            snprintf(out.runtime, sizeof(out.runtime), "%s", runtime);
        }
    }

    const char* languages_begin = nullptr;
    const char* languages_end = nullptr;
    if (!find_object_value(begin, end, "languages", languages_begin,
                           languages_end))
        return false;

    const char* runtime_begin = nullptr;
    const char* runtime_end = nullptr;
    if (!find_object_value(languages_begin, languages_end, out.runtime,
                           runtime_begin, runtime_end))
        return false;

    if (!find_string_property(runtime_begin, runtime_end, "rom", out.romPath,
                              sizeof(out.romPath)))
        return false;

    find_string_property(runtime_begin, runtime_end, "extensionRom",
                         out.extRomPath, sizeof(out.extRomPath));
    out.valid = true;
    return true;
}
