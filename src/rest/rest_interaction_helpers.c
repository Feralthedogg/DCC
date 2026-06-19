#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_require_callback_fields(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t *out_interaction_id,
    const char **out_token
) {
    if (interaction == NULL || interaction->id == 0 || interaction->token == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_interaction_id != NULL) {
        *out_interaction_id = interaction->id;
    }
    if (out_token != NULL) {
        *out_token = interaction->token;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_interaction_require_webhook_fields(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t *out_application_id,
    const char **out_token
) {
    if (interaction == NULL || interaction->application_id == 0 || interaction->token == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_application_id != NULL) {
        *out_application_id = interaction->application_id;
    }
    if (out_token != NULL) {
        *out_token = interaction->token;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_interaction_webhook_path(
    char **out,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *suffix_fmt,
    dcc_snowflake_t message_id
) {
    char *token = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(interaction_token, &token);
    if (status != DCC_OK) {
        return status;
    }

    if (suffix_fmt == NULL) {
        status = dcc_rest_alloc_formatted_path(
            out,
            "/webhooks/%llu/%s",
            (unsigned long long)application_id,
            token
        );
    } else if (message_id == 0) {
        status = dcc_rest_alloc_formatted_path(
            out,
            "/webhooks/%llu/%s/%s",
            (unsigned long long)application_id,
            token,
            suffix_fmt
        );
    } else {
        status = dcc_rest_alloc_formatted_path(
            out,
            "/webhooks/%llu/%s/%s/%llu",
            (unsigned long long)application_id,
            token,
            suffix_fmt,
            (unsigned long long)message_id
        );
    }

    free(token);
    return status;
}
