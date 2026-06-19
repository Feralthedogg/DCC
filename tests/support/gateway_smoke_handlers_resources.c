#include "gateway_smoke_handlers.h"

#include <stdio.h>
#include <string.h>

void on_poll_vote_add(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_poll_vote_t *poll_vote = dcc_event_poll_vote(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        poll_vote != NULL &&
        (type == DCC_EVENT_MESSAGE_POLL_VOTE_ADD || type == DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE) &&
        data->user_id == 446 &&
        data->channel_id == 222 &&
        data->message_id == 111 &&
        data->guild_id == 333 &&
        data->answer_id == 3 &&
        poll_vote->user_id == 446 &&
        poll_vote->channel_id == 222 &&
        poll_vote->message_id == 111 &&
        poll_vote->guild_id == 333 &&
        poll_vote->answer_id == 3) {
        dcc_poll_vote_t *copy = dcc_poll_vote_clone(poll_vote);
        if (copy != NULL &&
            copy->answer_id == 3 &&
            copy->message_id == 111) {
            if (type == DCC_EVENT_MESSAGE_POLL_VOTE_ADD) {
                state->poll_vote_seen = 1;
            } else {
                state->poll_vote_remove_seen = 1;
            }
        }
        dcc_poll_vote_free(copy);
    }
}

void on_invite_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_invite_t *invite = dcc_event_invite(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data == NULL || invite == NULL) {
        return;
    }

    if (type == DCC_EVENT_INVITE_CREATE &&
        data->channel_id == 222 &&
        data->guild_id == 333 &&
        data->code != NULL &&
        strcmp(data->code, "abc") == 0 &&
        data->user_id == 446 &&
        data->target_type == 2 &&
        data->max_age == 60 &&
        data->max_uses == 5 &&
        data->uses == 1 &&
        data->created_at != NULL &&
        data->expires_at != NULL &&
        invite->channel_id == 222 &&
        invite->guild_id == 333 &&
        invite->code != NULL &&
        strcmp(invite->code, "abc") == 0 &&
        invite->inviter.id == 446 &&
        invite->inviter.username != NULL &&
        strcmp(invite->inviter.username, "member") == 0 &&
        invite->target_type == 2 &&
        invite->max_age == 60 &&
        invite->max_uses == 5 &&
        invite->uses == 1 &&
        invite->created_at != NULL &&
        invite->expires_at != NULL) {
        dcc_invite_t *copy = dcc_invite_clone(invite);
        if (copy != NULL &&
            copy->code != NULL &&
            strcmp(copy->code, "abc") == 0 &&
            copy->inviter.username != NULL &&
            strcmp(copy->inviter.username, "member") == 0) {
            state->invite_create_seen = 1;
        }
        dcc_invite_free(copy);
    } else if (type == DCC_EVENT_INVITE_DELETE &&
               data->channel_id == 222 &&
               data->guild_id == 333 &&
               data->code != NULL &&
               strcmp(data->code, "abc") == 0 &&
               data->user_id == 0 &&
               data->target_type == 0 &&
               data->max_age == 0 &&
               data->max_uses == 0 &&
               data->uses == 0 &&
               data->created_at == NULL &&
               data->expires_at == NULL &&
               invite->channel_id == 222 &&
               invite->guild_id == 333 &&
               invite->code != NULL &&
               strcmp(invite->code, "abc") == 0 &&
               invite->inviter.id == 0 &&
               invite->inviter.username == NULL &&
               invite->target_type == 0 &&
               invite->max_age == 0 &&
               invite->max_uses == 0 &&
               invite->uses == 0 &&
               invite->created_at == NULL &&
               invite->expires_at == NULL) {
        dcc_invite_t *copy = dcc_invite_clone(invite);
        if (copy != NULL &&
            copy->code != NULL &&
            strcmp(copy->code, "abc") == 0 &&
            copy->inviter.id == 0 &&
            copy->inviter.username == NULL &&
            copy->created_at == NULL &&
            copy->expires_at == NULL) {
            state->invite_delete_seen = 1;
        }
        dcc_invite_free(copy);
    }
}

