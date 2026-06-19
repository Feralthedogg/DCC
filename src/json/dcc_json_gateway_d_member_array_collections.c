#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_member_array_collection(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_MEMBERS &&
        out->has_event_name &&
        dcc_json_event_name_eq(
            out->event_name,
            sizeof(out->event_name),
            "GUILD_MEMBERS_CHUNK",
            sizeof("GUILD_MEMBERS_CHUNK") - 1
        )) {
        return dcc_json_gateway_parse_guild_member_array(parser, out);
    }

    if (key_id == DCC_JSON_KEY_MEMBERS ||
        key_id == DCC_JSON_KEY_ADDED_MEMBERS) {
        return dcc_json_gateway_parse_thread_member_array(
            parser,
            out,
            key_id == DCC_JSON_KEY_ADDED_MEMBERS
        );
    }

    return DCC_ERR_JSON;
}
