#include "internal/dcc_windows_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>

#if !defined(_WIN32)
#include <sched.h>
#endif

/* DCC clients share LLAM's process-default runtime, which permits exactly one
 * scheduler driver. Keep the token held through the full llam_run() call so a
 * destroying client can either drive global cancellation itself or wait for
 * the existing driver without issuing a racy second llam_run(). */
static atomic_bool dcc_runtime_driver_active;

static void dcc_runtime_quiesce_yield(void) {
#if defined(_WIN32)
  (void)SwitchToThread();
#else
  (void)sched_yield();
#endif
}

dcc_status_t dcc_runtime_run(dcc_runtime_t *runtime) {
  if (runtime == NULL || !runtime->initialized) {
    return DCC_ERR_INVALID_ARG;
  }
  bool expected = false;
  if (!atomic_compare_exchange_strong_explicit(
          &dcc_runtime_driver_active, &expected, true, memory_order_acq_rel,
          memory_order_acquire)) {
    return DCC_ERR_RUNTIME;
  }
  int rc = llam_run();
  int run_errno = errno;
  atomic_store_explicit(&dcc_runtime_driver_active, false,
                        memory_order_release);
  return rc == 0                  ? DCC_OK
         : run_errno == ECANCELED ? DCC_ERR_CANCELED
                                  : DCC_ERR_RUNTIME;
}

dcc_status_t dcc_runtime_request_stop(dcc_runtime_t *runtime) {
  if (runtime == NULL || !runtime->initialized) {
    return DCC_OK;
  }
  if (llam_runtime_request_stop() != 0) {
    return DCC_ERR_RUNTIME;
  }
  return DCC_OK;
}

dcc_status_t dcc_runtime_quiesce(dcc_runtime_t *runtime) {
  if (runtime == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (!runtime->initialized) {
    return DCC_OK;
  }

  dcc_status_t status = dcc_runtime_request_stop(runtime);
  if (status != DCC_OK) {
    return status;
  }

  bool expected = false;
  if (atomic_compare_exchange_strong_explicit(
          &dcc_runtime_driver_active, &expected, true, memory_order_acq_rel,
          memory_order_acquire)) {
    int rc = llam_run();
    int run_errno = errno;
    atomic_store_explicit(&dcc_runtime_driver_active, false,
                          memory_order_release);
    status = rc == 0                  ? DCC_OK
             : run_errno == ECANCELED ? DCC_ERR_CANCELED
                                      : DCC_ERR_RUNTIME;
  } else {
    while (atomic_load_explicit(&dcc_runtime_driver_active,
                                memory_order_acquire)) {
      dcc_runtime_quiesce_yield();
    }
    status = DCC_OK;
  }
  while (
      atomic_load_explicit(&dcc_runtime_driver_active, memory_order_acquire)) {
    dcc_runtime_quiesce_yield();
  }
  if (status == DCC_ERR_CANCELED) {
    return DCC_OK;
  }
  return status;
}
