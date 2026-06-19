#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_long(const char *key, size_t len) {
    switch (len) {
        case 11:
            return dcc_json_gateway_key_id_len11(key, len);
        case 12:
            return dcc_json_gateway_key_id_len12(key, len);
        case 13:
            return dcc_json_gateway_key_id_len13(key, len);
        case 14:
            return dcc_json_gateway_key_id_len14(key, len);
        case 15:
            return dcc_json_gateway_key_id_len15(key, len);
        default:
            return DCC_JSON_KEY_UNKNOWN;
    }
}
