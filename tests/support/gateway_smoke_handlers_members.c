#include "gateway_smoke_handlers.h"

#include <string.h>

void on_typing_start(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_typing_start_t *typing = dcc_event_typing_start(event);
    const dcc_member_t *member = dcc_event_member(event);
    if (data != NULL &&
        typing != NULL &&
        member != NULL &&
        dcc_event_type(event) == DCC_EVENT_TYPING_START &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->user_id == 444 &&
        data->user.id == 444 &&
        data->user.username != NULL &&
        strcmp(data->user.username, "typer") == 0 &&
        data->timestamp == 123456 &&
        data->nick != NULL &&
        strcmp(data->nick, "tap") == 0 &&
        data->role_ids != NULL &&
        data->role_ids_count == 1 &&
        data->role_ids[0] == 555 &&
        data->joined_at != NULL &&
        data->premium_since == NULL &&
        data->pending == 1 &&
        data->communication_disabled_until != NULL &&
        data->flags == 2 &&
        member->guild_id == 333 &&
        member->user.id == 444 &&
        member->user.username != NULL &&
        strcmp(member->user.username, "typer") == 0 &&
        member->nick != NULL &&
        strcmp(member->nick, "tap") == 0 &&
        member->role_ids != NULL &&
        member->role_ids_count == 1 &&
        member->role_ids[0] == 555 &&
        member->pending == 1 &&
        member->flags == 2 &&
        typing->guild_id == 333 &&
        typing->channel_id == 222 &&
        typing->user_id == 444 &&
        typing->timestamp == 123456 &&
        typing->user.id == 444 &&
        typing->user.username != NULL &&
        strcmp(typing->user.username, "typer") == 0 &&
        typing->member.guild_id == 333 &&
        typing->member.user.id == 444 &&
        typing->member.user.username != NULL &&
        strcmp(typing->member.user.username, "typer") == 0 &&
        typing->member.nick != NULL &&
        strcmp(typing->member.nick, "tap") == 0 &&
        typing->member.role_ids != NULL &&
        typing->member.role_ids_count == 1 &&
        typing->member.role_ids[0] == 555 &&
        typing->member.pending == 1 &&
        typing->member.flags == 2) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_member_t *member_copy = dcc_member_clone(member);
        dcc_typing_start_t *typing_copy = dcc_typing_start_clone(typing);
        if (copy != NULL &&
            copy->user.username != NULL &&
            strcmp(copy->user.username, "typer") == 0 &&
            copy->nick != NULL &&
            strcmp(copy->nick, "tap") == 0 &&
            copy->role_ids != NULL &&
            copy->role_ids_count == 1 &&
            copy->role_ids[0] == 555 &&
            copy->joined_at != NULL &&
            copy->premium_since == NULL &&
            copy->communication_disabled_until != NULL &&
            copy->flags == 2 &&
            member_copy != NULL &&
            member_copy->user.username != NULL &&
            strcmp(member_copy->user.username, "typer") == 0 &&
            member_copy->role_ids != NULL &&
            member_copy->role_ids_count == 1 &&
            member_copy->role_ids[0] == 555 &&
            member_copy->flags == 2 &&
            typing_copy != NULL &&
            typing_copy->guild_id == 333 &&
            typing_copy->channel_id == 222 &&
            typing_copy->user_id == 444 &&
            typing_copy->timestamp == 123456 &&
            typing_copy->user.username != NULL &&
            strcmp(typing_copy->user.username, "typer") == 0 &&
            typing_copy->member.user.username != NULL &&
            strcmp(typing_copy->member.user.username, "typer") == 0 &&
            typing_copy->member.role_ids != NULL &&
            typing_copy->member.role_ids_count == 1 &&
            typing_copy->member.role_ids[0] == 555 &&
            typing_copy->member.flags == 2) {
            state->typing_start_seen = 1;
        }
        dcc_typing_start_free(typing_copy);
        dcc_member_free(member_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_thread_members_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_thread_members_update_t *thread_members = dcc_event_thread_members_update(event);
    if (data != NULL &&
        thread_members != NULL &&
        dcc_event_type(event) == DCC_EVENT_THREAD_MEMBERS_UPDATE &&
        data->id == 888 &&
        data->guild_id == 333 &&
        data->count == 2 &&
        data->removed_member_ids != NULL &&
        data->removed_member_ids_count == 2 &&
        data->removed_member_ids[0] == 444 &&
        data->removed_member_ids[1] == 445 &&
        data->removed_member_ids_truncated == 0 &&
        data->member_ids != NULL &&
        data->member_ids_count == 1 &&
        data->member_ids[0] == 446 &&
        data->added_members != NULL &&
        data->added_members_count == 1 &&
        data->added_members[0].id == 888 &&
        data->added_members[0].user_id == 446 &&
        data->added_members[0].guild_id == 333 &&
        data->added_members[0].flags == 1 &&
        data->added_members[0].join_timestamp != NULL &&
        strcmp(data->added_members[0].join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
        thread_members->id == 888 &&
        thread_members->guild_id == 333 &&
        thread_members->count == 2 &&
        thread_members->member_ids != NULL &&
        thread_members->member_ids_count == 1 &&
        thread_members->member_ids[0] == 446 &&
        thread_members->added_members != NULL &&
        thread_members->added_members_count == 1 &&
        thread_members->added_members[0].id == 888 &&
        thread_members->added_members[0].user_id == 446 &&
        thread_members->added_members[0].guild_id == 333 &&
        thread_members->added_members[0].flags == 1 &&
        thread_members->added_members[0].join_timestamp != NULL &&
        strcmp(
            thread_members->added_members[0].join_timestamp,
            "2026-06-15T00:00:00.000000+00:00"
        ) == 0 &&
        thread_members->removed_member_ids != NULL &&
        thread_members->removed_member_ids_count == 2 &&
        thread_members->removed_member_ids[0] == 444 &&
        thread_members->removed_member_ids[1] == 445 &&
        thread_members->removed_member_ids_truncated == 0) {
        dcc_thread_members_update_t *copy = dcc_thread_members_update_clone(thread_members);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->removed_member_ids != NULL &&
            copy->removed_member_ids_count == 2 &&
            copy->removed_member_ids[1] == 445 &&
            copy->added_members != NULL &&
            copy->added_members_count == 1 &&
            copy->added_members[0].join_timestamp != NULL &&
            strcmp(copy->added_members[0].join_timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
            data_copy != NULL &&
            data_copy->added_members != NULL &&
            data_copy->added_members_count == 1 &&
            data_copy->added_members[0].user_id == 446 &&
            data_copy->added_members[0].join_timestamp != NULL &&
            strcmp(
                data_copy->added_members[0].join_timestamp,
                "2026-06-15T00:00:00.000000+00:00"
            ) == 0) {
            state->thread_members_seen = 1;
        }
        dcc_thread_members_update_free(copy);
        dcc_gateway_event_data_free(data_copy);
    }
}

void on_guild_member_add(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_member_t *member = dcc_event_member(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        member != NULL &&
        type == DCC_EVENT_GUILD_MEMBER_ADD &&
        data->guild_id == 333 &&
        data->user_id == 446 &&
        data->user.id == 446 &&
        data->user.username != NULL &&
        strcmp(data->user.username, "member") == 0 &&
        data->user.global_name != NULL &&
        strcmp(data->user.global_name, "Member") == 0 &&
        data->user.discriminator == 0 &&
        data->user.bot == 0 &&
        member->guild_id == 333 &&
        member->user.id == 446 &&
        member->user.username != NULL &&
        strcmp(member->user.username, "member") == 0 &&
        member->user.global_name != NULL &&
        strcmp(member->user.global_name, "Member") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_member_t *member_copy = dcc_member_clone(member);
        if (copy != NULL &&
            copy->user.id == 446 &&
            copy->user.username != NULL &&
            strcmp(copy->user.username, "member") == 0 &&
            member_copy != NULL &&
            member_copy->user.id == 446 &&
            member_copy->user.username != NULL &&
            strcmp(member_copy->user.username, "member") == 0) {
            state->guild_member_add_seen = 1;
        }
        dcc_member_free(member_copy);
        dcc_gateway_event_data_free(copy);
    } else if (data != NULL &&
               member != NULL &&
               type == DCC_EVENT_GUILD_MEMBER_REMOVE &&
               data->guild_id == 333 &&
               data->user_id == 446 &&
               data->user.id == 446 &&
               data->user.username != NULL &&
               strcmp(data->user.username, "member") == 0 &&
               data->nick == NULL &&
               data->role_ids == NULL &&
               data->role_ids_count == 0 &&
               member->guild_id == 333 &&
               member->user.id == 446 &&
               member->user.username != NULL &&
               strcmp(member->user.username, "member") == 0 &&
               member->nick == NULL &&
               member->role_ids == NULL &&
               member->role_ids_count == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_member_t *member_copy = dcc_member_clone(member);
        if (copy != NULL &&
            copy->user.id == 446 &&
            copy->user.username != NULL &&
            strcmp(copy->user.username, "member") == 0 &&
            copy->nick == NULL &&
            copy->role_ids == NULL &&
            member_copy != NULL &&
            member_copy->user.id == 446 &&
            member_copy->user.username != NULL &&
            strcmp(member_copy->user.username, "member") == 0 &&
            member_copy->nick == NULL &&
            member_copy->role_ids == NULL) {
            state->guild_member_remove_seen = 1;
        }
        dcc_member_free(member_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_user_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_user_t *user = dcc_event_user(event);
    if (data != NULL &&
        user != NULL &&
        dcc_event_type(event) == DCC_EVENT_USER_UPDATE &&
        data->user_id == 447 &&
        data->user.id == 447 &&
        data->user.username != NULL &&
        strcmp(data->user.username, "self") == 0 &&
        data->user.global_name == NULL &&
        data->user.avatar != NULL &&
        strcmp(data->user.avatar, "user-avatar") == 0 &&
        data->user.flags == 64 &&
        data->user.discriminator == 0 &&
        data->user.bot == 0 &&
        user->id == 447 &&
        user->username != NULL &&
        strcmp(user->username, "self") == 0 &&
        user->avatar != NULL &&
        strcmp(user->avatar, "user-avatar") == 0 &&
        user->flags == 64) {
        dcc_user_t *copy = dcc_user_clone(user);
        if (copy != NULL &&
            copy->username != NULL &&
            strcmp(copy->username, "self") == 0 &&
            copy->avatar != NULL &&
            strcmp(copy->avatar, "user-avatar") == 0 &&
            copy->flags == 64) {
            state->user_update_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_USER_UPDATE);
        }
        dcc_user_free(copy);
    }
}
