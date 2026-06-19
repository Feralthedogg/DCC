#include "internal/voice/dcc_voice_internal.h"

void dcc_voice_client_set_self_user_id(dcc_client_t *client, dcc_snowflake_t user_id) {
    if (client != NULL && user_id != 0) {
        client->self_user_id = user_id;
    }
}

void dcc_voice_client_apply_gateway_event(dcc_client_t *client, const dcc_event_t *event) {
    if (client == NULL || event == NULL || client->voice_clients_len == 0) {
        return;
    }

    dcc_voice_client_registry_lock(client);
    if (event->type == DCC_EVENT_VOICE_STATE_UPDATE) {
        const dcc_voice_state_t *voice_state = &event->data.voice_state;
        dcc_snowflake_t self_user_id = client->self_user_id;
        if (self_user_id != 0 && voice_state->guild_id != 0 && voice_state->user_id == self_user_id) {
            for (uint32_t i = 0; i < client->voice_clients_len; ++i) {
                dcc_voice_client_t *voice_client = client->voice_clients[i];
                if (voice_client != NULL && voice_client->guild_id == voice_state->guild_id) {
                    (void)dcc_voice_client_apply_voice_state(voice_client, voice_state, self_user_id);
                }
            }
        }
    } else if (event->type == DCC_EVENT_VOICE_SERVER_UPDATE) {
        const dcc_voice_server_update_t *voice_server = &event->data.voice_server_update;
        if (voice_server->guild_id != 0) {
            for (uint32_t i = 0; i < client->voice_clients_len; ++i) {
                dcc_voice_client_t *voice_client = client->voice_clients[i];
                if (voice_client != NULL && voice_client->guild_id == voice_server->guild_id) {
                    (void)dcc_voice_client_apply_voice_server_update(voice_client, voice_server);
                }
            }
        }
    }
    dcc_voice_client_registry_unlock(client);
}
