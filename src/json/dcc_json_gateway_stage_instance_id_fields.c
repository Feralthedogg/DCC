#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_id_field(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || stage_instance == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    uint64_t value = 0;

    if (key_id == DCC_JSON_KEY_ID) {
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->id = value;
        return DCC_OK;
    }

    int has_value = 0;
    if (key_id == DCC_JSON_KEY_GUILD_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->guild_id = has_value ? value : 0;
        return DCC_OK;
    }
    if (key_id == DCC_JSON_KEY_CHANNEL_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->channel_id = has_value ? value : 0;
        return DCC_OK;
    }
    if (key_id == DCC_JSON_KEY_GUILD_SCHEDULED_EVENT_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        stage_instance->scheduled_event_id = has_value ? value : 0;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