void on_voice_channel_effect_send(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_voice_channel_effect_t *effect = dcc_event_voice_channel_effect(event);
    if (data != NULL &&
        effect != NULL &&
        dcc_event_type(event) == DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND &&
        data->channel_id == 222 &&
        data->guild_id == 333 &&
        data->user_id == 446 &&
        data->emoji_id == 9999 &&
        data->emoji_name != NULL &&
        strcmp(data->emoji_name, "spark") == 0 &&
        data->animation_type == 1 &&
        data->animation_id == 1234 &&
        data->sound_id == 5678 &&
        data->volume > 0.49 &&
        data->volume < 0.51 &&
        effect->channel_id == 222 &&
        effect->guild_id == 333 &&
        effect->user_id == 446 &&
        effect->emoji_id == 9999 &&
        effect->emoji_name != NULL &&
        strcmp(effect->emoji_name, "spark") == 0 &&
        effect->animation_type == 1 &&
        effect->animation_id == 1234 &&
        effect->sound_id == 5678 &&
        effect->volume > 0.49 &&
        effect->volume < 0.51) {
        dcc_voice_channel_effect_t *copy = dcc_voice_channel_effect_clone(effect);
        if (copy != NULL &&
            copy->emoji_name != NULL &&
            strcmp(copy->emoji_name, "spark") == 0 &&
            copy->sound_id == 5678) {
            state->voice_effect_seen = 1;
        }
        dcc_voice_channel_effect_free(copy);
    }
}

