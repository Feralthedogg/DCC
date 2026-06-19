#ifndef DCC_REST_QUERY_WEBHOOKS_INTERNAL_H
#define DCC_REST_QUERY_WEBHOOKS_INTERNAL_H

#include "internal/rest/dcc_rest_query_append_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_webhook_options_query(
    dcc_rest_buffer_t *query,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components
);

#ifdef __cplusplus
}
#endif

#endif
