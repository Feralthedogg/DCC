#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"

#include <stdio.h>
#include <stdlib.h>

dcc_status_t dcc_gateway_run(dcc_client_t *client) {
    bool resume = false;
    unsigned attempt = 0;
    dcc_status_t last_status = DCC_OK;

    while (!dcc_gateway_should_stop(client)) {
        if (dcc_gateway_reconnect_requested(client)) {
            resume = dcc_gateway_take_reconnect_request(client) == DCC_GATEWAY_NEXT_RECONNECT_RESUME;
            attempt = 0;
        }
        if (resume && !dcc_gateway_can_resume(client)) {
            resume = false;
        }
        if (!resume && (client->gateway_url == NULL || client->gateway_url[0] == '\0') &&
            (client->gateway_cached_url == NULL || client->gateway_cached_url[0] == '\0')) {
            last_status = dcc_gateway_fetch_bot_info(client);
            if (last_status != DCC_OK) {
                return last_status;
            }
        }

        char *url = dcc_gateway_attempt_url(client, resume);
        if (url == NULL) {
            dcc_set_error(client, "failed to allocate gateway URL");
            return DCC_ERR_NOMEM;
        }
        if (url[0] == '\0') {
            free(url);
            dcc_set_error(client, "gateway URL is empty");
            return DCC_ERR_INVALID_ARG;
        }

        dcc_gateway_next_t next = DCC_GATEWAY_NEXT_FATAL;
        last_status = dcc_gateway_run_once(client, url, resume, &next);
        free(url);

        if (next == DCC_GATEWAY_NEXT_STOP ||
            dcc_gateway_should_stop(client)) {
            return DCC_OK;
        }
        if (next == DCC_GATEWAY_NEXT_FATAL) {
            return last_status;
        }

        resume = next == DCC_GATEWAY_NEXT_RECONNECT_RESUME;
        uint32_t delay_ms = dcc_gateway_backoff_jitter_ms(
            attempt,
            ((uint64_t)client->shard_id << 32U) ^ (uint64_t)client->shard_count
        );
        attempt++;
        if (client->gateway_identify_delay_ms > delay_ms) {
            delay_ms = client->gateway_identify_delay_ms;
            client->gateway_identify_delay_ms = 0;
        }
        const char *reason = dcc_client_last_error(client);
        char log_message[256];
        int n = snprintf(
            log_message,
            sizeof(log_message),
            "gateway reconnecting via %s in %ums after %s",
            resume ? "RESUME" : "IDENTIFY",
            (unsigned)delay_ms,
            reason != NULL && reason[0] != '\0' ? reason : dcc_status_string(last_status)
        );
        if (n > 0 && (size_t)n < sizeof(log_message)) {
            dcc_emit_log(client, DCC_LOG_WARN, log_message);
        }

        if (dcc_gateway_wait_ms(client, delay_ms) == DCC_ERR_CANCELED) {
            if (dcc_gateway_should_stop(client)) {
                return DCC_OK;
            }
            if (dcc_gateway_reconnect_requested(client)) {
                resume = dcc_gateway_take_reconnect_request(client) == DCC_GATEWAY_NEXT_RECONNECT_RESUME;
                attempt = 0;
                continue;
            }
            return DCC_OK;
        }
    }

    return last_status == DCC_ERR_CANCELED ? DCC_OK : last_status;
}
