#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_close_policy_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"

#include <stdio.h>

dcc_gateway_next_t dcc_gateway_next_for_close(dcc_gateway_session_t *session, uint16_t code) {
    dcc_client_t *client = session->client;
    char message[128];

    if (code == 0) {
        dcc_set_error(client, "gateway transport closed without close code");
        return dcc_gateway_can_resume(client) ? DCC_GATEWAY_NEXT_RECONNECT_RESUME : DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
    }

    int n = snprintf(message, sizeof(message), "gateway websocket closed with code %u", (unsigned)code);
    if (n > 0 && (size_t)n < sizeof(message)) {
        dcc_set_error(client, message);
        dcc_emit_log(client, DCC_LOG_WARN, message);
    }

    switch (code) {
        case 1000:
        case 1001:
            dcc_gateway_clear_resume(client);
            return DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
        case 4007:
        case 4009:
            dcc_gateway_clear_resume(client);
            return DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
        case 4004:
        case 4010:
        case 4011:
        case 4012:
        case 4013:
        case 4014:
            return DCC_GATEWAY_NEXT_FATAL;
        case 4008:
            client->gateway_identify_delay_ms = 5000U;
            return dcc_gateway_can_resume(client)
                ? DCC_GATEWAY_NEXT_RECONNECT_RESUME
                : DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
        case 4000:
        case 4001:
        case 4002:
        case 4003:
        case 4005:
        default:
            return dcc_gateway_can_resume(client)
                ? DCC_GATEWAY_NEXT_RECONNECT_RESUME
                : DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
    }
}
