#include "internal/json/dcc_json_internal.h"

static dcc_status_t dcc_json_gateway_parse_removed_member_ids(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_status_t status = dcc_json_parse_snowflake_array(
        parser,
        out->gateway_removed_member_ids,
        DCC_JSON_GATEWAY_ID_LIST_CAP,
        &out->gateway.removed_member_ids,
        &out->gateway.removed_member_ids_count,
        &out->gateway.removed_member_ids_truncated
    );
    if (status != DCC_OK) {
        return status;
    }

    out->thread_members_update.removed_member_ids = out->gateway.removed_member_ids;
    out->thread_members_update.removed_member_ids_count = out->gateway.removed_member_ids_count;
    out->thread_members_update.removed_member_ids_truncated =
        out->gateway.removed_member_ids_truncated;
    out->has_thread_members_update = 1;
    return DCC_OK;
}

static dcc_status_t dcc_json_gateway_parse_not_found_member_ids(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_status_t status = dcc_json_parse_snowflake_array(
        parser,
        out->gateway_not_found_ids,
        DCC_JSON_GATEWAY_ID_LIST_CAP,
        &out->gateway.not_found_ids,
        &out->gateway.not_found_ids_count,
        &out->gateway.not_found_ids_truncated
    );
    if (status != DCC_OK) {
        return status;
    }

    out->guild_members_chunk.not_found_ids = out->gateway.not_found_ids;
    out->guild_members_chunk.not_found_ids_count = out->gateway.not_found_ids_count;
    out->guild_members_chunk.not_found_ids_truncated = out->gateway.not_found_ids_truncated;
    out->has_guild_members_chunk = 1;
    return DCC_OK;
}

dcc_status_t dcc_json_gateway_parse_d_member_id_collection(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_REMOVED_MEMBER_IDS) {
        return dcc_json_gateway_parse_removed_member_ids(parser, out);
    }
    if (key_id == DCC_JSON_KEY_NOT_FOUND) {
        return dcc_json_gateway_parse_not_found_member_ids(parser, out);
    }
    return DCC_ERR_JSON;
}
