#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_guild_presence_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t index
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_presence_list_object(
        parser,
        &out->gateway_guild_presences[index],
        out->gateway_guild_presence_usernames[index],
        sizeof(out->gateway_guild_presence_usernames[index]),
        out->gateway_guild_presence_user_global_names[index],
        sizeof(out->gateway_guild_presence_user_global_names[index]),
        out->gateway_guild_presence_user_avatars[index],
        sizeof(out->gateway_guild_presence_user_avatars[index]),
        out->gateway_guild_presence_statuses[index],
        sizeof(out->gateway_guild_presence_statuses[index]),
        out->gateway_guild_presence_desktop_statuses[index],
        sizeof(out->gateway_guild_presence_desktop_statuses[index]),
        out->gateway_guild_presence_mobile_statuses[index],
        sizeof(out->gateway_guild_presence_mobile_statuses[index]),
        out->gateway_guild_presence_web_statuses[index],
        sizeof(out->gateway_guild_presence_web_statuses[index]),
        out->gateway_guild_presence_activities[index],
        out->gateway_guild_presence_activity_names[index][0],
        sizeof(out->gateway_guild_presence_activity_names[index][0]),
        out->gateway_guild_presence_activity_states[index][0],
        sizeof(out->gateway_guild_presence_activity_states[index][0]),
        out->gateway_guild_presence_activity_details[index][0],
        sizeof(out->gateway_guild_presence_activity_details[index][0]),
        out->gateway_guild_presence_activity_urls[index][0],
        sizeof(out->gateway_guild_presence_activity_urls[index][0]),
        out->gateway_guild_presence_activity_emoji_names[index][0],
        sizeof(out->gateway_guild_presence_activity_emoji_names[index][0])
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->gateway_guild_presences[index].user.id != 0 && index < DCC_JSON_GATEWAY_ID_LIST_CAP) {
        out->gateway_ids[index] = out->gateway_guild_presences[index].user.id;
    }
    return DCC_OK;
}
