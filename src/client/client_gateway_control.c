#include "internal/client/dcc_client_state_internal.h"
#include "internal/client/dcc_cluster_identify_coordinator_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"

#include <stddef.h>

#define DCC_GATEWAY_INFO_LEGACY_SIZE offsetof(dcc_gateway_info_t, session_id)
#define DCC_GATEWAY_INFO_HAS(out_, field_) \
    ((out_)->size >= offsetof(dcc_gateway_info_t, field_) + sizeof((out_)->field_))

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
    if (client == NULL || out == NULL || out->size < DCC_GATEWAY_INFO_LEGACY_SIZE) {
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
    if (client->gateway_identify_coordinator != NULL) {
        dcc_cluster_identify_coordinator_t *coordinator = client->gateway_identify_coordinator;
        while (atomic_flag_test_and_set_explicit(&coordinator->lock, memory_order_acquire)) {
        }
        out->remaining_identifies = coordinator->remaining;
        atomic_flag_clear_explicit(&coordinator->lock, memory_order_release);
    }
    out->total_identifies = client->gateway_session_total_identifies;
    out->reset_after_ms = client->gateway_session_reset_after_ms;
    out->reset_at_ms = client->gateway_session_reset_at_ms;
    if (DCC_GATEWAY_INFO_HAS(out, session_id)) {
        out->session_id = client->gateway_session_id != NULL ? client->gateway_session_id : "";
    }
    if (DCC_GATEWAY_INFO_HAS(out, seq)) {
        out->seq = client->gateway_seq;
    }
    if (DCC_GATEWAY_INFO_HAS(out, identify_delay_ms)) {
        out->identify_delay_ms = client->gateway_identify_delay_ms;
    }
    if (DCC_GATEWAY_INFO_HAS(out, last_status)) {
        out->last_status =
            (dcc_status_t)atomic_load_explicit(&client->gateway_last_status, memory_order_acquire);
    }
    if (DCC_GATEWAY_INFO_HAS(out, has_seq)) {
        out->has_seq = client->gateway_has_seq ? 1U : 0U;
    }
    if (DCC_GATEWAY_INFO_HAS(out, can_resume)) {
        out->can_resume = dcc_gateway_can_resume(client) ? 1U : 0U;
    }
    if (DCC_GATEWAY_INFO_HAS(out, task_running)) {
        out->task_running =
            atomic_load_explicit(&client->gateway_task_running, memory_order_acquire) ? 1U : 0U;
    }
    if (DCC_GATEWAY_INFO_HAS(out, reconnect_requested)) {
        out->reconnect_requested =
            atomic_load_explicit(&client->gateway_reconnect_requested, memory_order_acquire) ? 1U : 0U;
    }
    if (DCC_GATEWAY_INFO_HAS(out, reconnect_resume)) {
        out->reconnect_resume =
            atomic_load_explicit(&client->gateway_reconnect_resume, memory_order_acquire) ? 1U : 0U;
    }
    return DCC_OK;
}

#undef DCC_GATEWAY_INFO_HAS
#undef DCC_GATEWAY_INFO_LEGACY_SIZE
