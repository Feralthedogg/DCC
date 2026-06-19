#include "internal/voice/dcc_voice_internal.h"

#include <stdlib.h>

dcc_status_t dcc_voice_client_create(dcc_client_t *client, dcc_voice_client_t **out) {
    if (client == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_client_t *voice_client = (dcc_voice_client_t *)calloc(1, sizeof(*voice_client));
    if (voice_client == NULL) {
        *out = NULL;
        return DCC_ERR_NOMEM;
    }

    voice_client->udp_fd = LLAM_INVALID_FD;
    voice_client->receive_sequence = -1;
    atomic_init(&voice_client->receive_jitter_timer_running, false);
    atomic_init(&voice_client->receive_jitter_timer_stop, false);
    atomic_init(&voice_client->receive_jitter_timer_completed, true);
    atomic_flag_clear(&voice_client->receive_jitter_timer_lock);
    atomic_init(&voice_client->receive_jitter_timer_handle, (uintptr_t)0);
    atomic_init(&voice_client->websocket_task_running, false);
    atomic_init(&voice_client->websocket_task_stop, false);
    atomic_init(&voice_client->websocket_task_completed, true);
    atomic_init(&voice_client->websocket_last_status, DCC_OK);
    atomic_flag_clear(&voice_client->websocket_lock);
    dcc_status_t status = dcc_voice_client_register_owner(client, voice_client);
    if (status != DCC_OK) {
        free(voice_client);
        *out = NULL;
        return status;
    }
    *out = voice_client;
    return DCC_OK;
}

void dcc_voice_client_destroy(dcc_voice_client_t *voice_client) {
    if (voice_client != NULL) {
        (void)dcc_voice_client_stop_websocket_loop(voice_client);
        (void)dcc_voice_client_stop_receive_jitter_timer(voice_client);
        dcc_voice_client_unregister_owner(voice_client);
        dcc_voice_client_disconnect_udp(voice_client);
        dcc_voice_opus_backend_destroy(voice_client);
        (void)dcc_voice_client_clear_send_queue(voice_client);
    }
    free(voice_client);
}

dcc_client_t *dcc_voice_client_owner(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->client : NULL;
}

dcc_status_t dcc_voice_client_set_channel_id(dcc_voice_client_t *voice_client, dcc_snowflake_t channel_id) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->channel_id = channel_id;
    return DCC_OK;
}

dcc_snowflake_t dcc_voice_client_channel_id(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->channel_id : 0;
}

uint32_t dcc_voice_client_ssrc(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->ssrc : 0;
}

dcc_status_t dcc_voice_dispatch(dcc_voice_client_t *voice_client, const dcc_event_t *event) {
    if (voice_client == NULL || voice_client->client == NULL || event == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_event_bus_dispatch(&voice_client->client->events, voice_client->client, event);
}
