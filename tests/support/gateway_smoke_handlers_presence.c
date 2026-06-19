#include "gateway_smoke_handlers.h"

#include <string.h>

void on_presence_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_presence_t *presence = dcc_event_presence(event);
    if (data != NULL &&
        presence != NULL &&
        dcc_event_type(event) == DCC_EVENT_PRESENCE_UPDATE &&
        data->guild_id == 333 &&
        data->user_id == 446 &&
        data->user.id == 446 &&
        data->user.username != NULL &&
        strcmp(data->user.username, "member") == 0 &&
        data->user.avatar != NULL &&
        strcmp(data->user.avatar, "member-avatar") == 0 &&
        data->user.flags == 64 &&
        data->status != NULL &&
        strcmp(data->status, "online") == 0 &&
        data->since == 123456 &&
        data->desktop_status != NULL &&
        strcmp(data->desktop_status, "online") == 0 &&
        data->mobile_status != NULL &&
        strcmp(data->mobile_status, "idle") == 0 &&
        data->web_status != NULL &&
        strcmp(data->web_status, "dnd") == 0 &&
        data->activities != NULL &&
        data->activities_count == 1 &&
        data->activities[0].name != NULL &&
        strcmp(data->activities[0].name, "coding") == 0 &&
        data->activities[0].state != NULL &&
        strcmp(data->activities[0].state, "deep") == 0 &&
        data->activities[0].details != NULL &&
        strcmp(data->activities[0].details, "parser") == 0 &&
        data->activities[0].url == NULL &&
        data->activities[0].created_at == 123456789 &&
        data->activities[0].application_id == 666 &&
        data->activities[0].flags == 1 &&
        data->activities[0].emoji_id == 9999 &&
        data->activities[0].emoji_name != NULL &&
        strcmp(data->activities[0].emoji_name, "spark") == 0 &&
        presence->guild_id == 333 &&
        presence->user.id == 446 &&
        presence->user.username != NULL &&
        strcmp(presence->user.username, "member") == 0 &&
        presence->user.avatar != NULL &&
        strcmp(presence->user.avatar, "member-avatar") == 0 &&
        presence->user.flags == 64 &&
        presence->status != NULL &&
        strcmp(presence->status, "online") == 0 &&
        presence->since == 123456 &&
        presence->desktop_status != NULL &&
        strcmp(presence->desktop_status, "online") == 0 &&
        presence->mobile_status != NULL &&
        strcmp(presence->mobile_status, "idle") == 0 &&
        presence->web_status != NULL &&
        strcmp(presence->web_status, "dnd") == 0 &&
        presence->activities != NULL &&
        presence->activities_count == 1 &&
        presence->activities[0].name != NULL &&
        strcmp(presence->activities[0].name, "coding") == 0 &&
        presence->activities[0].emoji_id == 9999 &&
        presence->activities[0].emoji_name != NULL &&
        strcmp(presence->activities[0].emoji_name, "spark") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_presence_t *presence_copy = dcc_presence_clone(presence);
        if (copy != NULL &&
            copy->user.avatar != NULL &&
            strcmp(copy->user.avatar, "member-avatar") == 0 &&
            copy->user.flags == 64 &&
            copy->desktop_status != NULL &&
            strcmp(copy->desktop_status, "online") == 0 &&
            copy->mobile_status != NULL &&
            strcmp(copy->mobile_status, "idle") == 0 &&
            copy->web_status != NULL &&
            strcmp(copy->web_status, "dnd") == 0 &&
            copy->activities != NULL &&
            copy->activities_count == 1 &&
            copy->activities[0].name != NULL &&
            strcmp(copy->activities[0].name, "coding") == 0 &&
            copy->activities[0].state != NULL &&
            strcmp(copy->activities[0].state, "deep") == 0 &&
            copy->activities[0].details != NULL &&
            strcmp(copy->activities[0].details, "parser") == 0 &&
            copy->activities[0].url == NULL &&
            copy->activities[0].created_at == 123456789 &&
            copy->activities[0].application_id == 666 &&
            copy->activities[0].flags == 1 &&
            copy->activities[0].emoji_id == 9999 &&
            copy->activities[0].emoji_name != NULL &&
            strcmp(copy->activities[0].emoji_name, "spark") == 0 &&
            presence_copy != NULL &&
            presence_copy->user.avatar != NULL &&
            strcmp(presence_copy->user.avatar, "member-avatar") == 0 &&
            presence_copy->user.flags == 64 &&
            presence_copy->desktop_status != NULL &&
            strcmp(presence_copy->desktop_status, "online") == 0 &&
            presence_copy->activities != NULL &&
            presence_copy->activities_count == 1 &&
            presence_copy->activities[0].name != NULL &&
            strcmp(presence_copy->activities[0].name, "coding") == 0 &&
            presence_copy->activities[0].emoji_name != NULL &&
            strcmp(presence_copy->activities[0].emoji_name, "spark") == 0) {
            state->presence_update_seen = 1;
        }
        dcc_presence_free(presence_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_thread_member_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_thread_member_t *thread_member = dcc_event_thread_member(event);
    if (data != NULL &&
        thread_member != NULL &&
        dcc_event_type(event) == DCC_EVENT_THREAD_MEMBER_UPDATE &&
        data->id == 888 &&
        data->user_id == 446 &&
        data->guild_id == 333 &&
        data->flags == 1 &&
        data->join_timestamp != NULL &&
        strcmp(data->join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
        thread_member->id == 888 &&
        thread_member->user_id == 446 &&
        thread_member->guild_id == 333 &&
        thread_member->flags == 1 &&
        thread_member->join_timestamp != NULL &&
        strcmp(thread_member->join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_thread_member_t *thread_copy = dcc_thread_member_clone(thread_member);
        if (copy != NULL &&
            copy->join_timestamp != NULL &&
            strcmp(copy->join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
            thread_copy != NULL &&
            thread_copy->join_timestamp != NULL &&
            strcmp(thread_copy->join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0) {
            state->thread_member_update_seen = 1;
        }
        dcc_thread_member_free(thread_copy);
        dcc_gateway_event_data_free(copy);
    }
}
