#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>
#include <string.h>

int dcc_voice_gateway_json_u64(const dcc_json_t *json, uint64_t *out) {
    if (json == NULL || out == NULL) {
        return -1;
    }
    if (dcc_json_u64(json, out) == 0) {
        return 0;
    }

    const char *text = dcc_json_string(json);
    if (text == NULL || text[0] == '\0') {
        return -1;
    }

    uint64_t value = 0;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return -1;
        }
        uint64_t digit = (uint64_t)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return -1;
        }
        value = value * 10U + digit;
    }

    *out = value;
    return 0;
}

int dcc_voice_gateway_json_u32(const dcc_json_t *json, uint32_t *out) {
    uint64_t value = 0;
    if (out == NULL || dcc_voice_gateway_json_u64(json, &value) != 0 || value > UINT32_MAX) {
        return -1;
    }
    *out = (uint32_t)value;
    return 0;
}

int dcc_voice_gateway_json_i32(const dcc_json_t *json, int32_t *out) {
    int64_t value = 0;
    if (json == NULL || out == NULL || dcc_json_i64(json, &value) != 0 ||
        value < INT32_MIN ||
        value > INT32_MAX) {
        return -1;
    }
    *out = (int32_t)value;
    return 0;
}

int dcc_voice_gateway_json_u16(const dcc_json_t *json, uint16_t *out) {
    uint64_t value = 0;
    if (out == NULL || dcc_voice_gateway_json_u64(json, &value) != 0 || value > UINT16_MAX) {
        return -1;
    }
    *out = (uint16_t)value;
    return 0;
}

dcc_voice_dave_version_t dcc_voice_gateway_dave_version_from_u32(uint32_t version) {
    return version == (uint32_t)DCC_VOICE_DAVE_VERSION_1
        ? DCC_VOICE_DAVE_VERSION_1
        : DCC_VOICE_DAVE_NONE;
}

dcc_status_t dcc_voice_gateway_parse_secret_key(dcc_voice_client_t *voice_client, const dcc_json_t *json) {
    if (voice_client == NULL || json == NULL || dcc_json_typeof(json) != DCC_JSON_ARRAY) {
        return DCC_ERR_JSON;
    }
    if (dcc_json_array_size(json) != DCC_VOICE_SECRET_KEY_SIZE) {
        return DCC_ERR_JSON;
    }

    uint8_t key[DCC_VOICE_SECRET_KEY_SIZE];
    for (size_t i = 0; i < DCC_VOICE_SECRET_KEY_SIZE; ++i) {
        uint64_t value = 0;
        if (dcc_voice_gateway_json_u64(dcc_json_array_get(json, i), &value) != 0 || value > UINT8_MAX) {
            return DCC_ERR_JSON;
        }
        key[i] = (uint8_t)value;
    }
    return dcc_voice_client_set_secret_key(voice_client, key, sizeof(key));
}

void dcc_voice_gateway_select_mode_from_ready(dcc_voice_client_t *voice_client, const dcc_json_t *modes) {
    if (voice_client == NULL || modes == NULL || dcc_json_typeof(modes) != DCC_JSON_ARRAY) {
        return;
    }

    size_t count = dcc_json_array_size(modes);
    for (size_t i = 0; i < count; ++i) {
        const char *mode = dcc_json_string(dcc_json_array_get(modes, i));
        if (mode != NULL && strcmp(mode, "aead_xchacha20_poly1305_rtpsize") == 0) {
            voice_client->encryption_mode = DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE;
            return;
        }
    }
}
