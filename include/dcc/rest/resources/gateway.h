#ifndef DCC_REST_RESOURCES_GATEWAY_H
#define DCC_REST_RESOURCES_GATEWAY_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_gateway_bot(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
