#include "internal/rest/dcc_rest_async_queue_internal.h"

void dcc_rest_async_push_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request) {
    request->active_next = client->rest_async_active_head;
    client->rest_async_active_head = request;
}

void dcc_rest_async_remove_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request) {
    dcc_rest_async_request_t **cursor = &client->rest_async_active_head;
    while (*cursor != NULL) {
        if (*cursor == request) {
            *cursor = request->active_next;
            request->active_next = NULL;
            return;
        }
        cursor = &(*cursor)->active_next;
    }
}
