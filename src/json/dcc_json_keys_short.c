#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_short(const char *key, size_t len) {
    switch (len) {
        case 1:
            switch (key[0]) {
                case 's':
                    return DCC_JSON_KEY_S;
                case 't':
                    return DCC_JSON_KEY_T;
                case 'd':
                    return DCC_JSON_KEY_D;
                default:
                    return DCC_JSON_KEY_UNKNOWN;
            }
        case 2:
            if (key[0] == 'o' && key[1] == 'p') {
                return DCC_JSON_KEY_OP;
            }
            if (key[0] == 'i' && key[1] == 'd') {
                return DCC_JSON_KEY_ID;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 3:
            return dcc_json_gateway_key_id_len3(key, len);
        case 4:
            return dcc_json_gateway_key_id_len4(key, len);
        case 5:
            return dcc_json_gateway_key_id_len5(key, len);
        default:
            return DCC_JSON_KEY_UNKNOWN;
    }
}
