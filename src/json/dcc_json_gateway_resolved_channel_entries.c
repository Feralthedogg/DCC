#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_channel_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_thread_list_object(
        parser,
        &out->interaction_resolved_channels[index],
        out->interaction_resolved_channel_names[index],
        sizeof(out->interaction_resolved_channel_names[index]),
        out->interaction_resolved_channel_topics[index],
        sizeof(out->interaction_resolved_channel_topics[index])
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->interaction_resolved_channels[index].id == 0) {
        out->interaction_resolved_channels[index].id = key;
    }
    if (out->interaction_resolved_channels[index].guild_id == 0) {
        out->interaction_resolved_channels[index].guild_id = out->interaction.guild_id;
    }
    return DCC_OK;
}
