#include "internal/json/dcc_json_internal.h"

#include <stdint.h>
#include <string.h>

static uint64_t dcc_json_load_u64_unaligned(const char *ptr) {
    uint64_t value = 0;
    memcpy(&value, ptr, sizeof(value));
    return value;
}

static uint32_t dcc_json_load_u32_unaligned(const char *ptr) {
    uint32_t value = 0;
    memcpy(&value, ptr, sizeof(value));
    return value;
}

static int dcc_json_fixed_prefix_eq(const char *name, const char *prefix, size_t prefix_len) {
    size_t offset = 0;
    while (prefix_len - offset >= sizeof(uint64_t)) {
        if (dcc_json_load_u64_unaligned(name + offset) !=
            dcc_json_load_u64_unaligned(prefix + offset)) {
            return 0;
        }
        offset += sizeof(uint64_t);
    }
    if (prefix_len - offset >= sizeof(uint32_t)) {
        if (dcc_json_load_u32_unaligned(name + offset) !=
            dcc_json_load_u32_unaligned(prefix + offset)) {
            return 0;
        }
        offset += sizeof(uint32_t);
    }
    return offset == prefix_len ||
        memcmp(name + offset, prefix + offset, prefix_len - offset) == 0;
}

int dcc_json_event_prefix_eq(const char *name, size_t cap, const char *prefix, size_t prefix_len) {
    return cap >= prefix_len &&
        prefix_len > 0 &&
        name[0] == prefix[0] &&
        name[prefix_len - 1] == prefix[prefix_len - 1] &&
        dcc_json_fixed_prefix_eq(name, prefix, prefix_len);
}

int dcc_json_event_name_eq(const char *name, size_t cap, const char *literal, size_t literal_len) {
    return cap > literal_len &&
        name[literal_len] == '\0' &&
        dcc_json_event_prefix_eq(name, cap, literal, literal_len);
}
