#ifndef DCC_REST_BASE_H
#define DCC_REST_BASE_H

#include <dcc/application_command.h>
#include <dcc/autocomplete.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/objects.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_client dcc_client_t;
typedef struct dcc_rest_future dcc_rest_future_t;

typedef enum dcc_rest_method {
    DCC_REST_GET = 0,
    DCC_REST_POST,
    DCC_REST_PUT,
    DCC_REST_PATCH,
    DCC_REST_DELETE
} dcc_rest_method_t;

typedef enum dcc_rest_priority {
    DCC_REST_PRIORITY_LOW = 0,
    DCC_REST_PRIORITY_NORMAL = 1,
    DCC_REST_PRIORITY_HIGH = 2
} dcc_rest_priority_t;

typedef struct dcc_rest_response {
    size_t size;
    uint16_t status;
    dcc_status_t error;
    const char *body;
    size_t body_len;
} dcc_rest_response_t;

typedef void (*dcc_rest_cb)(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);

typedef struct dcc_rest_async_status {
    size_t size;
    size_t pending;
    size_t active;
    size_t active_routes;
    uint32_t concurrency;
    size_t pending_low;
    size_t pending_normal;
    size_t pending_high;
    size_t pending_ready;
    size_t pending_blocked_by_active_route;
    size_t active_capacity;
    size_t active_cancel_requested;
    size_t rate_limit_buckets;
    size_t rate_limit_blocked_buckets;
    uint64_t global_wait_ms;
    uint64_t next_bucket_wait_ms;
    uint64_t max_bucket_wait_ms;
    size_t pending_blocked_by_rate_limit;
    size_t pending_retry;
    uint64_t next_retry_wait_ms;
    uint64_t max_retry_wait_ms;
} dcc_rest_async_status_t;

typedef struct dcc_rest_multipart_field {
    const char *name;
    const char *value;
} dcc_rest_multipart_field_t;

typedef struct dcc_rest_multipart_file {
    const char *field_name;
    const char *filename;
    const char *content_type;
    const void *data;
    size_t data_len;
} dcc_rest_multipart_file_t;

#ifdef __cplusplus
}
#endif

#endif
