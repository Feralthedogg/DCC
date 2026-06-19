#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser == NULL || out == NULL || stored == NULL || truncated == NULL) {
        return DCC_ERR_JSON;
    }

    if (parser->cur < parser->end &&
        *parser->cur == '{' &&
        *stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
        size_t index = *stored;
        dcc_status_t status = dcc_json_gateway_parse_scheduled_event_list_object(
            parser,
            &out->gateway_guild_scheduled_events[index],
            out->gateway_guild_scheduled_event_names[index],
            sizeof(out->gateway_guild_scheduled_event_names[index]),
            out->gateway_guild_scheduled_event_descriptions[index],
            sizeof(out->gateway_guild_scheduled_event_descriptions[index]),
            out->gateway_guild_scheduled_event_images[index],
            sizeof(out->gateway_guild_scheduled_event_images[index]),
            out->gateway_guild_scheduled_event_locations[index],
            sizeof(out->gateway_guild_scheduled_event_locations[index]),
            out->gateway_guild_scheduled_event_start_times[index],
            sizeof(out->gateway_guild_scheduled_event_start_times[index]),
            out->gateway_guild_scheduled_event_end_times[index],
            sizeof(out->gateway_guild_scheduled_event_end_times[index]),
            out->gateway_guild_scheduled_event_creator_usernames[index],
            sizeof(out->gateway_guild_scheduled_event_creator_usernames[index]),
            out->gateway_guild_scheduled_event_creator_global_names[index],
            sizeof(out->gateway_guild_scheduled_event_creator_global_names[index]),
            out->gateway_guild_scheduled_event_creator_avatars[index],
            sizeof(out->gateway_guild_scheduled_event_creator_avatars[index])
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->gateway_guild_scheduled_events[index].id != 0 &&
            index < DCC_JSON_GATEWAY_ID_LIST_CAP) {
            out->gateway_ids[index] = out->gateway_guild_scheduled_events[index].id;
        }
        *stored = index + 1U;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }
    *truncated = 1;
    return DCC_OK;
}
