#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"

dcc_status_t dcc_client_gateway_start(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!client->runtime.initialized || !atomic_load_explicit(&client->started, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    if (atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    return dcc_gateway_start(client);
}

dcc_status_t dcc_client_gateway_stop(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_gateway_request_stop(client);
}

dcc_status_t dcc_client_gateway_reconnect(dcc_client_t *client, uint8_t resume) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_gateway_request_reconnect(client, resume);
}

dcc_status_t dcc_client_update_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf
) {
    if (client != NULL &&
        guild_id == 0U &&
        channel_id != 0U &&
        client->infer_guild_id_from_channel != 0U) {
        dcc_status_t status = dcc_client_infer_guild_id_from_channel(client, channel_id, &guild_id);
        if (status != DCC_OK) {
            dcc_set_error(client, "could not infer guild id from channel id");
            return status;
        }
    }
    return dcc_gateway_send_voice_state_update(client, guild_id, channel_id, self_mute, self_deaf);
}

dcc_status_t dcc_client_infer_guild_id_from_channel(
    const dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t *out_guild_id
) {
    return dcc_cache_infer_guild_id_from_channel(client, channel_id, out_guild_id);
}

dcc_status_t dcc_client_gateway_info(const dcc_client_t *client, dcc_gateway_info_t *out) {
    if (client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    const char *url = client->gateway_cached_url != NULL && client->gateway_cached_url[0] != '\0'
        ? client->gateway_cached_url
        : client->gateway_url;
    out->url = url != NULL ? url : "";
    out->resume_url = client->gateway_resume_url != NULL ? client->gateway_resume_url : "";
    out->recommended_shards = client->gateway_recommended_shards;
    out->max_concurrency = client->gateway_max_concurrency == 0 ? 1U : client->gateway_max_concurrency;
    out->remaining_identifies = client->gateway_remaining_identifies;
    out->total_identifies = client->gateway_session_total_identifies;
    out->reset_after_ms = client->gateway_session_reset_after_ms;
    out->reset_at_ms = client->gateway_session_reset_at_ms;
    return DCC_OK;
}
