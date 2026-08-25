#ifndef DCC_REST_RESOURCES_GATEWAY_H
#define DCC_REST_RESOURCES_GATEWAY_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_gateway_bot(
    dcc_client_t *client,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
