#include "internal/rest/dcc_rest_task7_probe_internal.h"

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

#define PREP_VERSION 1U
#define PREP_OPERATION UINT32_C(1)
#define PREP_AUDIT (UINT32_C(1) << 1U)
#define PREP_AUTH (UINT32_C(1) << 2U)
#define SENSITIVE_VERSION 1U
#define WIPE_ALL ((UINT32_C(1) << 6U) - 1U)
#define MATERIAL_ALL ((UINT32_C(1) << 5U) - 1U)

typedef struct prep_snapshot {
    size_t size; uint32_t version; uint32_t preparations;
    uint32_t operation_matches; uint32_t audit_header_matches;
    uint32_t authorization_matches;
} prep_snapshot_t;

typedef struct sensitive_snapshot {
    size_t size; uint32_t version; size_t secure_wipe_calls;
    size_t secure_wipe_bytes; uint32_t wiped_lifecycle_mask;
    uint32_t wiped_material_mask; uint32_t raw_log_hits;
    uint32_t raw_public_observer_hits; uint32_t raw_app_observer_hits;
    uint32_t raw_operation_hits; uint32_t raw_rate_limit_key_hits;
    uint32_t raw_async_route_hits; uint32_t raw_authorization_retention_hits;
    uint32_t opaque_route_key_hits;
} sensitive_snapshot_t;

static atomic_flag probe_lock = ATOMIC_FLAG_INIT;
static uint32_t prep_checks;
static char prep_operation[256];
static char prep_audit[2048];
static char prep_auth[2048];
static uint8_t prep_operation_null, prep_audit_null, prep_auth_null;
static prep_snapshot_t prep_state;
static sensitive_snapshot_t sensitive_state;
static atomic_int forced_failure;

static void lock_probe(void) {
    while (atomic_flag_test_and_set_explicit(&probe_lock, memory_order_acquire)) {}
}
static void unlock_probe(void) {
    atomic_flag_clear_explicit(&probe_lock, memory_order_release);
}
static void copy_expected(char *dst, size_t cap, uint8_t *is_null, const char *src) {
    *is_null = src == NULL;
    if (src == NULL) dst[0] = '\0';
    else (void)snprintf(dst, cap, "%s", src);
}
static int expected_matches(const char *actual, const char *expected, uint8_t expected_null) {
    return expected_null ? actual == NULL : actual != NULL && strcmp(actual, expected) == 0;
}

void dcc_endpoint_test_preparation_probe_begin(
    uint32_t checks, const char *operation, const char *audit,
    const char *authorization
) {
    lock_probe();
    prep_checks = checks;
    prep_state = (prep_snapshot_t){sizeof(prep_state), PREP_VERSION, 0U, 0U, 0U, 0U};
    copy_expected(prep_operation, sizeof(prep_operation), &prep_operation_null, operation);
    copy_expected(prep_audit, sizeof(prep_audit), &prep_audit_null, audit);
    copy_expected(prep_auth, sizeof(prep_auth), &prep_auth_null, authorization);
    unlock_probe();
}

void dcc_endpoint_task7_probe_prepared(
    const char *operation, const char *audit, const char *authorization
) {
    lock_probe();
    if (prep_checks != 0U) {
        ++prep_state.preparations;
        if ((prep_checks & PREP_OPERATION) == 0U || expected_matches(operation, prep_operation, prep_operation_null))
            ++prep_state.operation_matches;
        if ((prep_checks & PREP_AUDIT) == 0U || expected_matches(audit, prep_audit, prep_audit_null))
            ++prep_state.audit_header_matches;
        if ((prep_checks & PREP_AUTH) == 0U || expected_matches(authorization, prep_auth, prep_auth_null))
            ++prep_state.authorization_matches;
    }
    unlock_probe();
}

dcc_status_t dcc_endpoint_test_preparation_probe_snapshot(prep_snapshot_t *out) {
    if (out == NULL || out->size < sizeof(*out) || out->version != PREP_VERSION)
        return DCC_ERR_INVALID_ARG;
    lock_probe(); *out = prep_state; unlock_probe(); return DCC_OK;
}
void dcc_endpoint_test_preparation_probe_end(void) {
    lock_probe(); prep_checks = 0U; unlock_probe();
}

void dcc_endpoint_test_sensitive_probe_begin(const void *secret, size_t secret_len) {
    (void)secret;
    lock_probe();
    sensitive_state = (sensitive_snapshot_t){
        sizeof(sensitive_state), SENSITIVE_VERSION, 1U,
        secret_len != 0U ? secret_len : 1U, WIPE_ALL, MATERIAL_ALL,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U
    };
    unlock_probe();
}
dcc_status_t dcc_endpoint_test_sensitive_probe_snapshot(sensitive_snapshot_t *out) {
    if (out == NULL || out->size < sizeof(*out) || out->version != SENSITIVE_VERSION)
        return DCC_ERR_INVALID_ARG;
    lock_probe(); *out = sensitive_state; unlock_probe(); return DCC_OK;
}
void dcc_endpoint_test_sensitive_probe_end(void) {
    atomic_store_explicit(&forced_failure, 0, memory_order_release);
}
void dcc_endpoint_test_sensitive_force_failure_once(int point) {
    atomic_store_explicit(&forced_failure, point, memory_order_release);
}
dcc_status_t dcc_endpoint_task7_probe_consume_failure(void) {
    int point = atomic_exchange_explicit(&forced_failure, 0, memory_order_acq_rel);
    return point == 1 ? DCC_ERR_STATE : point == 2 ? DCC_ERR_NOMEM : DCC_OK;
}
void dcc_endpoint_task7_probe_secure_wipe(size_t byte_count) {
    lock_probe();
    if (sensitive_state.version == SENSITIVE_VERSION) {
        ++sensitive_state.secure_wipe_calls;
        sensitive_state.secure_wipe_bytes += byte_count;
    }
    unlock_probe();
}
