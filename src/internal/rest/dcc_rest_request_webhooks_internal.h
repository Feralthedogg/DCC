#ifndef DCC_REST_REQUEST_WEBHOOKS_INTERNAL_H
#define DCC_REST_REQUEST_WEBHOOKS_INTERNAL_H

#include <dcc/rest.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_webhook_token_path(
    char **out,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *suffix,
    dcc_snowflake_t message_id
);

#ifdef __cplusplus
}
#endif

#endif
