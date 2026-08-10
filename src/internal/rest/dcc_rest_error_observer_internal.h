#ifndef DCC_REST_ERROR_OBSERVER_INTERNAL_H
#define DCC_REST_ERROR_OBSERVER_INTERNAL_H

#include <dcc/client.h>
#include <dcc/rest.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_terminal_completion {
    const char *operation;
    dcc_status_t transport_status;
    uint16_t http_status;
    dcc_status_t legacy_error;
    const char *body;
    size_t body_len;
    uint64_t retry_after_ms;
} dcc_rest_terminal_completion_t;

dcc_status_t dcc_rest_set_app_error_sink(
    dcc_client_t *client,
    dcc_client_error_fn handler,
    void *user_data
);

void dcc_rest_clear_app_error_sink_and_wait(dcc_client_t *client);
uint8_t dcc_rest_terminal_callback_active(const dcc_client_t *client);
void dcc_rest_terminal_wait(dcc_client_t *client);

void dcc_rest_deliver_terminal(
    dcc_client_t *client,
    const dcc_rest_terminal_completion_t *completion,
    dcc_rest_cb callback,
    void *callback_user_data
);

/* Deliver an already-observed composed response to its public callback while
 * retaining the same terminal-lifetime guard as a normal REST completion. */
void dcc_rest_deliver_terminal_callback_only(
    dcc_client_t *client,
    uint16_t http_status,
    dcc_status_t legacy_error,
    const char *body,
    size_t body_len,
    dcc_rest_cb callback,
    void *callback_user_data
);

#ifdef __cplusplus
}
#endif

#endif
