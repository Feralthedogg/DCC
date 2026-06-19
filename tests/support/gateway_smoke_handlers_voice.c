#include "gateway_smoke_handlers.h"

#include <string.h>

void on_voice_server_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_voice_server_update_t *voice_server = dcc_event_voice_server_update(event);
    if (data != NULL &&
        voice_server != NULL &&
        dcc_event_type(event) == DCC_EVENT_VOICE_SERVER_UPDATE &&
        data->guild_id == 333 &&
        data->token != NULL &&
        strcmp(data->token, "voice-token") == 0 &&
        data->endpoint != NULL &&
        strcmp(data->endpoint, "voice.example.com") == 0 &&
        voice_server->guild_id == 333 &&
        voice_server->token != NULL &&
        strcmp(voice_server->token, "voice-token") == 0 &&
        voice_server->endpoint != NULL &&
        strcmp(voice_server->endpoint, "voice.example.com") == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_voice_server_update_t *voice_copy = dcc_voice_server_update_clone(voice_server);
        if (copy != NULL &&
            copy->token != NULL &&
            strcmp(copy->token, "voice-token") == 0 &&
            copy->endpoint != NULL &&
            strcmp(copy->endpoint, "voice.example.com") == 0 &&
            voice_copy != NULL &&
            voice_copy->token != NULL &&
            strcmp(voice_copy->token, "voice-token") == 0 &&
            voice_copy->endpoint != NULL &&
            strcmp(voice_copy->endpoint, "voice.example.com") == 0) {
            state->voice_server_update_seen = 1;
        }
        dcc_voice_server_update_free(voice_copy);
        dcc_gateway_event_data_free(copy);
    }
}

void on_voice_channel_start_time_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_voice_channel_start_time_update_t *update = dcc_event_voice_channel_start_time_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->channel_id == 222 &&
        update->voice_start_time == 123456789) {
        dcc_voice_channel_start_time_update_t *copy = dcc_voice_channel_start_time_update_clone(update);
        if (copy != NULL && copy->voice_start_time == 123456789) {
            state->voice_channel_start_time_update_seen = 1;
        }
        dcc_voice_channel_start_time_update_free(copy);
    }
}

void on_voice_channel_status_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_voice_channel_status_update_t *update = dcc_event_voice_channel_status_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->channel_id == 222 &&
        update->status != NULL &&
        strcmp(update->status, "live") == 0) {
        dcc_voice_channel_status_update_t *copy = dcc_voice_channel_status_update_clone(update);
        if (copy != NULL && copy->status != NULL && strcmp(copy->status, "live") == 0) {
            state->voice_channel_status_update_seen = 1;
        }
        dcc_voice_channel_status_update_free(copy);
    }
}
