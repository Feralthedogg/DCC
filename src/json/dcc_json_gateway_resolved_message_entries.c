#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
) {
    dcc_status_t status = dcc_json_gateway_parse_resolved_message_object(
        parser,
        &out->interaction_resolved_messages[index],
        out->interaction_resolved_message_contents[index],
        sizeof(out->interaction_resolved_message_contents[index]),
        out->interaction_resolved_message_timestamps[index],
        sizeof(out->interaction_resolved_message_timestamps[index]),
        out->interaction_resolved_message_edited_timestamps[index],
        sizeof(out->interaction_resolved_message_edited_timestamps[index]),
        out->interaction_resolved_message_author_usernames[index],
        sizeof(out->interaction_resolved_message_author_usernames[index]),
        out->interaction_resolved_message_author_global_names[index],
        sizeof(out->interaction_resolved_message_author_global_names[index]),
        out->interaction_resolved_message_author_avatars[index],
        sizeof(out->interaction_resolved_message_author_avatars[index])
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->interaction_resolved_messages[index].id == 0) {
        out->interaction_resolved_messages[index].id = key;
    }
    if (out->interaction_resolved_messages[index].guild_id == 0) {
        out->interaction_resolved_messages[index].guild_id = out->interaction.guild_id;
    }
    return DCC_OK;
}
