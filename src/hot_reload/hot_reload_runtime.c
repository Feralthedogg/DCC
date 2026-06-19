#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

void dcc_hot_reload_broadcast(dcc_hot_reload_t *hot_reload) {
#if defined(_WIN32)
    WakeAllConditionVariable(&hot_reload->cond);
#else
    (void)pthread_cond_broadcast(&hot_reload->cond);
#endif
}

void dcc_hot_reload_wait_locked(dcc_hot_reload_t *hot_reload) {
#if defined(_WIN32)
    (void)SleepConditionVariableCS(&hot_reload->cond, &hot_reload->mutex, INFINITE);
#else
    (void)pthread_cond_wait(&hot_reload->cond, &hot_reload->mutex);
#endif
}

dcc_status_t dcc_hot_reload_wait_timeout_locked(dcc_hot_reload_t *hot_reload, uint32_t timeout_ms) {
#if defined(_WIN32)
    BOOL ok = SleepConditionVariableCS(&hot_reload->cond, &hot_reload->mutex, timeout_ms);
    if (ok) {
        return DCC_OK;
    }
    return GetLastError() == ERROR_TIMEOUT ? DCC_ERR_TIMEOUT : DCC_ERR_RUNTIME;
#else
    struct timespec deadline;
    if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
        return DCC_ERR_RUNTIME;
    }
    uint64_t sec = (uint64_t)timeout_ms / 1000U;
    uint64_t nsec = ((uint64_t)timeout_ms % 1000U) * UINT64_C(1000000);
    if (sec > (uint64_t)LONG_MAX - (uint64_t)deadline.tv_sec) {
        deadline.tv_sec = (time_t)LONG_MAX;
        deadline.tv_nsec = 999999999L;
    } else {
        deadline.tv_sec += (time_t)sec;
        deadline.tv_nsec += (long)nsec;
        if (deadline.tv_nsec >= 1000000000L) {
            deadline.tv_sec++;
            deadline.tv_nsec -= 1000000000L;
        }
    }

    int rc = pthread_cond_timedwait(&hot_reload->cond, &hot_reload->mutex, &deadline);
    if (rc == 0) {
        return DCC_OK;
    }
    return rc == ETIMEDOUT ? DCC_ERR_TIMEOUT : DCC_ERR_RUNTIME;
#endif
}

void dcc_hot_reload_set_error(dcc_hot_reload_t *hot_reload, dcc_status_t status, const char *message) {
    if (hot_reload == NULL) {
        return;
    }
    hot_reload->last_status = status;
    if (message != NULL) {
        snprintf(hot_reload->last_error, sizeof(hot_reload->last_error), "%s", message);
        dcc_set_error(hot_reload->client, message);
    } else {
        hot_reload->last_error[0] = '\0';
    }
}

void dcc_hot_reload_log(dcc_hot_reload_t *hot_reload, dcc_log_level_t level, const char *message) {
    if (hot_reload != NULL && message != NULL) {
        dcc_emit_log(hot_reload->client, level, message);
    }
}

uint64_t dcc_hot_reload_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

