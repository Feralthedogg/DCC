#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_resolved_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
) {
    if (key_id == DCC_JSON_KEY_USERS) {
        return dcc_json_gateway_parse_resolved_user_map(parser, out);
    }
    if (key_id == DCC_JSON_KEY_MEMBERS) {
        return dcc_json_gateway_parse_resolved_member_map(parser, out);
    }
    if (key_id == DCC_JSON_KEY_ROLES) {
        return dcc_json_gateway_parse_resolved_role_map(parser, out);
    }
    if (key_id == DCC_JSON_KEY_CHANNELS) {
        return dcc_json_gateway_parse_resolved_channel_map(parser, out);
    }
    if (key_id == DCC_JSON_KEY_MESSAGES) {
        return dcc_json_gateway_parse_resolved_message_map(parser, out);
    }
    if (key_id == DCC_JSON_KEY_ATTACHMENTS) {
        return dcc_json_gateway_parse_resolved_attachment_map(parser, out);
    }
    return dcc_json_skip_value(parser);
}
