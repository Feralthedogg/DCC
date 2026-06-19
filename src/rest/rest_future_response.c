#include "internal/rest/dcc_rest_future_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_rest_future_store_response(
    dcc_rest_future_t *future,
    const dcc_rest_response_t *response
) {
    if (future == NULL || response == NULL) {
        return;
    }

    future->response.size = sizeof(future->response);
    future->response.status = response->status;
    future->response.error = response->error;
    future->response.body = NULL;
    future->response.body_len = 0;
    free(future->body);
    future->body = NULL;
    future->body_len = 0;

    if (response->body == NULL || response->body_len == 0) {
        return;
    }
    if (response->body_len == SIZE_MAX) {
        future->response.error = DCC_ERR_NOMEM;
        return;
    }

    future->body = (char *)malloc(response->body_len + 1U);
    if (future->body == NULL) {
        future->response.error = DCC_ERR_NOMEM;
        return;
    }

    memcpy(future->body, response->body, response->body_len);
    future->body[response->body_len] = '\0';
    future->body_len = response->body_len;
    future->response.body = future->body;
    future->response.body_len = future->body_len;
}
