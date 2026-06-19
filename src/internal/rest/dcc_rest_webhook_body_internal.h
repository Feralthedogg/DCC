#ifndef DCC_REST_WEBHOOK_BODY_INTERNAL_H
#define DCC_REST_WEBHOOK_BODY_INTERNAL_H

#include <dcc/rest/types/webhooks.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_webhook_body(
    const dcc_webhook_params_t *params,
    int include_channel_id,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
