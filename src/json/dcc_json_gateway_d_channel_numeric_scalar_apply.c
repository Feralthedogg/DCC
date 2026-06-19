#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_apply_d_channel_numeric_scalar_key(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint32_t value,
    int *handled
) {
    if (out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_CHANNEL_TYPE) {
        out->gateway.channel_type = value;
        out->channel.type = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_TYPE;
    } else if (key_id == DCC_JSON_KEY_POSITION) {
        out->channel.position = value;
        out->role.position = value;
        out->has_channel = 1;
        out->has_role = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_POSITION | DCC_CACHE_FIELD_ROLE_POSITION;
    } else if (key_id == DCC_JSON_KEY_RATE_LIMIT_PER_USER) {
        out->channel.rate_limit_per_user = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_RATE_LIMIT_PER_USER;
    } else if (key_id == DCC_JSON_KEY_BITRATE) {
        out->channel.bitrate = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_BITRATE;
    } else if (key_id == DCC_JSON_KEY_USER_LIMIT) {
        out->channel.user_limit = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_USER_LIMIT;
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
