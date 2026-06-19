#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_bot_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"

#include <stdint.h>
#include <stdlib.h>

dcc_status_t dcc_gateway_apply_bot_info(dcc_client_t *client, const dcc_json_t *root) {
    if (client == NULL || root == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const char *raw_gateway_url = dcc_json_string(dcc_json_object_get(root, "url"));
    if (raw_gateway_url == NULL || raw_gateway_url[0] == '\0') {
        dcc_set_error(client, "gateway bot REST missing URL");
        return DCC_ERR_DISCORD;
    }

    char *gateway_url = NULL;
    dcc_status_t status = dcc_gateway_url_with_json_query(raw_gateway_url, &gateway_url);
    if (status == DCC_OK) {
        free(client->gateway_cached_url);
        client->gateway_cached_url = gateway_url;
        gateway_url = NULL;
    }

    uint64_t value = 0;
    if (dcc_json_u64(dcc_json_object_get(root, "shards"), &value) == 0 &&
        value > 0 && value <= UINT32_MAX) {
        client->gateway_recommended_shards = (uint32_t)value;
    }

    const dcc_json_t *session_start_limit = dcc_json_object_get(root, "session_start_limit");
    if (session_start_limit != NULL &&
        dcc_json_u64(dcc_json_object_get(session_start_limit, "total"), &value) == 0 &&
        value <= UINT32_MAX) {
        client->gateway_session_total_identifies = (uint32_t)value;
    }
    if (session_start_limit != NULL &&
        dcc_json_u64(dcc_json_object_get(session_start_limit, "max_concurrency"), &value) == 0 &&
        value > 0 && value <= UINT32_MAX) {
        client->gateway_max_concurrency = (uint32_t)value;
    }
    if (session_start_limit != NULL &&
        dcc_json_u64(dcc_json_object_get(session_start_limit, "remaining"), &value) == 0 &&
        value <= UINT32_MAX) {
        client->gateway_remaining_identifies = (uint32_t)value;
    }
    if (session_start_limit != NULL &&
        dcc_json_u64(dcc_json_object_get(session_start_limit, "reset_after"), &value) == 0) {
        client->gateway_session_reset_after_ms = value;
        uint64_t now_ms = dcc_gateway_now_ms();
        client->gateway_session_reset_at_ms = value > UINT64_MAX - now_ms ? UINT64_MAX : now_ms + value;
    }

    return status;
}
