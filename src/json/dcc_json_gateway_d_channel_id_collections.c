#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_channel_id_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_CHANNEL_IDS &&
        key_id != DCC_JSON_KEY_EXEMPT_CHANNELS) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_snowflake_array(
        parser,
        out->gateway_channel_ids,
        DCC_JSON_GATEWAY_ID_LIST_CAP,
        &out->gateway.channel_ids,
        &out->gateway.channel_ids_count,
        &out->gateway.channel_ids_truncated
    );
    if (status != DCC_OK) {
        return status;
    }
    out->auto_moderation_rule.channel_ids = out->gateway.channel_ids;
    out->auto_moderation_rule.channel_ids_count = out->gateway.channel_ids_count;
    out->auto_moderation_rule.channel_ids_truncated = out->gateway.channel_ids_truncated;
    out->thread_list_sync.channel_ids = out->gateway.channel_ids;
    out->thread_list_sync.channel_ids_count = out->gateway.channel_ids_count;
    out->thread_list_sync.channel_ids_truncated = out->gateway.channel_ids_truncated;
    out->has_auto_moderation_rule = 1;
    out->has_thread_list_sync = 1;
    *handled = 1;
    return DCC_OK;
}
