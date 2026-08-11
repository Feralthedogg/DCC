#ifndef DCC_REST_V2_TASK7_SENSITIVE_PROBE_H
#define DCC_REST_V2_TASK7_SENSITIVE_PROBE_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

/* Test-only observation seam for Task 7's private sensitive-state lifecycle. */
#define DCC_ENDPOINT_SENSITIVE_PROBE_VERSION 1U

#define DCC_ENDPOINT_SENSITIVE_WIPED_COMPLETION UINT32_C(1)
#define DCC_ENDPOINT_SENSITIVE_WIPED_CANCEL (UINT32_C(1) << 1U)
#define DCC_ENDPOINT_SENSITIVE_WIPED_DESTROY (UINT32_C(1) << 2U)
#define DCC_ENDPOINT_SENSITIVE_WIPED_ADMISSION_FAILURE (UINT32_C(1) << 3U)
#define DCC_ENDPOINT_SENSITIVE_WIPED_ROLLBACK (UINT32_C(1) << 4U)
#define DCC_ENDPOINT_SENSITIVE_WIPED_CLIENT_TEARDOWN (UINT32_C(1) << 5U)

#define DCC_ENDPOINT_SENSITIVE_MATERIAL_ESCAPED_TOKEN UINT32_C(1)
#define DCC_ENDPOINT_SENSITIVE_MATERIAL_WIRE_PATH (UINT32_C(1) << 1U)
#define DCC_ENDPOINT_SENSITIVE_MATERIAL_ROUTE_DIGEST (UINT32_C(1) << 2U)
#define DCC_ENDPOINT_SENSITIVE_MATERIAL_BEARER_TOKEN (UINT32_C(1) << 3U)
#define DCC_ENDPOINT_SENSITIVE_MATERIAL_AUTHORIZATION (UINT32_C(1) << 4U)

typedef enum dcc_endpoint_sensitive_failure_point {
    DCC_ENDPOINT_SENSITIVE_FAILURE_NONE = 0,
    DCC_ENDPOINT_SENSITIVE_FAILURE_ADMISSION,
    DCC_ENDPOINT_SENSITIVE_FAILURE_ALLOCATION_ROLLBACK
} dcc_endpoint_sensitive_failure_point_t;

typedef struct dcc_endpoint_sensitive_probe_snapshot {
    size_t size;
    uint32_t version;
    size_t secure_wipe_calls;
    size_t secure_wipe_bytes;
    uint32_t wiped_lifecycle_mask;
    uint32_t wiped_material_mask;
    uint32_t raw_log_hits;
    uint32_t raw_public_observer_hits;
    uint32_t raw_app_observer_hits;
    uint32_t raw_operation_hits;
    uint32_t raw_rate_limit_key_hits;
    uint32_t raw_async_route_hits;
    uint32_t raw_authorization_retention_hits;
    uint32_t opaque_route_key_hits;
} dcc_endpoint_sensitive_probe_snapshot_t;

#define DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT \
    { sizeof(dcc_endpoint_sensitive_probe_snapshot_t), \
      DCC_ENDPOINT_SENSITIVE_PROBE_VERSION, 0U, 0U, UINT32_C(0), UINT32_C(0), \
      0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }

/*
 * The implementation copies the exact secret bytes into thread-safe probe
 * state and compares them with every log, observer operation, rate-limit key,
 * async route, and Authorization storage retained after transport use. It
 * also records calls to the one optimization-resistant secure-zero primitive.
 */
void dcc_endpoint_test_sensitive_probe_begin(
    const void *secret,
    size_t secret_len
);
dcc_status_t dcc_endpoint_test_sensitive_probe_snapshot(
    dcc_endpoint_sensitive_probe_snapshot_t *out
);
void dcc_endpoint_test_sensitive_probe_end(void);
void dcc_endpoint_test_sensitive_force_failure_once(
    dcc_endpoint_sensitive_failure_point_t point
);

/* Private helpers are exposed only to the in-tree test target. */
dcc_status_t dcc_endpoint_sensitive_route_fingerprint(
    const char *encoded_segment,
    size_t encoded_segment_len,
    char out_hex[65]
);
void dcc_endpoint_secure_zero(void *bytes, size_t byte_count);

#endif
