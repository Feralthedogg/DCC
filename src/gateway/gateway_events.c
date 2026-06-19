#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_event_names_internal.h"

#include <stdint.h>
#include <string.h>

static uint64_t dcc_gateway_load_u64_unaligned(const char *ptr) {
    uint64_t value = 0;
    memcpy(&value, ptr, sizeof(value));
    return value;
}

static uint32_t dcc_gateway_load_u32_unaligned(const char *ptr) {
    uint32_t value = 0;
    memcpy(&value, ptr, sizeof(value));
    return value;
}

static int dcc_gateway_event_name_eq_fixed(const char *name, const char *known, size_t len) {
    size_t offset = 0;
    while (len - offset >= sizeof(uint64_t)) {
        if (dcc_gateway_load_u64_unaligned(name + offset) != dcc_gateway_load_u64_unaligned(known + offset)) {
            return 0;
        }
        offset += sizeof(uint64_t);
    }
    if (len - offset >= sizeof(uint32_t)) {
        if (dcc_gateway_load_u32_unaligned(name + offset) != dcc_gateway_load_u32_unaligned(known + offset)) {
            return 0;
        }
        offset += sizeof(uint32_t);
    }
    return memcmp(name + offset, known + offset, len - offset) == 0;
}

dcc_event_type_t dcc_gateway_event_type_from_name(const char *name) {
    if (name == NULL) {
        return DCC_EVENT_RAW;
    }

    size_t name_len = strlen(name);
    if (name_len == 0) {
        return DCC_EVENT_RAW;
    }

    for (size_t i = 0; i < dcc_gateway_event_name_count; ++i) {
        const dcc_gateway_event_name_entry_t *entry = &dcc_gateway_event_names[i];
        if (entry->len == name_len &&
            name[0] == entry->name[0] &&
            name[name_len - 1U] == entry->name[entry->len - 1U] &&
            dcc_gateway_event_name_eq_fixed(name, entry->name, entry->len)) {
            return entry->type;
        }
    }
    return DCC_EVENT_RAW;
}
