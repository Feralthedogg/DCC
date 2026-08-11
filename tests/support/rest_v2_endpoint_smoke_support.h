#ifndef DCC_REST_V2_ENDPOINT_SMOKE_SUPPORT_H
#define DCC_REST_V2_ENDPOINT_SMOKE_SUPPORT_H

#include <dcc/client.h>
#include <dcc/rest/request.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

typedef struct endpoint_capture {
    atomic_uint calls;
    atomic_uint delay_ms;
    atomic_uint entered;
    atomic_uint capture_release;
    char method[16];
    char path[512];
    char content_type[256];
    unsigned char body[65536];
    size_t body_len;
} endpoint_capture_t;

typedef struct endpoint_callback {
    atomic_uint calls;
    atomic_uint published_handle;
    dcc_rest_request_t **request_slot;
    dcc_rest_result_t *result;
} endpoint_callback_t;

typedef struct endpoint_observer {
    atomic_uint calls;
} endpoint_observer_t;

typedef struct endpoint_runner {
    dcc_client_t *client;
    dcc_status_t status;
} endpoint_runner_t;

uint64_t endpoint_now_ms(void);
void endpoint_result_callback(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
);
int endpoint_contains(
    const unsigned char *haystack,
    size_t haystack_len,
    const void *needle,
    size_t needle_len
);
void endpoint_capture_reset(endpoint_capture_t *capture, uint32_t delay_ms);
int endpoint_wait_for_atomic(
    const atomic_uint *value,
    unsigned expected,
    uint32_t timeout_ms
);
void endpoint_callback_reset(endpoint_callback_t *callback);
int endpoint_completed_contract(
    const char *label,
    endpoint_capture_t *capture,
    dcc_status_t submit_status,
    dcc_rest_request_t *request,
    const char *expected_method,
    const char *expected_path,
    const char *expected_content_type,
    const void *expected_body_fragment,
    size_t expected_body_fragment_len
);

int endpoint_create_message_ownership_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback
);
int endpoint_interaction_tags_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
);
int endpoint_auto_release_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback
);
int endpoint_interaction_crud_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
);
int endpoint_webhook_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
);
int endpoint_versioned_prefix_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
);
int endpoint_nested_message_preflight_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
);
int endpoint_multipart_overflow_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
);
int endpoint_transition_submission_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
);
int endpoint_transition_rejection_contract(dcc_client_t *client);

#endif
