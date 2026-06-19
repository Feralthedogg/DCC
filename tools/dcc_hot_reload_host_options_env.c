#include "internal/dcc_hot_reload_host.h"

int dcc_hot_reload_host_parse_env_options(dcc_hot_reload_host_options_t *options) {
    const char *value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_INTENTS");
    if (value != NULL && dcc_hot_reload_host_parse_intents(value, &options->intents) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_INTENTS value\n");
        return -1;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_HEALTH_ADDRESS");
    if (value != NULL) {
        options->health_address = value;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_HEALTH_PORT");
    if (value != NULL) {
        if (dcc_hot_reload_host_parse_u16(value, &options->health_port) != 0) {
            fprintf(stderr, "invalid DCC_HOT_RELOAD_HEALTH_PORT value\n");
            return -1;
        }
        options->health_enabled = 1;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_POLL_MS");
    if (value != NULL && dcc_hot_reload_host_parse_positive_u32(value, &options->poll_interval_ms) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_POLL_MS value\n");
        return -1;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_SETTLE_MS");
    if (value != NULL && dcc_hot_reload_host_parse_settle_ms(value, &options->settle_interval_ms) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_SETTLE_MS value\n");
        return -1;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_WORKER_HEALTH_MS");
    if (value != NULL && dcc_hot_reload_host_parse_positive_u32(value, &options->worker_health_timeout_ms) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_WORKER_HEALTH_MS value\n");
        return -1;
    }

    value = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_WORKER_DRAIN_MS");
    if (value != NULL && dcc_hot_reload_host_parse_u32(value, &options->worker_drain_timeout_ms) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_WORKER_DRAIN_MS value\n");
        return -1;
    }

    return 0;
}
