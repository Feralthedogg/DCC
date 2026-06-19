#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_gateway_bot(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/gateway/bot", NULL, cb, user_data);
}
