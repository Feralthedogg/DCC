#ifndef DCC_REST_TYPES_WEBHOOKS_H
#define DCC_REST_TYPES_WEBHOOKS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_webhook_params {
    size_t size;
    dcc_snowflake_t webhook_id;
    dcc_snowflake_t channel_id;
    const char *name;
    const char *avatar;
    const char *token;
} dcc_webhook_params_t;

#ifdef __cplusplus
}
#endif

#endif
