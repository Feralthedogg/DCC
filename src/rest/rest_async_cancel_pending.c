#include "internal/rest/dcc_rest_async_cancel_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"

size_t dcc_rest_async_cancel_pending_list(
    dcc_client_t *client,
    dcc_rest_async_request_t *request
) {
    size_t canceled = 0;
    while (request != NULL) {
        dcc_rest_async_request_t *next = request->next;
        request->next = NULL;
        dcc_rest_terminal_completion_t completion = {
            .operation = request->operation,
            .transport_status = DCC_ERR_CANCELED,
            .legacy_error = DCC_ERR_CANCELED,
        };
        if (request->request_handle != NULL) {
            dcc_rest_request_handle_finalize(request->request_handle, &completion);
        } else {
            dcc_rest_deliver_terminal(client, &completion, request->cb, request->user_data);
        }
        dcc_rest_async_request_free(request);
        canceled++;
        request = next;
    }
    return canceled;
}
