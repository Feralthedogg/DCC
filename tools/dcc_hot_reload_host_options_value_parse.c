#include "internal/dcc_hot_reload_host.h"

int dcc_hot_reload_host_parse_intents_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
) {
    if (dcc_hot_reload_host_parse_intents(value, &options->intents) != 0) {
        fprintf(stderr, "invalid intent value: %s\n", value);
        return -1;
    }
    return 0;
}

int dcc_hot_reload_host_parse_guild_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
) {
    if (dcc_hot_reload_host_parse_snowflake(value, &options->guild_id) != 0) {
        fprintf(stderr, "invalid guild id: %s\n", value);
        return -1;
    }
    options->guild_id_set = 1;
    return 0;
}

int dcc_hot_reload_host_parse_health_port_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
) {
    if (dcc_hot_reload_host_parse_u16(value, &options->health_port) != 0) {
        fprintf(stderr, "invalid health port: %s\n", value);
        return -1;
    }
    options->health_enabled = 1;
    return 0;
}

int dcc_hot_reload_host_parse_u32_arg(
    const char *value,
    uint32_t *out,
    const char *label,
    int positive
) {
    int status = positive
        ? dcc_hot_reload_host_parse_positive_u32(value, out)
        : dcc_hot_reload_host_parse_u32(value, out);
    if (status != 0) {
        fprintf(stderr, "invalid %s: %s\n", label, value);
        return -1;
    }
    return 0;
}

int dcc_hot_reload_host_parse_settle_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
) {
    if (dcc_hot_reload_host_parse_settle_ms(value, &options->settle_interval_ms) != 0) {
        fprintf(stderr, "invalid settle interval: %s\n", value);
        return -1;
    }
    return 0;
}
