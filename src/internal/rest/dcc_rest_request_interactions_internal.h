#ifndef DCC_REST_REQUEST_INTERACTIONS_INTERNAL_H
#define DCC_REST_REQUEST_INTERACTIONS_INTERNAL_H

#include <dcc/rest.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_interaction_require_callback_fields(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t *out_interaction_id,
    const char **out_token
);
dcc_status_t dcc_rest_interaction_require_webhook_fields(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t *out_application_id,
    const char **out_token
);
dcc_status_t dcc_rest_interaction_webhook_path(
    char **out,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *suffix_fmt,
    dcc_snowflake_t message_id
);

#ifdef __cplusplus
}
#endif

#endif
