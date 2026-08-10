#ifndef DCC_CLIENT_LIFECYCLE_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_LIFECYCLE_STATE_FIELDS_INTERNAL_H

#include <dcc/client.h>

#include <limits.h>
#include <stdatomic.h>

#define DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED ((UINT_MAX / 2U) + 1U)
#define DCC_CLIENT_LIFECYCLE_ADMISSION_COUNT_MASK \
    (DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED - 1U)

static inline dcc_status_t dcc_client_lifecycle_admission_enter(
    atomic_uint *admission
) {
    unsigned state = atomic_load_explicit(admission, memory_order_acquire);
    for (;;) {
        if ((state & DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED) != 0U) {
            return DCC_ERR_STATE;
        }
        if ((state & DCC_CLIENT_LIFECYCLE_ADMISSION_COUNT_MASK) ==
            DCC_CLIENT_LIFECYCLE_ADMISSION_COUNT_MASK) {
            return DCC_ERR_RUNTIME;
        }
        if (atomic_compare_exchange_weak_explicit(
                admission,
                &state,
                state + 1U,
                memory_order_acq_rel,
                memory_order_acquire
            )) {
            return DCC_OK;
        }
    }
}

static inline void dcc_client_lifecycle_admission_leave(
    atomic_uint *admission
) {
    (void)atomic_fetch_sub_explicit(admission, 1U, memory_order_acq_rel);
}

static inline void dcc_client_lifecycle_admission_close(
    atomic_uint *admission
) {
    (void)atomic_fetch_or_explicit(
        admission,
        DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED,
        memory_order_acq_rel
    );
}

static inline unsigned dcc_client_lifecycle_admission_count(
    const atomic_uint *admission
) {
    return atomic_load_explicit(admission, memory_order_acquire) &
        DCC_CLIENT_LIFECYCLE_ADMISSION_COUNT_MASK;
}

#define DCC_CLIENT_LIFECYCLE_STATE_FIELDS \
    atomic_bool started; \
    atomic_bool stopping; \
    atomic_uint start_admission; \
    atomic_uint wait_admission; \
    dcc_log_fn log_fn; \
    void *log_user_data; \
    char last_error[256]

#endif
