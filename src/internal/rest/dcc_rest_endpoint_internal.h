#ifndef DCC_REST_ENDPOINT_INTERNAL_H
#define DCC_REST_ENDPOINT_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest.h>

typedef struct dcc_endpoint_body {
    char *data;
    size_t len;
    const char *content_type;
} dcc_endpoint_body_t;

dcc_status_t dcc_endpoint_prepare(
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request,
    dcc_rest_call_options_t *out_options
);

dcc_status_t dcc_endpoint_submit(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_endpoint_body_t *body,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

dcc_status_t dcc_endpoint_path_with_query(
    const char *path,
    const dcc_rest_buffer_t *query,
    char **out_path
);

dcc_status_t dcc_endpoint_build_message_body(
    const dcc_rest_message_payload_t *payload,
    dcc_endpoint_body_t *out_body
);

dcc_status_t dcc_endpoint_build_interaction_body(
    const dcc_rest_interaction_response_t *response,
    dcc_endpoint_body_t *out_body
);

dcc_status_t dcc_endpoint_build_webhook_builder_body(
    const dcc_rest_webhook_builder_t *builder,
    int create,
    int token_authenticated,
    dcc_endpoint_body_t *out_body
);

dcc_status_t dcc_endpoint_build_webhook_execute_body(
    const dcc_rest_webhook_execute_t *execute,
    dcc_endpoint_body_t *out_body
);

void dcc_endpoint_body_deinit(dcc_endpoint_body_t *body);

dcc_status_t dcc_endpoint_legacy_options(
    dcc_rest_cb callback,
    void *user_data,
    dcc_rest_call_options_t *out_options,
    void **out_bridge
);

void dcc_endpoint_legacy_bridge_release(void *bridge);

#define DCC_ENDPOINT_LEGACY_RETURN(callback_, user_data_, endpoint_, ...) \
    do { \
        dcc_rest_call_options_t dcc_endpoint_legacy_call_options; \
        void *dcc_endpoint_legacy_call_bridge = NULL; \
        dcc_status_t dcc_endpoint_legacy_call_status = \
            dcc_endpoint_legacy_options( \
                (callback_), (user_data_), \
                &dcc_endpoint_legacy_call_options, \
                &dcc_endpoint_legacy_call_bridge \
            ); \
        if (dcc_endpoint_legacy_call_status == DCC_OK) { \
            dcc_endpoint_legacy_call_status = (endpoint_)( \
                __VA_ARGS__, &dcc_endpoint_legacy_call_options, NULL \
            ); \
        } \
        if (dcc_endpoint_legacy_call_status != DCC_OK) { \
            dcc_endpoint_legacy_bridge_release(dcc_endpoint_legacy_call_bridge); \
        } \
        return dcc_endpoint_legacy_call_status; \
    } while (0)

#endif
