#ifndef DCC_REST_V2_TASK7_PREPARATION_PROBE_H
#define DCC_REST_V2_TASK7_PREPARATION_PROBE_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_ENDPOINT_PREPARATION_PROBE_VERSION 1U
#define DCC_ENDPOINT_PREPARATION_CHECK_OPERATION UINT32_C(1)
#define DCC_ENDPOINT_PREPARATION_CHECK_AUDIT_HEADER (UINT32_C(1) << 1U)
#define DCC_ENDPOINT_PREPARATION_CHECK_AUTHORIZATION (UINT32_C(1) << 2U)

typedef struct dcc_endpoint_preparation_probe_snapshot {
    size_t size;
    uint32_t version;
    uint32_t preparations;
    uint32_t operation_matches;
    uint32_t audit_header_matches;
    uint32_t authorization_matches;
} dcc_endpoint_preparation_probe_snapshot_t;

#define DCC_ENDPOINT_PREPARATION_PROBE_SNAPSHOT_INIT \
    { sizeof(dcc_endpoint_preparation_probe_snapshot_t), \
      DCC_ENDPOINT_PREPARATION_PROBE_VERSION, 0U, 0U, 0U, 0U }

/*
 * Test-only observation seam at the final prepared-request boundary. A NULL
 * expected value means the checked header must be absent. Expected values are
 * copied so callers may mutate/free them immediately after submission.
 */
void dcc_endpoint_test_preparation_probe_begin(
    uint32_t checks,
    const char *expected_operation,
    const char *expected_audit_header,
    const char *expected_authorization
);
dcc_status_t dcc_endpoint_test_preparation_probe_snapshot(
    dcc_endpoint_preparation_probe_snapshot_t *out
);
void dcc_endpoint_test_preparation_probe_end(void);

#endif
