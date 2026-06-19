#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_guild_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_CHANNELS) {
        dcc_status_t status = dcc_json_gateway_parse_guild_channel_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_THREADS) {
        dcc_status_t status = dcc_json_gateway_parse_thread_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_GUILD_SCHEDULED_EVENTS) {
        dcc_status_t status = dcc_json_gateway_parse_guild_scheduled_event_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_STAGE_INSTANCES) {
        dcc_status_t status = dcc_json_gateway_parse_stage_instance_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_GUILDS) {
        dcc_status_t status = dcc_json_parse_object_id_array(
            parser,
            0,
            out->ready_guild_id_storage,
            DCC_JSON_GATEWAY_ID_LIST_CAP,
            &out->ready_guild_ids,
            &out->ready_guild_ids_count,
            &out->ready_guild_ids_truncated,
            &out->ready_guild_count
        );
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_PRESENCES) {
        dcc_status_t status = dcc_json_gateway_parse_presence_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
