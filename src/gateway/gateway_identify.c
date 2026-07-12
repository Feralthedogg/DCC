#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_identify_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"
#include "internal/client/dcc_cluster_identify_coordinator_internal.h"

#include <llam/runtime.h>

#include <stdio.h>

dcc_gateway_next_t dcc_gateway_reconnect_next(const dcc_client_t *client) {
    return dcc_gateway_can_resume(client) ? DCC_GATEWAY_NEXT_RECONNECT_RESUME : DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
}

dcc_gateway_next_t dcc_gateway_invalid_session_next(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload
) {
    if (payload->has_invalid_session_resumable && payload->invalid_session_resumable &&
        dcc_gateway_can_resume(session->client)) {
        return DCC_GATEWAY_NEXT_RECONNECT_RESUME;
    }

    dcc_gateway_clear_resume(session->client);
    session->client->gateway_identify_delay_ms =
        1000U + dcc_gateway_now_jitter(4000U, session->client->shard_id + 1U);
    return DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
}

dcc_status_t dcc_gateway_wait_ms(dcc_client_t *client, uint64_t delay_ms) {
    uint64_t remaining = delay_ms;
    while (remaining > 0 &&
           !dcc_gateway_should_stop(client) &&
           !dcc_gateway_reconnect_requested(client)) {
        uint64_t slice = remaining > 100U ? 100U : remaining;
        (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
        remaining -= slice;
    }
    return dcc_gateway_should_stop(client) || dcc_gateway_reconnect_requested(client)
        ? DCC_ERR_CANCELED
        : DCC_OK;
}

static void dcc_gateway_refresh_identify_limit(dcc_client_t *client, uint64_t now_ms) {
    if (client->gateway_remaining_identifies != 0 ||
        client->gateway_session_reset_at_ms == 0 ||
        now_ms < client->gateway_session_reset_at_ms) {
        return;
    }

    client->gateway_remaining_identifies = client->gateway_session_total_identifies != 0
        ? client->gateway_session_total_identifies
        : 1U;
    client->gateway_session_reset_at_ms = 0;
}

static dcc_status_t dcc_gateway_wait_cluster_identify_slot(dcc_client_t *client) {
    dcc_cluster_identify_coordinator_t *coordinator = client->gateway_identify_coordinator;
    if (coordinator == NULL) return DCC_OK;
    for (;;) {
        uint64_t now_ms = dcc_gateway_now_ms();
        uint64_t delay_ms = 0U;
        while (atomic_flag_test_and_set_explicit(&coordinator->lock, memory_order_acquire)) {
        }
        if (coordinator->remaining == 0U && coordinator->reset_at_ms != 0U &&
            now_ms >= coordinator->reset_at_ms) {
            coordinator->remaining = coordinator->total != 0U ? coordinator->total : 1U;
            coordinator->reset_at_ms = 0U;
        }
        if (coordinator->remaining == 0U && coordinator->reset_at_ms > now_ms) {
            delay_ms = coordinator->reset_at_ms - now_ms;
        }
        uint32_t concurrency = coordinator->max_concurrency != 0U
            ? coordinator->max_concurrency
            : 1U;
        if (concurrency > DCC_CLUSTER_IDENTIFY_BUCKET_CAP) {
            concurrency = DCC_CLUSTER_IDENTIFY_BUCKET_CAP;
        }
        uint32_t bucket = client->shard_id % concurrency;
        if (coordinator->bucket_next_ms[bucket] > now_ms) {
            uint64_t bucket_delay = coordinator->bucket_next_ms[bucket] - now_ms;
            if (bucket_delay > delay_ms) delay_ms = bucket_delay;
        }
        if (delay_ms == 0U) {
            coordinator->bucket_next_ms[bucket] = now_ms + UINT64_C(5000);
            if (coordinator->remaining > 0U) coordinator->remaining--;
            client->gateway_remaining_identifies = coordinator->remaining;
            atomic_fetch_add_explicit(&coordinator->reservations, 1U, memory_order_relaxed);
        }
        atomic_flag_clear_explicit(&coordinator->lock, memory_order_release);
        if (delay_ms == 0U) return DCC_OK;
        atomic_fetch_add_explicit(&coordinator->waits, 1U, memory_order_relaxed);
        dcc_status_t status = dcc_gateway_wait_ms(client, delay_ms);
        if (status != DCC_OK) return status;
    }
}

dcc_status_t dcc_gateway_wait_identify_slot(dcc_client_t *client) {
    uint64_t delay_ms = client->gateway_identify_delay_ms;
    client->gateway_identify_delay_ms = 0;

    uint64_t now_ms = dcc_gateway_now_ms();
    dcc_gateway_refresh_identify_limit(client, now_ms);
    if (client->gateway_remaining_identifies == 0 &&
        client->gateway_session_reset_at_ms != 0 &&
        now_ms < client->gateway_session_reset_at_ms) {
        uint64_t reset_delay = client->gateway_session_reset_at_ms - now_ms;
        if (reset_delay > delay_ms) {
            delay_ms = reset_delay;
        }
    }

    if (delay_ms == 0) {
        return dcc_gateway_wait_cluster_identify_slot(client);
    }

    char message[128];
    int n = snprintf(
        message,
        sizeof(message),
        "gateway delaying IDENTIFY for %llums",
        (unsigned long long)delay_ms
    );
    if (n > 0 && (size_t)n < sizeof(message)) {
        dcc_emit_log(client, DCC_LOG_DEBUG, message);
    }
    dcc_status_t status = dcc_gateway_wait_ms(client, delay_ms);
    if (status == DCC_OK) {
        dcc_gateway_refresh_identify_limit(client, dcc_gateway_now_ms());
        status = dcc_gateway_wait_cluster_identify_slot(client);
    }
    return status;
}

void dcc_gateway_consume_identify(dcc_client_t *client) {
    if (client != NULL && client->gateway_identify_coordinator != NULL) {
        return;
    }
    if (client != NULL && client->gateway_remaining_identifies > 0) {
        client->gateway_remaining_identifies--;
    }
}
