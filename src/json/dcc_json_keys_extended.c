#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_extended(const char *key, size_t len) {
    switch (len) {
        case 16:
            return dcc_json_gateway_key_id_len16(key, len);
        case 17:
            return dcc_json_gateway_key_id_len17(key, len);
        case 18:
            return dcc_json_gateway_key_id_len18(key, len);
        default:
            return dcc_json_gateway_key_id_extended_tail(key, len);
    }
}
