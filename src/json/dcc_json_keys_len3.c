#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len3(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "tts")) {
        return DCC_JSON_KEY_TTS;
    }
    if (dcc_json_key_span_eq(key, len, "bot")) {
        return DCC_JSON_KEY_BOT;
    }
    if (dcc_json_key_span_eq(key, len, "ids")) {
        return DCC_JSON_KEY_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "web")) {
        return DCC_JSON_KEY_WEB;
    }
    if (dcc_json_key_span_eq(key, len, "url")) {
        return DCC_JSON_KEY_URL;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
