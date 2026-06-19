#include "internal/rest/dcc_rest_capture_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_rest_capture_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_rest_captured_response_t *captured = (dcc_rest_captured_response_t *)user_data;
    if (captured == NULL || response == NULL) {
        return;
    }

    captured->called = 1;
    captured->status = response->status;
    captured->error = response->error;
    captured->body_len = response->body_len;
    if (response->body != NULL && response->body_len != 0) {
        captured->body = (char *)malloc(response->body_len);
        if (captured->body == NULL) {
            captured->copy_error = DCC_ERR_NOMEM;
            captured->body_len = 0;
            return;
        }
        memcpy(captured->body, response->body, response->body_len);
    }
}

void dcc_rest_captured_response_deinit(dcc_rest_captured_response_t *captured) {
    if (captured == NULL) {
        return;
    }
    free(captured->body);
    captured->body = NULL;
    captured->body_len = 0;
}

void dcc_rest_forward_captured_response(
    dcc_client_t *client,
    const dcc_rest_captured_response_t *captured,
    dcc_status_t error,
    dcc_rest_cb cb,
    void *user_data
) {
    if (cb == NULL || captured == NULL) {
        return;
    }

    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = captured->status,
        .error = error,
        .body = captured->body,
        .body_len = captured->body_len,
    };
    cb(client, &response, user_data);
}
