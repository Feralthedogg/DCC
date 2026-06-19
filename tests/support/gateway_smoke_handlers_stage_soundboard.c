#include "gateway_smoke_handlers.h"

#include <string.h>

void on_stage_instance_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_stage_instance_t *stage_instance = dcc_event_stage_instance(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        stage_instance != NULL &&
        type == DCC_EVENT_STAGE_INSTANCE_CREATE &&
        data->id == 900 &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->topic != NULL &&
        strcmp(data->topic, "town hall") == 0 &&
        data->privacy_level == 2 &&
        data->discoverable_disabled == 1 &&
        data->scheduled_event_id == 888 &&
        stage_instance->id == 900 &&
        stage_instance->guild_id == 333 &&
        stage_instance->channel_id == 222 &&
        stage_instance->topic != NULL &&
        strcmp(stage_instance->topic, "town hall") == 0 &&
        stage_instance->privacy_level == 2 &&
        stage_instance->discoverable_disabled == 1 &&
        stage_instance->scheduled_event_id == 888) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_stage_instance_t *stage_copy = dcc_stage_instance_clone(stage_instance);
        if (copy != NULL &&
            copy->topic != NULL &&
            strcmp(copy->topic, "town hall") == 0 &&
            stage_copy != NULL &&
            stage_copy->topic != NULL &&
            strcmp(stage_copy->topic, "town hall") == 0) {
            state->stage_instance_seen = 1;
        }
        dcc_stage_instance_free(stage_copy);
        dcc_gateway_event_data_free(copy);
    } else if (data != NULL &&
        stage_instance != NULL &&
        (type == DCC_EVENT_STAGE_INSTANCE_UPDATE || type == DCC_EVENT_STAGE_INSTANCE_DELETE) &&
        data->id == 900 &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->topic != NULL &&
        strcmp(data->topic, "town hall") == 0 &&
        stage_instance->id == 900 &&
        stage_instance->guild_id == 333 &&
        stage_instance->channel_id == 222 &&
        stage_instance->topic != NULL &&
        strcmp(stage_instance->topic, "town hall") == 0 &&
        (type == DCC_EVENT_STAGE_INSTANCE_DELETE || stage_instance->privacy_level == 2) &&
        (type == DCC_EVENT_STAGE_INSTANCE_DELETE || stage_instance->discoverable_disabled == 1)) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_stage_instance_t *stage_copy = dcc_stage_instance_clone(stage_instance);
        if (copy != NULL &&
            copy->topic != NULL &&
            strcmp(copy->topic, "town hall") == 0 &&
            stage_copy != NULL &&
            stage_copy->topic != NULL &&
            strcmp(stage_copy->topic, "town hall") == 0) {
            if (type == DCC_EVENT_STAGE_INSTANCE_UPDATE) {
                state->stage_instance_update_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_STAGE_INSTANCE_UPDATE);
            } else {
                state->stage_instance_delete_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_STAGE_INSTANCE_DELETE);
            }
        }
        dcc_stage_instance_free(stage_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_soundboard_sound_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_soundboard_sound_t *sound = dcc_event_soundboard_sound(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data == NULL || sound == NULL) {
        return;
    }

    if (type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE &&
        sound != NULL &&
        data->sound_id == 5678 &&
        data->guild_id == 333 &&
        data->name != NULL &&
        strcmp(data->name, "spark") == 0 &&
        data->volume > 0.74 &&
        data->volume < 0.76 &&
        data->emoji_id == 9999 &&
        data->emoji_name != NULL &&
        strcmp(data->emoji_name, "spark") == 0 &&
        data->available == 1 &&
        data->user_id == 446 &&
        sound->id == 5678 &&
        sound->guild_id == 333 &&
        sound->name != NULL &&
        strcmp(sound->name, "spark") == 0 &&
        sound->volume > 0.74 &&
        sound->volume < 0.76 &&
        sound->emoji_id == 9999 &&
        sound->emoji_name != NULL &&
        strcmp(sound->emoji_name, "spark") == 0 &&
        sound->available == 1 &&
        sound->user_id == 446 &&
        sound->user.id == 446 &&
        sound->user.username != NULL &&
        strcmp(sound->user.username, "member") == 0 &&
        sound->user.avatar != NULL &&
        strcmp(sound->user.avatar, "member-avatar") == 0 &&
        sound->user.flags == 64) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_soundboard_sound_t *sound_copy = dcc_soundboard_sound_clone(sound);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "spark") == 0 &&
            copy->emoji_name != NULL &&
            strcmp(copy->emoji_name, "spark") == 0 &&
            sound_copy != NULL &&
            sound_copy->name != NULL &&
            strcmp(sound_copy->name, "spark") == 0 &&
            sound_copy->emoji_name != NULL &&
            strcmp(sound_copy->emoji_name, "spark") == 0 &&
            sound_copy->user.username != NULL &&
            strcmp(sound_copy->user.username, "member") == 0 &&
            sound_copy->user.avatar != NULL &&
            strcmp(sound_copy->user.avatar, "member-avatar") == 0 &&
            sound_copy->user.flags == 64) {
            state->soundboard_sound_seen = 1;
        }
        dcc_soundboard_sound_free(sound_copy);
        dcc_gateway_event_data_free(copy);
    } else if (type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE &&
        data->sound_id == 5678 &&
        data->guild_id == 333 &&
        data->name != NULL &&
        strcmp(data->name, "spark") == 0 &&
        data->volume > 0.74 &&
        data->volume < 0.76 &&
        data->emoji_id == 9999 &&
        data->emoji_name != NULL &&
        strcmp(data->emoji_name, "spark") == 0 &&
        data->available == 1 &&
        sound->id == 5678 &&
        sound->guild_id == 333 &&
        sound->name != NULL &&
        strcmp(sound->name, "spark") == 0 &&
        sound->volume > 0.74 &&
        sound->volume < 0.76 &&
        sound->emoji_id == 9999 &&
        sound->emoji_name != NULL &&
        strcmp(sound->emoji_name, "spark") == 0 &&
        sound->available == 1) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_soundboard_sound_t *sound_copy = dcc_soundboard_sound_clone(sound);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "spark") == 0 &&
            copy->emoji_name != NULL &&
            strcmp(copy->emoji_name, "spark") == 0 &&
            sound_copy != NULL &&
            sound_copy->name != NULL &&
            strcmp(sound_copy->name, "spark") == 0 &&
            sound_copy->emoji_name != NULL &&
            strcmp(sound_copy->emoji_name, "spark") == 0) {
            state->soundboard_sound_update_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE);
        }
        dcc_soundboard_sound_free(sound_copy);
        dcc_gateway_event_data_free(copy);
    } else if (type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE &&
        data->sound_id == 5678 &&
        data->guild_id == 333 &&
        sound->id == 5678 &&
        sound->guild_id == 333 &&
        sound->name == NULL) {
        dcc_soundboard_sound_t *sound_copy = dcc_soundboard_sound_clone(sound);
        if (sound_copy != NULL &&
            sound_copy->id == 5678 &&
            sound_copy->guild_id == 333 &&
            sound_copy->name == NULL) {
            state->soundboard_sound_delete_seen = 1;
            gateway_state_mark_typed_event(state, DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE);
        }
        dcc_soundboard_sound_free(sound_copy);
    }
}

