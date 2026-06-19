#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdio.h>

dcc_status_t dcc_gateway_send_heartbeat(dcc_gateway_session_t *session) {
    char heartbeat[128];
    int n;
    if (session->has_seq) {
        n = snprintf(
            heartbeat,
            sizeof(heartbeat),
            "{\"op\":1,\"d\":%llu}",
            (unsigned long long)session->seq
        );
    } else {
        n = snprintf(heartbeat, sizeof(heartbeat), "{\"op\":1,\"d\":null}");
    }
    if (n <= 0 || (size_t)n >= sizeof(heartbeat)) {
        return DCC_ERR_NOMEM;
    }
    return dcc_ws_send_text(session->ws, heartbeat, (size_t)n);
}
