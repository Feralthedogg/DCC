#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

static int dcc_voice_endpoint_has_scheme(const char *endpoint) {
    return endpoint != NULL &&
        (strncmp(endpoint, "wss://", 6U) == 0 || strncmp(endpoint, "ws://", 5U) == 0);
}

static int dcc_voice_endpoint_has_path(const char *endpoint) {
    if (endpoint == NULL) {
        return 0;
    }
    const char *cursor = endpoint;
    if (strncmp(cursor, "wss://", 6U) == 0) {
        cursor += 6U;
    } else if (strncmp(cursor, "ws://", 5U) == 0) {
        cursor += 5U;
    }
    return strpbrk(cursor, "/?") != NULL;
}

dcc_status_t dcc_voice_client_build_websocket_url(
    const dcc_voice_client_t *voice_client,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->endpoint[0] == '\0') {
        return DCC_ERR_STATE;
    }

    dcc_voice_json_buffer_t url = {
        .data = out,
        .cap = out_cap,
    };
    if (!dcc_voice_endpoint_has_scheme(voice_client->endpoint) &&
        dcc_voice_json_append_cstr(&url, "wss://") != 0) {
        return dcc_voice_json_finish(&url, out_len);
    }
    if (dcc_voice_json_append_cstr(&url, voice_client->endpoint) != 0) {
        return dcc_voice_json_finish(&url, out_len);
    }
    if (!dcc_voice_endpoint_has_path(voice_client->endpoint)) {
        if (dcc_voice_json_append_cstr(&url, "/?v=") != 0 ||
            dcc_voice_json_append_u64(&url, DCC_VOICE_PROTOCOL_VERSION) != 0) {
            return dcc_voice_json_finish(&url, out_len);
        }
    }
    return dcc_voice_json_finish(&url, out_len);
}
