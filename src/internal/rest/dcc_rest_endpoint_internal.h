#ifndef DCC_REST_ENDPOINT_INTERNAL_H
#define DCC_REST_ENDPOINT_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest.h>

typedef struct dcc_endpoint_body {
    char *data;
    size_t len;
    const char *content_type;
} dcc_endpoint_body_t;

typedef struct dcc_endpoint_record_view {
    size_t size;
    uint32_t version;
    uint64_t present;
} dcc_endpoint_record_view_t;

typedef struct dcc_endpoint_interaction_view {
    dcc_endpoint_record_view_t record;
    dcc_interaction_response_type_t type;
    uint8_t with_response;
    dcc_rest_interaction_response_data_t data;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
} dcc_endpoint_interaction_view_t;

typedef struct dcc_endpoint_webhook_execute_view {
    dcc_endpoint_record_view_t record;
    const dcc_message_builder_t *message;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
    const char *username;
    const char *avatar_url;
    const char *thread_name;
    const dcc_snowflake_t *applied_tag_ids;
    size_t applied_tag_count;
    uint8_t wait;
    dcc_snowflake_t thread_id;
    uint8_t with_components;
} dcc_endpoint_webhook_execute_view_t;

typedef struct dcc_endpoint_webhook_edit_view {
    dcc_endpoint_record_view_t record;
    const dcc_rest_message_payload_t *payload;
    dcc_snowflake_t thread_id;
    uint8_t with_components;
} dcc_endpoint_webhook_edit_view_t;

#define DCC_ENDPOINT_FIELD_END(type_, field_) \
    (offsetof(type_, field_) + sizeof(((type_ *)0)->field_))

int dcc_endpoint_field_covered(size_t size, size_t offset, size_t width);
dcc_status_t dcc_endpoint_record_read(
    const void *value,
    size_t version_offset,
    size_t present_offset,
    size_t mandatory_size,
    uint32_t expected_version,
    uint64_t known_present,
    dcc_endpoint_record_view_t *out
);
int dcc_endpoint_present_field_covered(
    const dcc_endpoint_record_view_t *view,
    uint64_t bit,
    size_t offset,
    size_t width
);

dcc_status_t dcc_endpoint_message_payload_preflight(
    const dcc_rest_message_payload_t *payload
);
dcc_status_t dcc_endpoint_message_list_preflight(
    const dcc_rest_message_list_query_t *query,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_id_page_preflight(
    const dcc_rest_id_page_t *page,
    uint64_t allowed_present,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_pin_page_preflight(
    const dcc_rest_pin_page_t *page,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_reaction_query_preflight(
    const dcc_rest_reaction_query_t *query,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_interaction_response_preflight(
    const dcc_rest_interaction_response_t *response,
    dcc_endpoint_interaction_view_t *out
);
dcc_status_t dcc_endpoint_webhook_builder_preflight(
    const dcc_rest_webhook_builder_t *builder,
    int create,
    int token_authenticated,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_webhook_execute_preflight(
    const dcc_rest_webhook_execute_t *execute,
    dcc_endpoint_webhook_execute_view_t *out
);
dcc_status_t dcc_endpoint_webhook_message_query_preflight(
    const dcc_rest_webhook_message_query_t *query,
    dcc_endpoint_record_view_t *out
);
dcc_status_t dcc_endpoint_webhook_message_edit_preflight(
    const dcc_rest_webhook_message_edit_t *edit,
    dcc_endpoint_webhook_edit_view_t *out
);
dcc_status_t dcc_endpoint_webhook_compat_preflight(
    const dcc_rest_webhook_compat_payload_t *payload,
    const void **out_body,
    size_t *out_body_len
);

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

/* Transition adapter for public APIs that still expose dcc_rest_cb. The
 * descriptor submission copies path/query/body before this function returns;
 * accepted callbacks are therefore always delivered by the REST worker. */
dcc_status_t dcc_endpoint_submit_legacy_raw(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *content_type,
    const void *body,
    size_t body_len,
    dcc_rest_cb callback,
    void *user_data
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
dcc_status_t dcc_endpoint_measure_interaction_message_json(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    size_t *out_json_len
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
dcc_status_t dcc_endpoint_measure_webhook_execute_json(
    const dcc_endpoint_webhook_execute_view_t *view,
    size_t *out_json_len
);

void dcc_endpoint_body_deinit(dcc_endpoint_body_t *body);

dcc_status_t dcc_endpoint_legacy_options(
    dcc_rest_cb callback,
    void *user_data,
    dcc_rest_call_options_t *out_options,
    void **out_bridge
);

void dcc_endpoint_legacy_bridge_release(void *bridge);

/* Test-only construction-allocation probe. It is thread-local and inert until
 * armed, so production endpoint behavior is unchanged. */
void dcc_endpoint_test_allocation_probe_begin(size_t successful_allocations);
size_t dcc_endpoint_test_allocation_probe_end(void);
dcc_status_t dcc_endpoint_allocation_probe(void);

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
