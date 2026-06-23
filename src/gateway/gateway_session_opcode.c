#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/gateway/dcc_gateway_identify_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_session_receive_internal.h"

#include <stdio.h>

static void dcc_gateway_update_seq(dcc_gateway_session_t *session, const dcc_json_gateway_payload_t *payload) {
    if (payload->has_seq) {
        session->seq = payload->seq;
        session->has_seq = true;
        session->client->gateway_seq = payload->seq;
        session->client->gateway_has_seq = true;
        if (session->heartbeat != NULL) {
            atomic_store_explicit(&session->heartbeat->seq, (unsigned long long)payload->seq, memory_order_release);
            atomic_store_explicit(&session->heartbeat->has_seq, true, memory_order_release);
        }
    }
}

dcc_status_t dcc_gateway_session_handle_payload(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_client_t *client = session->client;
    dcc_status_t status = DCC_OK;

    dcc_gateway_update_seq(session, payload);
    if (!payload->has_op) {
        return DCC_OK;
    }

    if (payload->op == 0) {
        status = dcc_gateway_handle_dispatch(session, payload, raw_json, raw_json_len);
        if (status != DCC_OK) {
            session->next = DCC_GATEWAY_NEXT_FATAL;
        }
    } else if (payload->op == 1) {
        if (session->heartbeat != NULL) {
            /* Mark before send so a fast ACK cannot be overwritten after send returns. */
            atomic_store_explicit(&session->heartbeat->waiting_ack, true, memory_order_release);
        }
        status = dcc_gateway_send_heartbeat(session);
        if (status != DCC_OK) {
            session->next = dcc_gateway_reconnect_next(client);
        }
    } else if (payload->op == 7) {
        dcc_emit_log(client, DCC_LOG_WARN, "gateway requested reconnect");
        dcc_set_error(client, "gateway requested reconnect");
        status = DCC_ERR_DISCORD;
        session->next = dcc_gateway_reconnect_next(client);
    } else if (payload->op == 9) {
        session->next = dcc_gateway_invalid_session_next(session, payload);
        char message[192];
        int n = snprintf(
            message,
            sizeof(message),
            "gateway invalid session: discord_resumable=%s cached_resume=%s seq=%s next=%s",
            payload->has_invalid_session_resumable
                ? (payload->invalid_session_resumable ? "yes" : "no")
                : "unknown",
            dcc_gateway_can_resume(client) ? "yes" : "no",
            client->gateway_has_seq ? "set" : "unset",
            session->next == DCC_GATEWAY_NEXT_RECONNECT_RESUME ? "resume" : "identify"
        );
        if (n > 0 && (size_t)n < sizeof(message)) {
            dcc_emit_log(client, DCC_LOG_WARN, message);
        }
        dcc_set_error(client, session->next == DCC_GATEWAY_NEXT_RECONNECT_RESUME
            ? "gateway invalid session, resuming"
            : "gateway invalid session, identifying");
        status = DCC_ERR_DISCORD;
    } else if (payload->op == 11) {
        if (session->heartbeat != NULL) {
            atomic_store_explicit(&session->heartbeat->waiting_ack, false, memory_order_release);
        }
        status = DCC_OK;
    }

    return status;
}
