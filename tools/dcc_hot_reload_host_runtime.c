#include "internal/dcc_hot_reload_host.h"

#include <string.h>

static void dcc_hot_reload_host_log(dcc_log_level_t level, const char *message, void *user_data) {
    (void)user_data;
    fprintf(stderr, "[dcc:%d] %s\n", (int)level, message != NULL ? message : "");
}

void dcc_hot_reload_host_build_client_options(
    const dcc_hot_reload_host_options_t *options,
    dcc_client_options_t *out
) {
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    out->token = dcc_hot_reload_host_token();
    out->intents = options->intents;
    out->log_fn = dcc_hot_reload_host_log;
}

void dcc_hot_reload_host_build_hot_reload_options(
    const dcc_hot_reload_host_options_t *options,
    dcc_hot_reload_options_t *out
) {
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    out->poll_interval_ms = options->poll_interval_ms;
    out->settle_interval_ms = options->settle_interval_ms;
    out->backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER;
    out->worker_path = dcc_hot_reload_host_resolved_worker(options);
    out->worker_health_timeout_ms = options->worker_health_timeout_ms;
    out->worker_drain_timeout_ms = options->worker_drain_timeout_ms;
}
