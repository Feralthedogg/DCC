#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_webhooks_internal.h"
#include "internal/rest/dcc_rest_request_webhooks_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_webhook_token_path(
    char **out,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *suffix,
    dcc_snowflake_t message_id
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    char *token = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(webhook_token, &token);
    if (status != DCC_OK) {
        return status;
    }

    if (suffix == NULL) {
        status = dcc_rest_alloc_formatted_path(
            out,
            "/webhooks/%llu/%s",
            (unsigned long long)webhook_id,
            token
        );
    } else {
        status = dcc_rest_alloc_formatted_path(
            out,
            "/webhooks/%llu/%s/%s/%llu",
            (unsigned long long)webhook_id,
            token,
            suffix,
            (unsigned long long)message_id
        );
    }

    free(token);
    return status;
}

dcc_status_t dcc_rest_webhook_options_query(
    dcc_rest_buffer_t *query,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components
) {
    dcc_status_t status = dcc_rest_query_append_bool_true(query, "wait", wait);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(query, "thread_id", thread_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_bool_true(query, "with_components", with_components);
    }
    return status;
}
