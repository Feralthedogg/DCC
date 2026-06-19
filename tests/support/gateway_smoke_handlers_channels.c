#include "gateway_smoke_handlers.h"

#include <string.h>

void on_voice_state_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_voice_state_t *voice_state = dcc_event_voice_state(event);
    if (state != NULL &&
        state->voice_client != NULL &&
        voice_state != NULL &&
        voice_state->guild_id == 333 &&
        voice_state->user_id == 444) {
        dcc_voice_session_info_t info = {
            .size = sizeof(info),
        };
        if (dcc_voice_client_session_info(state->voice_client, &info) == DCC_OK) {
            if (voice_state->channel_id == 222 &&
                info.state == DCC_VOICE_SESSION_READY &&
                info.ready &&
                info.guild_id == 333 &&
                info.channel_id == 222 &&
                info.session_id != NULL &&
                strcmp(info.session_id, "voice-session") == 0 &&
                info.token != NULL &&
                strcmp(info.token, "voice-token") == 0 &&
                info.endpoint != NULL &&
                strcmp(info.endpoint, "voice.example.com") == 0) {
                state->voice_auto_ready_seen = 1;
            } else if (voice_state->channel_id == 0 &&
                       info.state == DCC_VOICE_SESSION_DISCONNECTED &&
                       !info.ready &&
                       info.guild_id == 0 &&
                       info.channel_id == 0) {
                state->voice_auto_disconnected_seen = 1;
            }
        }
    }
    if (voice_state != NULL &&
        voice_state->guild_id == 333 &&
        voice_state->channel_id == 0 &&
        voice_state->user_id == 444 &&
        voice_state->session_id != NULL &&
        strcmp(voice_state->session_id, "voice-session") == 0 &&
        voice_state->request_to_speak_timestamp != NULL &&
        strcmp(voice_state->request_to_speak_timestamp, "2026-06-15T03:00:00.000000+00:00") == 0 &&
        voice_state->deaf == 0 &&
        voice_state->mute == 1 &&
        voice_state->self_deaf == 0 &&
        voice_state->self_mute == 1 &&
        voice_state->self_stream == 0 &&
        voice_state->self_video == 0 &&
        voice_state->suppress == 1) {
        dcc_voice_state_t *copy = dcc_voice_state_clone(voice_state);
        if (copy != NULL &&
            copy->session_id != NULL &&
            strcmp(copy->session_id, "voice-session") == 0 &&
            copy->request_to_speak_timestamp != NULL &&
            strcmp(copy->request_to_speak_timestamp, "2026-06-15T03:00:00.000000+00:00") == 0) {
            state->voice_state_seen = 1;
        }
        dcc_voice_state_free(copy);
    }
}