void on_scheduled_event_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_scheduled_event_t *scheduled_event = dcc_event_scheduled_event(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        scheduled_event != NULL &&
        type == DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE &&
        data->scheduled_event_id == 888 &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->name != NULL &&
        strcmp(data->name, "event") == 0 &&
        data->start_time != NULL &&
        data->end_time == NULL &&
        data->privacy_level == 2 &&
        data->entity_type == 3 &&
        data->entity_id == 0 &&
        data->creator_id == 446 &&
        scheduled_event->id == 888 &&
        scheduled_event->guild_id == 333 &&
        scheduled_event->channel_id == 222 &&
        scheduled_event->name != NULL &&
        strcmp(scheduled_event->name, "event") == 0 &&
        scheduled_event->description != NULL &&
        strcmp(scheduled_event->description, "desc") == 0 &&
        scheduled_event->image != NULL &&
        strcmp(scheduled_event->image, "directhash") == 0 &&
        scheduled_event->location != NULL &&
        strcmp(scheduled_event->location, "stage") == 0 &&
        scheduled_event->creator.id == 446 &&
        scheduled_event->creator.username != NULL &&
        strcmp(scheduled_event->creator.username, "creator") == 0 &&
        scheduled_event->creator.avatar != NULL &&
        strcmp(scheduled_event->creator.avatar, "creator-avatar") == 0 &&
        scheduled_event->creator.flags == 32 &&
        scheduled_event->status == 2 &&
        scheduled_event->user_count == 7 &&
        scheduled_event->start_time != NULL &&
        scheduled_event->end_time == NULL &&
        scheduled_event->privacy_level == 2 &&
        scheduled_event->entity_type == 3 &&
        scheduled_event->entity_id == 0 &&
        scheduled_event->creator_id == 446) {
        dcc_scheduled_event_t *copy = dcc_scheduled_event_clone(scheduled_event);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "event") == 0 &&
            copy->description != NULL &&
            strcmp(copy->description, "desc") == 0 &&
            copy->image != NULL &&
            strcmp(copy->image, "directhash") == 0 &&
            copy->location != NULL &&
            strcmp(copy->location, "stage") == 0 &&
            copy->creator.username != NULL &&
            strcmp(copy->creator.username, "creator") == 0 &&
            copy->creator.avatar != NULL &&
            strcmp(copy->creator.avatar, "creator-avatar") == 0 &&
            copy->creator.flags == 32 &&
            copy->status == 2 &&
            copy->user_count == 7 &&
            copy->start_time != NULL &&
            copy->end_time == NULL) {
            state->scheduled_event_seen = 1;
        }
        dcc_scheduled_event_free(copy);
    } else if (data != NULL &&
        scheduled_event != NULL &&
        (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE || type == DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE) &&
        data->scheduled_event_id == 888 &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->name != NULL &&
        strcmp(data->name, "event") == 0 &&
        scheduled_event->id == 888 &&
        scheduled_event->guild_id == 333 &&
        scheduled_event->channel_id == 222 &&
        scheduled_event->name != NULL &&
        strcmp(scheduled_event->name, "event") == 0 &&
        (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE || scheduled_event->start_time != NULL) &&
        (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE || scheduled_event->end_time == NULL)) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_scheduled_event_t *event_copy = dcc_scheduled_event_clone(scheduled_event);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "event") == 0 &&
            event_copy != NULL &&
            event_copy->name != NULL &&
            strcmp(event_copy->name, "event") == 0 &&
            (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE || event_copy->start_time != NULL)) {
            if (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE) {
                state->scheduled_event_update_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE);
            } else {
                state->scheduled_event_delete_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE);
            }
        }
        dcc_scheduled_event_free(event_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_guild_member_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_member_t *member = dcc_event_member(event);
    if (data != NULL &&
        member != NULL &&
        dcc_event_type(event) == DCC_EVENT_GUILD_MEMBER_UPDATE &&
        data->guild_id == 333 &&
        data->user_id == 446 &&
        data->nick != NULL &&
        strcmp(data->nick, "nick") == 0 &&
        data->role_ids != NULL &&
        data->role_ids_count == 2 &&
        data->role_ids[0] == 555 &&
        data->role_ids[1] == 556 &&
        data->joined_at != NULL &&
        data->premium_since == NULL &&
        data->pending == 1 &&
        data->communication_disabled_until != NULL &&
        member->guild_id == 333 &&
        member->user.id == 446 &&
        member->user.username != NULL &&
        strcmp(member->user.username, "member") == 0 &&
        member->nick != NULL &&
        strcmp(member->nick, "nick") == 0 &&
        member->role_ids != NULL &&
        member->role_ids_count == 2 &&
        member->role_ids[0] == 555 &&
        member->role_ids[1] == 556 &&
        member->pending == 1) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_member_t *member_copy = dcc_member_clone(member);
        if (copy != NULL &&
            copy->role_ids != NULL &&
            copy->role_ids_count == 2 &&
            copy->nick != NULL &&
            strcmp(copy->nick, "nick") == 0 &&
            member_copy != NULL &&
            member_copy->role_ids != NULL &&
            member_copy->role_ids_count == 2 &&
            member_copy->nick != NULL &&
            strcmp(member_copy->nick, "nick") == 0) {
            state->guild_member_update_seen = 1;
        }
        dcc_member_free(member_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_guild_members_chunk(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_guild_members_chunk_t *chunk = dcc_event_guild_members_chunk(event);
    if (data != NULL &&
        chunk != NULL &&
        dcc_event_type(event) == DCC_EVENT_GUILD_MEMBERS_CHUNK &&
        data->guild_id == 333 &&
        data->chunk_index == 1 &&
        data->chunk_count == 2 &&
        data->member_ids != NULL &&
        data->member_ids_count == 1 &&
        data->member_ids[0] == 446 &&
        data->members != NULL &&
        data->members_count == 1 &&
        data->members[0].guild_id == 333 &&
        data->members[0].user.id == 446 &&
        data->members[0].user.username != NULL &&
        strcmp(data->members[0].user.username, "member") == 0 &&
        data->members[0].user.avatar != NULL &&
        strcmp(data->members[0].user.avatar, "member-user-avatar") == 0 &&
        data->members[0].user.flags == 64 &&
        data->members[0].role_ids != NULL &&
        data->members[0].role_ids_count == 2 &&
        data->members[0].role_ids[1] == 556 &&
        data->members[0].nick != NULL &&
        strcmp(data->members[0].nick, "nick") == 0 &&
        data->members[0].pending == 1 &&
        data->not_found_ids != NULL &&
        data->not_found_ids_count == 1 &&
        data->not_found_ids[0] == 447 &&
        data->nonce != NULL &&
        strcmp(data->nonce, "n1") == 0 &&
        chunk->guild_id == 333 &&
        chunk->chunk_index == 1 &&
        chunk->chunk_count == 2 &&
        chunk->member_ids != NULL &&
        chunk->member_ids_count == 1 &&
        chunk->member_ids[0] == 446 &&
        chunk->members != NULL &&
        chunk->members_count == 1 &&
        chunk->members[0].user.id == 446 &&
        chunk->members[0].user.username != NULL &&
        strcmp(chunk->members[0].user.username, "member") == 0 &&
        chunk->members[0].user.avatar != NULL &&
        strcmp(chunk->members[0].user.avatar, "member-user-avatar") == 0 &&
        chunk->members[0].user.flags == 64 &&
        chunk->members[0].role_ids != NULL &&
        chunk->members[0].role_ids_count == 2 &&
        chunk->members[0].pending == 1 &&
        chunk->not_found_ids != NULL &&
        chunk->not_found_ids_count == 1 &&
        chunk->not_found_ids[0] == 447 &&
        chunk->nonce != NULL &&
        strcmp(chunk->nonce, "n1") == 0) {
        dcc_guild_members_chunk_t *copy = dcc_guild_members_chunk_clone(chunk);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->not_found_ids != NULL &&
            copy->not_found_ids_count == 1 &&
            copy->not_found_ids[0] == 447 &&
            copy->members != NULL &&
            copy->members_count == 1 &&
            copy->members[0].user.username != NULL &&
            strcmp(copy->members[0].user.username, "member") == 0 &&
            copy->members[0].user.avatar != NULL &&
            strcmp(copy->members[0].user.avatar, "member-user-avatar") == 0 &&
            copy->members[0].user.flags == 64 &&
            copy->members[0].role_ids != NULL &&
            copy->members[0].role_ids_count == 2 &&
            copy->nonce != NULL &&
            strcmp(copy->nonce, "n1") == 0 &&
            data_copy != NULL &&
            data_copy->members != NULL &&
            data_copy->members_count == 1 &&
            data_copy->members[0].user.username != NULL &&
            strcmp(data_copy->members[0].user.username, "member") == 0 &&
            data_copy->members[0].user.avatar != NULL &&
            strcmp(data_copy->members[0].user.avatar, "member-user-avatar") == 0 &&
            data_copy->members[0].user.flags == 64 &&
            data_copy->members[0].role_ids != NULL &&
            data_copy->members[0].role_ids_count == 2) {
            state->members_chunk_seen = 1;
        }
        dcc_guild_members_chunk_free(copy);
        dcc_gateway_event_data_free(data_copy);
    }
}

void on_channel_pins_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_channel_pins_update_t *pins = dcc_event_channel_pins_update(event);
    if (data != NULL &&
        pins != NULL &&
        dcc_event_type(event) == DCC_EVENT_CHANNEL_PINS_UPDATE &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->timestamp_text != NULL &&
        pins->guild_id == 333 &&
        pins->channel_id == 222 &&
        pins->last_pin_timestamp != NULL) {
        dcc_channel_pins_update_t *copy = dcc_channel_pins_update_clone(pins);
        if (copy != NULL &&
            copy->last_pin_timestamp != NULL) {
            state->channel_pins_seen = 1;
        }
        dcc_channel_pins_update_free(copy);
    }
}

