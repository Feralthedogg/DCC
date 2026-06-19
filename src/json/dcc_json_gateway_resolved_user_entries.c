#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_user_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
) {
    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &out->interaction_resolved_users[index],
        out->interaction_resolved_usernames[index],
        sizeof(out->interaction_resolved_usernames[index]),
        out->interaction_resolved_user_global_names[index],
        sizeof(out->interaction_resolved_user_global_names[index]),
        out->interaction_resolved_user_avatars[index],
        sizeof(out->interaction_resolved_user_avatars[index])
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->interaction_resolved_users[index].id == 0) {
        out->interaction_resolved_users[index].id = key;
    }
    return DCC_OK;
}