void on_channel_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_channel_t *channel = dcc_event_channel(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (raw != NULL &&
        data != NULL &&
        channel != NULL &&
        type == DCC_EVENT_CHANNEL_CREATE &&
        raw->type == DCC_EVENT_CHANNEL_CREATE &&
        raw->sequence == 9 &&
        data->id == 888 &&
        data->guild_id == 333 &&
        data->type == 0 &&
        data->name != NULL &&
        strcmp(data->name, "general") == 0 &&
        data->topic != NULL &&
        strcmp(data->topic, "chat") == 0 &&
        channel->id == 888 &&
        channel->guild_id == 333 &&
        channel->parent_id == 222 &&
        channel->last_message_id == 111 &&
        channel->type == 0 &&
        channel->name != NULL &&
        strcmp(channel->name, "general") == 0 &&
        channel->topic != NULL &&
        strcmp(channel->topic, "chat") == 0 &&
        channel->position == 2 &&
        channel->nsfw == 1 &&
        channel->rate_limit_per_user == 5 &&
        channel->bitrate == 64000 &&
        channel->user_limit == 25 &&
        raw->name != NULL &&
        strcmp(raw->name, "CHANNEL_CREATE") == 0 &&
        raw->json != NULL &&
        strstr(raw->json, "\"name\":\"general\"") != NULL) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_channel_t *channel_copy = dcc_channel_clone(channel);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "general") == 0 &&
            channel_copy != NULL &&
            channel_copy->name != NULL &&
            strcmp(channel_copy->name, "general") == 0 &&
            channel_copy->topic != NULL &&
            strcmp(channel_copy->topic, "chat") == 0 &&
            channel_copy->rate_limit_per_user == 5) {
            state->channel_create_seen = 1;
        }
        dcc_channel_free(channel_copy);
        dcc_gateway_event_data_free(copy);
    } else if (raw != NULL &&
               data != NULL &&
               channel != NULL &&
               (type == DCC_EVENT_CHANNEL_UPDATE ||
                type == DCC_EVENT_CHANNEL_DELETE ||
                type == DCC_EVENT_CHANNEL_INFO ||
                type == DCC_EVENT_THREAD_CREATE ||
                type == DCC_EVENT_THREAD_UPDATE ||
                type == DCC_EVENT_THREAD_DELETE ||
                type == DCC_EVENT_CHANNEL_TOPIC_UPDATE) &&
               raw->type == type &&
               data->id == (type == DCC_EVENT_THREAD_CREATE ||
                            type == DCC_EVENT_THREAD_UPDATE ||
                            type == DCC_EVENT_THREAD_DELETE ? 888U : 222U) &&
               data->guild_id == 333 &&
               channel->id == data->id &&
               channel->guild_id == data->guild_id) {
        int ok = 0;
        switch (type) {
            case DCC_EVENT_CHANNEL_UPDATE:
            case DCC_EVENT_CHANNEL_DELETE:
                ok = data->type == 0 &&
                    data->name != NULL &&
                    strcmp(data->name, "general") == 0 &&
                    data->topic == NULL &&
                    data->parent_id == 0 &&
                    channel->type == 0 &&
                    channel->name != NULL &&
                    strcmp(channel->name, "general") == 0 &&
                    channel->topic == NULL &&
                    channel->parent_id == 0;
                break;
            case DCC_EVENT_CHANNEL_INFO:
                ok = data->topic != NULL &&
                    strcmp(data->topic, "general info") == 0 &&
                    data->name == NULL &&
                    data->type == 0 &&
                    channel->topic != NULL &&
                    strcmp(channel->topic, "general info") == 0 &&
                    channel->name == NULL &&
                    channel->type == 0;
                break;
            case DCC_EVENT_THREAD_CREATE:
            case DCC_EVENT_THREAD_UPDATE:
            case DCC_EVENT_THREAD_DELETE:
                ok = data->type == 11 &&
                    data->name != NULL &&
                    strcmp(data->name, "thread") == 0 &&
                    data->parent_id == 222 &&
                    channel->type == 11 &&
                    channel->name != NULL &&
                    strcmp(channel->name, "thread") == 0 &&
                    channel->parent_id == 222 &&
                    channel->topic == NULL;
                break;
            case DCC_EVENT_CHANNEL_TOPIC_UPDATE:
                ok = data->topic != NULL &&
                    strcmp(data->topic, "updated topic") == 0 &&
                    data->name == NULL &&
                    channel->topic != NULL &&
                    strcmp(channel->topic, "updated topic") == 0 &&
                    channel->name == NULL;
                break;
            default:
                break;
        }
        if (ok) {
            dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
            dcc_channel_t *channel_copy = dcc_channel_clone(channel);
            if (copy != NULL &&
                channel_copy != NULL &&
                copy->id == data->id &&
                channel_copy->id == channel->id &&
                channel_copy->guild_id == channel->guild_id) {
                switch (type) {
                    case DCC_EVENT_CHANNEL_UPDATE:
                        if (copy->name != NULL &&
                            strcmp(copy->name, "general") == 0 &&
                            channel_copy->name != NULL &&
                            strcmp(channel_copy->name, "general") == 0) {
                            state->channel_update_seen = 1;
                        }
                        break;
                    case DCC_EVENT_CHANNEL_DELETE:
                        if (copy->name != NULL &&
                            strcmp(copy->name, "general") == 0 &&
                            channel_copy->name != NULL &&
                            strcmp(channel_copy->name, "general") == 0) {
                            state->channel_delete_seen = 1;
                        }
                        break;
                    case DCC_EVENT_CHANNEL_INFO:
                        if (copy->topic != NULL &&
                            strcmp(copy->topic, "general info") == 0 &&
                            channel_copy->topic != NULL &&
                            strcmp(channel_copy->topic, "general info") == 0) {
                            state->channel_info_seen = 1;
                        }
                        break;
                    case DCC_EVENT_THREAD_CREATE:
                        if (copy->parent_id == 222 &&
                            channel_copy->parent_id == 222 &&
                            channel_copy->name != NULL &&
                            strcmp(channel_copy->name, "thread") == 0) {
                            state->thread_create_seen = 1;
                        }
                        break;
                    case DCC_EVENT_THREAD_UPDATE:
                        if (copy->parent_id == 222 &&
                            channel_copy->parent_id == 222 &&
                            channel_copy->name != NULL &&
                            strcmp(channel_copy->name, "thread") == 0) {
                            state->thread_update_seen = 1;
                        }
                        break;
                    case DCC_EVENT_THREAD_DELETE:
                        if (copy->parent_id == 222 &&
                            channel_copy->parent_id == 222 &&
                            channel_copy->name != NULL &&
                            strcmp(channel_copy->name, "thread") == 0) {
                            state->thread_delete_seen = 1;
                        }
                        break;
                    case DCC_EVENT_CHANNEL_TOPIC_UPDATE:
                        if (copy->topic != NULL &&
                            strcmp(copy->topic, "updated topic") == 0 &&
                            channel_copy->topic != NULL &&
                            strcmp(channel_copy->topic, "updated topic") == 0) {
                            state->channel_topic_update_seen = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
            dcc_channel_free(channel_copy);
            dcc_gateway_event_data_free(copy);
        }
    }
}

void on_subscription_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_subscription_t *subscription = dcc_event_subscription(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (raw != NULL &&
        data != NULL &&
        subscription != NULL &&
        type == DCC_EVENT_SUBSCRIPTION_CREATE &&
        raw->type == DCC_EVENT_SUBSCRIPTION_CREATE &&
        raw->sequence == 10 &&
        data->id == 999 &&
        data->user_id == 444 &&
        data->sku_id == 123 &&
        data->sku_ids != NULL &&
        data->sku_ids_count == 2 &&
        data->sku_ids[0] == 123 &&
        data->sku_ids[1] == 124 &&
        data->entitlement_ids != NULL &&
        data->entitlement_ids_count == 1 &&
        data->entitlement_ids[0] == 700 &&
        data->current_period_start != NULL &&
        data->current_period_end != NULL &&
        data->canceled_at == NULL &&
        data->status != NULL &&
        strcmp(data->status, "1") == 0 &&
        subscription->id == 999 &&
        subscription->user_id == 444 &&
        subscription->sku_ids != NULL &&
        subscription->sku_ids_count == 2 &&
        subscription->sku_ids[0] == 123 &&
        subscription->sku_ids[1] == 124 &&
        subscription->entitlement_ids != NULL &&
        subscription->entitlement_ids_count == 1 &&
        subscription->entitlement_ids[0] == 700 &&
        subscription->current_period_start != NULL &&
        subscription->current_period_end != NULL &&
        subscription->canceled_at == NULL &&
        subscription->status != NULL &&
        strcmp(subscription->status, "1") == 0 &&
        raw->name != NULL &&
        strcmp(raw->name, "SUBSCRIPTION_CREATE") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_subscription_t *subscription_copy = dcc_subscription_clone(subscription);
        if (copy != NULL &&
            copy->sku_ids != NULL &&
            copy->sku_ids_count == 2 &&
            copy->entitlement_ids != NULL &&
            copy->entitlement_ids_count == 1 &&
            copy->current_period_start != NULL &&
            copy->status != NULL &&
            strcmp(copy->status, "1") == 0 &&
            subscription_copy != NULL &&
            subscription_copy->sku_ids != NULL &&
            subscription_copy->sku_ids_count == 2 &&
            subscription_copy->entitlement_ids != NULL &&
            subscription_copy->entitlement_ids_count == 1 &&
            subscription_copy->current_period_start != NULL &&
            subscription_copy->status != NULL &&
            strcmp(subscription_copy->status, "1") == 0) {
            state->subscription_create_seen = 1;
        }
        dcc_subscription_free(subscription_copy);
        dcc_gateway_event_data_free(copy);
    } else if (data != NULL &&
        subscription != NULL &&
        (type == DCC_EVENT_SUBSCRIPTION_UPDATE || type == DCC_EVENT_SUBSCRIPTION_DELETE) &&
        data->id == 999 &&
        data->user_id == 446 &&
        data->sku_ids != NULL &&
        data->sku_ids_count == 1 &&
        data->sku_ids[0] == 123 &&
        data->entitlement_ids != NULL &&
        data->entitlement_ids_count == 1 &&
        data->entitlement_ids[0] == 700 &&
        data->status != NULL &&
        strcmp(data->status, type == DCC_EVENT_SUBSCRIPTION_UPDATE ? "1" : "2") == 0 &&
        subscription->id == 999 &&
        subscription->user_id == 446 &&
        subscription->sku_ids != NULL &&
        subscription->sku_ids_count == 1 &&
        subscription->sku_ids[0] == 123 &&
        subscription->entitlement_ids != NULL &&
        subscription->entitlement_ids_count == 1 &&
        subscription->entitlement_ids[0] == 700 &&
        subscription->status != NULL &&
        strcmp(subscription->status, type == DCC_EVENT_SUBSCRIPTION_UPDATE ? "1" : "2") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_subscription_t *subscription_copy = dcc_subscription_clone(subscription);
        if (copy != NULL &&
            copy->sku_ids != NULL &&
            copy->sku_ids_count == 1 &&
            copy->entitlement_ids != NULL &&
            copy->entitlement_ids_count == 1 &&
            copy->status != NULL &&
            strcmp(copy->status, type == DCC_EVENT_SUBSCRIPTION_UPDATE ? "1" : "2") == 0 &&
            subscription_copy != NULL &&
            subscription_copy->sku_ids != NULL &&
            subscription_copy->sku_ids_count == 1 &&
            subscription_copy->entitlement_ids != NULL &&
            subscription_copy->entitlement_ids_count == 1 &&
            subscription_copy->status != NULL &&
            strcmp(subscription_copy->status, type == DCC_EVENT_SUBSCRIPTION_UPDATE ? "1" : "2") == 0) {
            if (type == DCC_EVENT_SUBSCRIPTION_UPDATE) {
                state->subscription_update_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_SUBSCRIPTION_UPDATE);
            } else {
                state->subscription_delete_seen = 1;
                gateway_state_mark_typed_event(state, DCC_EVENT_SUBSCRIPTION_DELETE);
            }
        }
        dcc_subscription_free(subscription_copy);
        dcc_gateway_event_data_free(copy);
    }
}
