#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_medium(const char *key, size_t len) {
    switch (len) {
        case 6:
            return dcc_json_gateway_key_id_len6(key, len);
        case 7:
            return dcc_json_gateway_key_id_len7(key, len);
        case 8:
            return dcc_json_gateway_key_id_len8(key, len);
        case 9:
        case 10:
            return dcc_json_gateway_key_id_medium_tail(key, len);
        default:
            return DCC_JSON_KEY_UNKNOWN;
    }
}
