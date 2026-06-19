#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/gateway/dcc_gateway_identify_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_session_lifecycle_internal.h"
#include "internal/gateway/dcc_gateway_session_receive_internal.h"
#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>

dcc_status_t dcc_gateway_run_once(
    dcc_client_t *client,
    const char *url,
    bool resume,
    dcc_gateway_next_t *next
) {
    dcc_gateway_session_t session;
    dcc_gateway_session_init(&session, client, resume);

    dcc_status_t status = dcc_ws_connect(&session.ws, url, 30000);
    if (status != DCC_OK) {
        dcc_set_error(client, "gateway websocket connect failed");
        *next = dcc_gateway_reconnect_next(client);
        return status;
    }
    dcc_gateway_bind_ws(client, session.ws);

    status = dcc_gateway_read_hello(&session);
    if (status != DCC_OK) {
        if (dcc_gateway_should_stop(client)) {
            status = DCC_OK;
            *next = DCC_GATEWAY_NEXT_STOP;
        } else if (dcc_gateway_reconnect_requested(client)) {
            *next = dcc_gateway_take_reconnect_request(client);
        } else {
            *next = dcc_gateway_reconnect_next(client);
        }
        dcc_gateway_session_drop_ws(&session);
        return status;
    }

    llam_task_t *heartbeat = llam_spawn(dcc_gateway_heartbeat_task, &session, NULL);
    if (heartbeat == NULL) {
        dcc_gateway_session_drop_ws(&session);
        dcc_set_error(client, "failed to spawn gateway heartbeat");
        *next = DCC_GATEWAY_NEXT_FATAL;
        return DCC_ERR_RUNTIME;
    }

    if (!session.resume) {
        status = dcc_gateway_wait_identify_slot(client);
        if (status != DCC_OK) {
            dcc_gateway_session_stop_heartbeat(&session, heartbeat);
            dcc_gateway_session_drop_ws(&session);
            if (dcc_gateway_should_stop(client)) {
                *next = DCC_GATEWAY_NEXT_STOP;
            } else if (dcc_gateway_reconnect_requested(client)) {
                *next = dcc_gateway_take_reconnect_request(client);
            } else {
                *next = DCC_GATEWAY_NEXT_STOP;
            }
            return DCC_OK;
        }
    }

    status = session.resume ? dcc_gateway_send_resume(&session) : dcc_gateway_send_identify(&session);
    if (status != DCC_OK) {
        dcc_gateway_session_stop_heartbeat(&session, heartbeat);
        dcc_gateway_session_drop_ws(&session);
        dcc_set_error(client, session.resume ? "gateway RESUME failed" : "gateway IDENTIFY failed");
        if (dcc_gateway_should_stop(client)) {
            *next = DCC_GATEWAY_NEXT_STOP;
            return DCC_OK;
        }
        if (dcc_gateway_reconnect_requested(client)) {
            *next = dcc_gateway_take_reconnect_request(client);
            return status;
        }
        *next = dcc_gateway_reconnect_next(client);
        return status;
    }
    if (!session.resume) {
        dcc_gateway_consume_identify(client);
    }

    status = dcc_gateway_session_receive_loop(&session);

    dcc_gateway_session_stop_heartbeat(&session, heartbeat);
    status = dcc_gateway_session_finalize_receive(&session, status);
    dcc_gateway_session_finish_ws(&session);

    *next = session.next;
    return status;
}
