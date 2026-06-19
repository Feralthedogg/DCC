#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"
#include "internal/gateway/dcc_gateway_session_lifecycle_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
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
    atomic_init(&session->stop_heartbeat, false);
    atomic_init(&session->waiting_heartbeat_ack, false);
    atomic_init(&session->heartbeat_failed, false);
}

void dcc_gateway_session_stop_heartbeat(dcc_gateway_session_t *session, llam_task_t *heartbeat) {
    if (session == NULL || heartbeat == NULL) {
        return;
    }
    atomic_store_explicit(&session->stop_heartbeat, true, memory_order_release);
    (void)llam_join(heartbeat);
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
