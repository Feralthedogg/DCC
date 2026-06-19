#include "internal/rest/dcc_rest_async_cancel_internal.h"

size_t dcc_rest_async_cancel_pending_list(
    dcc_client_t *client,
    dcc_rest_async_request_t *request
) {
    size_t canceled = 0;
    while (request != NULL) {
        dcc_rest_async_request_t *next = request->next;
        request->next = NULL;
        if (request->cb != NULL) {
            dcc_rest_response_t response = {
                .size = sizeof(response),
                .status = 0,
                .error = DCC_ERR_CANCELED,
                .body = NULL,
                .body_len = 0,
            };
            request->cb(client, &response, request->user_data);
        }
        dcc_rest_async_request_free(request);
        canceled++;
        request = next;
    }
    return canceled;
}
