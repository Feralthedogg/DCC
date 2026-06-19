#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"
#include "internal/gateway/dcc_gateway_session_lifecycle_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

void dcc_gateway_session_init(dcc_gateway_session_t *session, dcc_client_t *client, bool resume) {
    if (session == NULL) {
        return;
    }

    memset(session, 0, sizeof(*session));
    session->client = client;
    session->seq = client->gateway_seq;
    session->has_seq = client->gateway_has_seq;
    session->resume = resume && dcc_gateway_can_resume(client);
    session->next = DCC_GATEWAY_NEXT_FATAL;
    session->heartbeat = NULL;
    session->heartbeat_failed = false;
}

dcc_status_t dcc_gateway_session_start_heartbeat(dcc_gateway_session_t *session, llam_task_t **out_heartbeat) {
    if (session == NULL || session->ws == NULL || out_heartbeat == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_gateway_heartbeat_state_t *state =
        (dcc_gateway_heartbeat_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (dcc_ws_retain(session->ws) != DCC_OK) {
        free(state);
        return DCC_ERR_STATE;
    }

    state->ws = session->ws;
    state->interval_ms = session->heartbeat_interval_ms;
    state->jitter_seed = session->client->shard_id + 1U;
    atomic_init(&state->refs, 1U);
    atomic_init(&state->stop, false);
    atomic_init(&state->waiting_ack, false);
    atomic_init(&state->failed, false);
    atomic_init(&state->has_seq, session->has_seq);
    atomic_init(&state->seq, (unsigned long long)session->seq);

    atomic_fetch_add_explicit(&state->refs, 1U, memory_order_relaxed);
    llam_task_t *heartbeat = llam_spawn(dcc_gateway_heartbeat_task, state, NULL);
    if (heartbeat == NULL) {
        dcc_gateway_heartbeat_state_release(state);
        dcc_gateway_heartbeat_state_release(state);
        return DCC_ERR_RUNTIME;
    }

    session->heartbeat = state;
    *out_heartbeat = heartbeat;
    return DCC_OK;
}

void dcc_gateway_session_stop_heartbeat(dcc_gateway_session_t *session, llam_task_t *heartbeat) {
    if (session == NULL || heartbeat == NULL) {
        return;
    }
    dcc_gateway_heartbeat_state_t *state = session->heartbeat;
    if (state == NULL) {
        (void)llam_join(heartbeat);
        return;
    }

    session->heartbeat_failed = atomic_load_explicit(&state->failed, memory_order_acquire);
    atomic_store_explicit(&state->stop, true, memory_order_release);
    (void)llam_join(heartbeat);
    if (atomic_load_explicit(&state->failed, memory_order_acquire)) {
        session->heartbeat_failed = true;
    }
    dcc_gateway_heartbeat_state_release(state);
    session->heartbeat = NULL;
}

void dcc_gateway_session_drop_ws(dcc_gateway_session_t *session) {
    if (session == NULL || session->ws == NULL) {
        return;
    }
    dcc_gateway_clear_ws(session->client, session->ws);
    dcc_ws_destroy(session->ws);
    session->ws = NULL;
}

void dcc_gateway_session_finish_ws(dcc_gateway_session_t *session) {
    if (session == NULL || session->ws == NULL) {
        return;
    }
    dcc_gateway_clear_ws(session->client, session->ws);
    if (session->next == DCC_GATEWAY_NEXT_STOP) {
        dcc_ws_close(session->ws, 1000, "done");
    } else {
        dcc_ws_abort(session->ws);
    }
    dcc_ws_destroy(session->ws);
    session->ws = NULL;
}