void on_entitlement_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_entitlement_t *entitlement = dcc_event_entitlement(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        entitlement != NULL &&
        type == DCC_EVENT_ENTITLEMENT_CREATE &&
        data->entitlement_id == 700 &&
        data->sku_id == 123 &&
        data->application_id == 666 &&
        data->subscription_id == 800 &&
        data->promotion_id == 900 &&
        data->gift_code_flags == 3 &&
        data->flags == 2 &&
        data->user_id == 446 &&
        data->guild_id == 333 &&
        data->type == 8 &&
        data->start_time != NULL &&
        data->end_time == NULL &&
        entitlement->id == 700 &&
        entitlement->sku_id == 123 &&
        entitlement->application_id == 666 &&
        entitlement->subscription_id == 800 &&
        entitlement->promotion_id == 900 &&
        entitlement->gift_code_flags == 3 &&
        entitlement->flags == 2 &&
        entitlement->user_id == 446 &&
        entitlement->guild_id == 333 &&
        entitlement->type == 8 &&
        entitlement->start_time != NULL &&
        entitlement->end_time == NULL) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_entitlement_t *entitlement_copy = dcc_entitlement_clone(entitlement);
        if (copy != NULL &&
            copy->subscription_id == 800 &&
            copy->promotion_id == 900 &&
            copy->gift_code_flags == 3 &&
            copy->flags == 2 &&
            copy->start_time != NULL &&
            copy->end_time == NULL &&
            entitlement_copy != NULL &&
            entitlement_copy->subscription_id == 800 &&
            entitlement_copy->promotion_id == 900 &&
            entitlement_copy->gift_code_flags == 3 &&
            entitlement_copy->flags == 2 &&
            entitlement_copy->start_time != NULL &&
            entitlement_copy->end_time == NULL) {
            state->entitlement_create_seen = 1;
        }
        dcc_entitlement_free(entitlement_copy);
        dcc_gateway_event_data_free(copy);
    } else if (data != NULL &&
        entitlement != NULL &&
        (type == DCC_EVENT_ENTITLEMENT_UPDATE || type == DCC_EVENT_ENTITLEMENT_DELETE) &&
        data->entitlement_id == 700 &&
        data->sku_id == 123 &&
        data->application_id == 666 &&
        data->user_id == 446 &&
        data->guild_id == 333 &&
        data->type == 8 &&
        entitlement->id == 700 &&
        entitlement->sku_id == 123 &&
        entitlement->application_id == 666 &&
        entitlement->user_id == 446 &&
        entitlement->guild_id == 333 &&
        entitlement->type == 8) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_entitlement_t *entitlement_copy = dcc_entitlement_clone(entitlement);
        if (copy != NULL &&
            copy->entitlement_id == 700 &&
            copy->sku_id == 123 &&
            entitlement_copy != NULL &&
            entitlement_copy->id == 700 &&
            entitlement_copy->sku_id == 123 &&
            entitlement_copy->application_id == 666) {
            if (type == DCC_EVENT_ENTITLEMENT_UPDATE) {
                state->entitlement_update_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_ENTITLEMENT_UPDATE);
            } else {
                state->entitlement_delete_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_ENTITLEMENT_DELETE);
            }
        }
        dcc_entitlement_free(entitlement_copy);
        dcc_gateway_event_data_free(copy);
    }
}
