#include "internal/client/dcc_cluster_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

static void dcc_cluster_wait_ready_sleep_ms(uint32_t ms) {
    if (ms == 0U) {
        return;
    }
    if (llam_sleep_ns((uint64_t)ms * UINT64_C(1000000)) == 0) {
        return;
    }
#if defined(_WIN32)
    Sleep((DWORD)ms);
#else
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000U);
    req.tv_nsec = (long)((ms % 1000U) * 1000000U);
    while (nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
#endif
}

static uint64_t dcc_cluster_wait_ready_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

static int dcc_cluster_wait_ready_failed(const dcc_cluster_health_summary_t *summary) {
    return summary->error_shards != 0U ||
        summary->stopped_shards != 0U ||
        summary->socket_closes != 0U ||
        summary->log_errors != 0U ||
        summary->restart_suppressed != 0U;
}

dcc_status_t dcc_cluster_wait_until_ready(
    dcc_cluster_t *cluster,
    uint32_t timeout_ms,
    dcc_cluster_health_summary_t *out
) {
    if (cluster == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }

    uint64_t start_ms = dcc_cluster_wait_ready_now_ms();
    for (;;) {
        dcc_cluster_health_summary_t summary = {
            .size = sizeof(summary),
        };
        dcc_status_t status = dcc_cluster_health_summary(cluster, &summary);
        if (status != DCC_OK) {
            return status;
        }
        if (out != NULL) {
            *out = summary;
        }
        if (summary.total_shards != 0U && summary.ready_shards == summary.total_shards) {
            return DCC_OK;
        }
        if (dcc_cluster_wait_ready_failed(&summary)) {
            return DCC_ERR_STATE;
        }
        if (timeout_ms != 0U && dcc_cluster_wait_ready_now_ms() - start_ms >= timeout_ms) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_cluster_wait_ready_sleep_ms(1U);
    }
}
