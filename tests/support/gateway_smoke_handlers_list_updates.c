#include "gateway_smoke_handlers.h"

#include <string.h>

void on_guild_emojis_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_guild_id_list_update_t *update = dcc_event_guild_id_list_update(event);
    if (data != NULL &&
        update != NULL &&
        dcc_event_type(event) == DCC_EVENT_GUILD_EMOJIS_UPDATE &&
        data->guild_id == 333 &&
        data->ids != NULL &&
        data->ids_count == 2 &&
        data->ids[0] == 9999 &&
        data->ids[1] == 10000 &&
        update->guild_id == 333 &&
        update->ids != NULL &&
        update->ids_count == 2 &&
        update->ids[0] == 9999 &&
        update->ids[1] == 10000 &&
        update->emojis != NULL &&
        update->emojis_count == 2 &&
        update->emojis[0].id == 9999 &&
        update->emojis[0].name != NULL &&
        strcmp(update->emojis[0].name, "spark") == 0 &&
        update->emojis[0].user.id == 446 &&
        update->emojis[0].user.avatar != NULL &&
        strcmp(update->emojis[0].user.avatar, "emoji-user-avatar") == 0 &&
        update->emojis[0].user.flags == 128 &&
        update->emojis[1].id == 10000 &&
        update->emojis[1].name != NULL &&
        strcmp(update->emojis[1].name, "wave") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_guild_id_list_update_t *update_copy = dcc_guild_id_list_update_clone(update);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids_count == 2 &&
            copy->ids[0] == 9999 &&
            copy->ids[1] == 10000 &&
            update_copy != NULL &&
            update_copy->ids != NULL &&
            update_copy->ids_count == 2 &&
            update_copy->ids[1] == 10000 &&
            update_copy->emojis != NULL &&
            update_copy->emojis_count == 2 &&
            update_copy->emojis[0].name != NULL &&
            strcmp(update_copy->emojis[0].name, "spark") == 0 &&
            update_copy->emojis[0].user.avatar != NULL &&
            strcmp(update_copy->emojis[0].user.avatar, "emoji-user-avatar") == 0 &&
            update_copy->emojis[0].user.flags == 128) {
            state->emojis_update_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_EMOJIS_UPDATE);
        }
        dcc_guild_id_list_update_free(update_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_thread_list_sync(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_thread_list_sync_t *sync = dcc_event_thread_list_sync(event);
    if (data != NULL &&
        sync != NULL &&
        dcc_event_type(event) == DCC_EVENT_THREAD_LIST_SYNC &&
        data->guild_id == 333 &&
        data->channel_ids != NULL &&
        data->channel_ids_count == 1 &&
        data->channel_ids[0] == 222 &&
        data->ids != NULL &&
        data->ids_count == 1 &&
        data->ids[0] == 888 &&
        data->member_ids != NULL &&
        data->member_ids_count == 1 &&
        data->member_ids[0] == 446 &&
        sync->guild_id == 333 &&
        sync->channel_ids != NULL &&
        sync->channel_ids_count == 1 &&
        sync->channel_ids[0] == 222 &&
        sync->thread_ids != NULL &&
        sync->thread_ids_count == 1 &&
        sync->thread_ids[0] == 888 &&
        sync->member_ids != NULL &&
        sync->member_ids_count == 1 &&
        sync->member_ids[0] == 446 &&
        sync->threads != NULL &&
        sync->threads_count == 1 &&
        sync->threads[0].id == 888 &&
        sync->threads[0].guild_id == 333 &&
        sync->members != NULL &&
        sync->members_count == 1 &&
        sync->members[0].id == 888 &&
        sync->members[0].user_id == 446) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_thread_list_sync_t *sync_copy = dcc_thread_list_sync_clone(sync);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids_count == 1 &&
            copy->member_ids != NULL &&
            copy->member_ids_count == 1 &&
            copy->member_ids[0] == 446 &&
            sync_copy != NULL &&
            sync_copy->thread_ids != NULL &&
            sync_copy->thread_ids_count == 1 &&
            sync_copy->thread_ids[0] == 888 &&
            sync_copy->member_ids != NULL &&
            sync_copy->member_ids_count == 1 &&
            sync_copy->member_ids[0] == 446 &&
            sync_copy->threads != NULL &&
            sync_copy->threads_count == 1 &&
            sync_copy->threads[0].id == 888 &&
            sync_copy->members != NULL &&
            sync_copy->members_count == 1 &&
            sync_copy->members[0].user_id == 446) {
            state->thread_list_sync_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_THREAD_LIST_SYNC);
        }
        dcc_thread_list_sync_free(sync_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_soundboard_sounds_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_guild_id_list_update_t *update = dcc_event_guild_id_list_update(event);
    if (data != NULL &&
        update != NULL &&
        dcc_event_type(event) == DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE &&
        data->guild_id == 333 &&
        data->ids != NULL &&
        data->ids_count == 2 &&
        data->ids[0] == 5678 &&
        data->ids[1] == 5679 &&
        update->guild_id == 333 &&
        update->ids != NULL &&
        update->ids_count == 2 &&
        update->ids[0] == 5678 &&
        update->ids[1] == 5679 &&
        update->soundboard_sounds != NULL &&
        update->soundboard_sounds_count == 2 &&
        update->soundboard_sounds[0].id == 5678 &&
        update->soundboard_sounds[0].name != NULL &&
        strcmp(update->soundboard_sounds[0].name, "spark") == 0 &&
        update->soundboard_sounds[0].user_id == 446 &&
        update->soundboard_sounds[0].user.avatar != NULL &&
        strcmp(update->soundboard_sounds[0].user.avatar, "sound-user-avatar") == 0 &&
        update->soundboard_sounds[0].user.flags == 512 &&
        update->soundboard_sounds[1].id == 5679 &&
        update->soundboard_sounds[1].name != NULL &&
        strcmp(update->soundboard_sounds[1].name, "wave") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_guild_id_list_update_t *update_copy = dcc_guild_id_list_update_clone(update);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids_count == 2 &&
            copy->ids[0] == 5678 &&
            copy->ids[1] == 5679 &&
            update_copy != NULL &&
            update_copy->ids != NULL &&
            update_copy->ids_count == 2 &&
            update_copy->ids[0] == 5678 &&
            update_copy->soundboard_sounds != NULL &&
            update_copy->soundboard_sounds_count == 2 &&
            update_copy->soundboard_sounds[0].name != NULL &&
            strcmp(update_copy->soundboard_sounds[0].name, "spark") == 0 &&
            update_copy->soundboard_sounds[0].user.avatar != NULL &&
            strcmp(update_copy->soundboard_sounds[0].user.avatar, "sound-user-avatar") == 0 &&
            update_copy->soundboard_sounds[0].user.flags == 512) {
            state->soundboard_sounds_update_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE);
        }
        dcc_guild_id_list_update_free(update_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_guild_stickers_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_id_list_update_t *update = dcc_event_guild_id_list_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->ids != NULL &&
        update->ids_count == 1 &&
        update->ids[0] == 9010 &&
        update->stickers != NULL &&
        update->stickers_count == 1 &&
        update->stickers[0].id == 9010 &&
        update->stickers[0].name != NULL &&
        strcmp(update->stickers[0].name, "sticker") == 0 &&
        update->stickers[0].user.id == 446 &&
        update->stickers[0].user.avatar != NULL &&
        strcmp(update->stickers[0].user.avatar, "sticker-user-avatar") == 0 &&
        update->stickers[0].user.flags == 256) {
        dcc_guild_id_list_update_t *copy = dcc_guild_id_list_update_clone(update);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids_count == 1 &&
            copy->ids[0] == 9010 &&
            copy->stickers != NULL &&
            copy->stickers_count == 1 &&
            copy->stickers[0].name != NULL &&
            strcmp(copy->stickers[0].name, "sticker") == 0 &&
            copy->stickers[0].user.avatar != NULL &&
            strcmp(copy->stickers[0].user.avatar, "sticker-user-avatar") == 0 &&
            copy->stickers[0].user.flags == 256) {
            state->guild_stickers_update_seen = 1;
        }
        dcc_guild_id_list_update_free(copy);
    }
}

void on_soundboard_sounds(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_id_list_update_t *update = dcc_event_guild_id_list_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->ids != NULL &&
        update->ids_count == 1 &&
        update->ids[0] == 5678 &&
        update->soundboard_sounds != NULL &&
        update->soundboard_sounds_count == 1 &&
        update->soundboard_sounds[0].id == 5678 &&
        update->soundboard_sounds[0].name != NULL &&
        strcmp(update->soundboard_sounds[0].name, "spark") == 0) {
        dcc_guild_id_list_update_t *copy = dcc_guild_id_list_update_clone(update);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids[0] == 5678 &&
            copy->soundboard_sounds != NULL &&
            copy->soundboard_sounds[0].name != NULL &&
            strcmp(copy->soundboard_sounds[0].name, "spark") == 0) {
            state->soundboard_sounds_seen = 1;
        }
        dcc_guild_id_list_update_free(copy);
    }
}
