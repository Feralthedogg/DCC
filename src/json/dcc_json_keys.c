#include "internal/json/dcc_json_internal.h"

#include <stddef.h>
#include <string.h>

int dcc_json_key_span_eq(const char *key, size_t len, const char *literal) {
    size_t literal_len = strlen(literal);
    return len == literal_len && memcmp(key, literal, len) == 0;
}

dcc_json_key_id_t dcc_json_gateway_key_id(const char *key, size_t len) {
    if (len <= 5U) {
        return dcc_json_gateway_key_id_short(key, len);
    }
    if (len <= 10U) {
        return dcc_json_gateway_key_id_medium(key, len);
    }
    if (len <= 15U) {
        return dcc_json_gateway_key_id_long(key, len);
    }
    return dcc_json_gateway_key_id_extended(key, len);
}
