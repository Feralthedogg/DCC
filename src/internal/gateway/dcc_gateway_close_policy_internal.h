#ifndef DCC_GATEWAY_CLOSE_POLICY_INTERNAL_H
#define DCC_GATEWAY_CLOSE_POLICY_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_gateway_next_t dcc_gateway_next_for_close(dcc_gateway_session_t *session, uint16_t code);

#ifdef __cplusplus
}
#endif

#endif
