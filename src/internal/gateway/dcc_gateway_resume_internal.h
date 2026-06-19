#ifndef DCC_GATEWAY_RESUME_INTERNAL_H
#define DCC_GATEWAY_RESUME_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

bool dcc_gateway_can_resume(const dcc_client_t *client);
void dcc_gateway_clear_resume(dcc_client_t *client);
dcc_status_t dcc_gateway_store_ready(
    dcc_gateway_session_t *session,
    const char *session_id,
    const char *resume_url
);

#ifdef __cplusplus
}
#endif

#endif
