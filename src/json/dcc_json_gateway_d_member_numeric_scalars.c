#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_member_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_COUNT:
        case DCC_JSON_KEY_CHUNK_INDEX:
        case DCC_JSON_KEY_CHUNK_COUNT:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_COUNT) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.count = value;
        out->thread_members_update.count = value;
        out->has_thread_members_update = 1;
    } else if (key_id == DCC_JSON_KEY_CHUNK_INDEX) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.chunk_index = value;
        out->guild_members_chunk.chunk_index = value;
        out->has_guild_members_chunk = 1;
    } else if (key_id == DCC_JSON_KEY_CHUNK_COUNT) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.chunk_count = value;
        out->guild_members_chunk.chunk_count = value;
        out->has_guild_members_chunk = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
