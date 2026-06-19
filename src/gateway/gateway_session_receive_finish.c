#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_session_lifecycle_internal.h"

dcc_status_t dcc_gateway_session_finalize_receive(
    dcc_gateway_session_t *session,
    dcc_status_t status
) {
    dcc_client_t *client = session->client;
    if (dcc_gateway_should_stop(client)) {
        session->next = DCC_GATEWAY_NEXT_STOP;
        return DCC_OK;
    }
    if (dcc_gateway_reconnect_requested(client)) {
        session->next = dcc_gateway_take_reconnect_request(client);
        return DCC_ERR_CANCELED;
    }
    if (session->heartbeat_failed) {
        session->next = dcc_gateway_reconnect_next(client);
        return DCC_ERR_DISCORD;
    }
    if (session->next == DCC_GATEWAY_NEXT_FATAL && status != DCC_OK) {
        return status;
    }
    if (session->next == DCC_GATEWAY_NEXT_FATAL) {
        session->next = dcc_gateway_reconnect_next(client);
    }
    return status;
}
