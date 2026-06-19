#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_medium_tail(const char *key, size_t len) {
    switch (len) {
        case 9:
            return dcc_json_gateway_key_id_len9(key, len);
        case 10:
            return dcc_json_gateway_key_id_len10(key, len);
        default:
            return DCC_JSON_KEY_UNKNOWN;
    }
}
