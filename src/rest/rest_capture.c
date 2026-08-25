#include "internal/rest/dcc_rest_capture_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_resource_internal.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void dcc_rest_capture_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_rest_captured_response_t *captured = (dcc_rest_captured_response_t *)user_data;
    const size_t required_size = offsetof(dcc_rest_response_t, body_len) +
        sizeof(((dcc_rest_response_t *)0)->body_len);
    if (captured == NULL || captured->called || response == NULL ||
        (captured->expected_client != NULL && captured->expected_client != client) ||
        response->size < required_size ||
        (response->body_len != 0U && response->body == NULL)) {
        return;
    }

    captured->called = 1;
    captured->status = response->status;
    captured->error = response->error;
    captured->body_len = response->body_len;
    if (response->body != NULL && response->body_len != 0) {
        dcc_status_t reserve_status =
            dcc_rest_resource_reserve_response(client, response->body_len);
        if (reserve_status != DCC_OK) {
            captured->copy_error = reserve_status;
            captured->body_len = 0U;
            return;
        }
        captured->reserved_body_bytes = response->body_len;
        captured->body = (char *)malloc(response->body_len);
        if (captured->body == NULL) {
            dcc_rest_resource_release_response(
                client, captured->reserved_body_bytes);
            captured->reserved_body_bytes = 0U;
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
    dcc_rest_resource_release_response(captured->expected_client,
                                       captured->reserved_body_bytes);
    captured->body = NULL;
    captured->body_len = 0;
    captured->reserved_body_bytes = 0U;
}

void dcc_rest_forward_captured_response(
    dcc_client_t *client,
    const dcc_rest_captured_response_t *captured,
    dcc_status_t error,
    dcc_rest_cb cb,
    void *user_data
) {
    if (captured == NULL) {
        return;
    }
    dcc_rest_deliver_terminal_callback_only(
        client,
        captured->status,
        error,
        captured->body,
        captured->body_len,
        cb,
        user_data
    );
}
