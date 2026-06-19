#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_scheduled_event_event_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_GUILD_SCHEDULED_EVENT_ID) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    out->gateway.scheduled_event_id = value;
    out->scheduled_event.id = value;
    out->invite.scheduled_event_id = value;
    out->stage_instance.scheduled_event_id = value;
    out->scheduled_event_user_update.scheduled_event_id = value;
    out->has_scheduled_event = 1;
    out->has_invite = 1;
    out->has_stage_instance = 1;
    out->has_scheduled_event_user_update = 1;
    (void)has_value;

    *handled = 1;
    return DCC_OK;
}
