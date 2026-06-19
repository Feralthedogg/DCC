#include "internal/rest/dcc_rest_async_queue_internal.h"

size_t dcc_rest_async_pending_count_locked(const dcc_client_t *client) {
    size_t count = 0;
    for (uint32_t priority = 0; priority < DCC_REST_PRIORITY_LEVELS; ++priority) {
        const dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        while (request != NULL) {
            count++;
            request = request->next;
        }
    }
    return count;
}

dcc_rest_async_request_t *dcc_rest_async_detach_pending_all_locked(dcc_client_t *client) {
    dcc_rest_async_request_t *head = NULL;
    dcc_rest_async_request_t *tail = NULL;

    for (uint32_t priority = 0; priority < DCC_REST_PRIORITY_LEVELS; ++priority) {
        dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        client->rest_async_heads[priority] = NULL;
        client->rest_async_tails[priority] = NULL;

        while (request != NULL) {
            dcc_rest_async_request_t *next = request->next;
            request->next = NULL;
            if (tail != NULL) {
                tail->next = request;
            } else {
                head = request;
            }
            tail = request;
            request = next;
        }
    }

    return head;
}

void dcc_rest_async_pending_free_all(dcc_client_t *client) {
    dcc_rest_async_request_t *request = dcc_rest_async_detach_pending_all_locked(client);
    while (request != NULL) {
        dcc_rest_async_request_t *next = request->next;
        dcc_rest_async_request_free(request);
        request = next;
    }
}
