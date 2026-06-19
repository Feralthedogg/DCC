#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_register_owner(dcc_client_t *client, dcc_voice_client_t *voice_client) {
    if (client == NULL || voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_client_registry_lock(client);
    for (uint32_t i = 0; i < client->voice_clients_len; ++i) {
        if (client->voice_clients[i] == voice_client) {
            dcc_voice_client_registry_unlock(client);
            return DCC_OK;
        }
    }
    if (client->voice_clients_len >= DCC_VOICE_CLIENT_REGISTRY_CAP) {
        dcc_voice_client_registry_unlock(client);
        return DCC_ERR_NOMEM;
    }

    client->voice_clients[client->voice_clients_len++] = voice_client;
    voice_client->client = client;
    dcc_voice_client_registry_unlock(client);
    return DCC_OK;
}

void dcc_voice_client_unregister_owner(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->client == NULL) {
        return;
    }

    (void)dcc_voice_client_stop_websocket_loop(voice_client);

    dcc_client_t *client = voice_client->client;
    dcc_voice_client_registry_lock(client);
    for (uint32_t i = 0; i < client->voice_clients_len; ++i) {
        if (client->voice_clients[i] == voice_client) {
            client->voice_clients[i] = client->voice_clients[client->voice_clients_len - 1U];
            client->voice_clients[client->voice_clients_len - 1U] = NULL;
            client->voice_clients_len--;
            break;
        }
    }
    voice_client->client = NULL;
    dcc_voice_client_registry_unlock(client);
}

void dcc_voice_client_stop_owned(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }

    dcc_voice_client_t *clients[DCC_VOICE_CLIENT_REGISTRY_CAP];
    uint32_t len = 0;

    dcc_voice_client_registry_lock(client);
    for (uint32_t i = 0; i < client->voice_clients_len && len < DCC_VOICE_CLIENT_REGISTRY_CAP; ++i) {
        if (client->voice_clients[i] != NULL) {
            clients[len++] = client->voice_clients[i];
        }
    }
    dcc_voice_client_registry_unlock(client);

    for (uint32_t i = 0; i < len; ++i) {
        (void)dcc_voice_client_stop_websocket_loop(clients[i]);
    }
}

void dcc_voice_client_unbind_owner(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }

    dcc_voice_client_stop_owned(client);

    dcc_voice_client_registry_lock(client);
    for (uint32_t i = 0; i < client->voice_clients_len; ++i) {
        if (client->voice_clients[i] != NULL) {
            client->voice_clients[i]->client = NULL;
            client->voice_clients[i] = NULL;
        }
    }
    client->voice_clients_len = 0;
    dcc_voice_client_registry_unlock(client);
}
