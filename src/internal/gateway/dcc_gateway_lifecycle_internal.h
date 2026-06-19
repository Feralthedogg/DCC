#ifndef DCC_GATEWAY_LIFECYCLE_INTERNAL_H
#define DCC_GATEWAY_LIFECYCLE_INTERNAL_H

#include "internal/gateway/dcc_gateway_voice_state_update_internal.h"

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_start(dcc_client_t *client);
dcc_status_t dcc_gateway_request_stop(dcc_client_t *client);
dcc_status_t dcc_gateway_request_reconnect(dcc_client_t *client, uint8_t resume);
dcc_status_t dcc_gateway_fetch_bot_info(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
