#ifndef DCC_REST_CAPTURE_INTERNAL_H
#define DCC_REST_CAPTURE_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_captured_response {
    int called;
    uint16_t status;
    dcc_status_t error;
    dcc_status_t copy_error;
    char *body;
    size_t body_len;
} dcc_rest_captured_response_t;

void dcc_rest_capture_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
);
void dcc_rest_captured_response_deinit(dcc_rest_captured_response_t *captured);
void dcc_rest_forward_captured_response(
    dcc_client_t *client,
    const dcc_rest_captured_response_t *captured,
    dcc_status_t error,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
