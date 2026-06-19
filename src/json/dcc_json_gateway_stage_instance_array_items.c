#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser->cur < parser->end && *parser->cur == '{' &&
        *stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
        dcc_status_t status = dcc_json_gateway_parse_stage_instance_list_object(
            parser,
            &out->gateway_guild_stage_instances[*stored],
            out->gateway_guild_stage_instance_topics[*stored],
            sizeof(out->gateway_guild_stage_instance_topics[*stored])
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->gateway_guild_stage_instances[*stored].id != 0 &&
            *stored < DCC_JSON_GATEWAY_ID_LIST_CAP) {
            out->gateway_ids[*stored] = out->gateway_guild_stage_instances[*stored].id;
        }
        (*stored)++;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status == DCC_OK) {
        *truncated = 1;
    }
    return status;
}
