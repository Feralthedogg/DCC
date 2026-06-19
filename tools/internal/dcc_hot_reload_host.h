#ifndef DCC_HOT_RELOAD_HOST_INTERNAL_H
#define DCC_HOT_RELOAD_HOST_INTERNAL_H

#include <dcc/dcc.h>

#include <stdio.h>
#include <stddef.h>

typedef struct dcc_hot_reload_host_options {
    const char *module_path;
    const char *worker_path;
    const char *health_address;
    uint16_t health_port;
    uint32_t poll_interval_ms;
    uint32_t settle_interval_ms;
    uint32_t worker_health_timeout_ms;
    uint32_t worker_drain_timeout_ms;
    dcc_intents_t intents;
    dcc_snowflake_t guild_id;
    int guild_id_set;
    int health_enabled;
    int check_only;
} dcc_hot_reload_host_options_t;

typedef struct dcc_hot_reload_host_sidecar {
    dcc_interaction_server_t *server;
} dcc_hot_reload_host_sidecar_t;

void dcc_hot_reload_host_usage(FILE *stream, const char *argv0);

const char *dcc_hot_reload_host_env_nonempty(const char *name);
int dcc_hot_reload_host_setenv(const char *name, const char *value);
int dcc_hot_reload_host_unsetenv(const char *name);
void dcc_hot_reload_host_load_env_file(const char *path);

int dcc_hot_reload_host_parse_snowflake(const char *text, dcc_snowflake_t *out);
int dcc_hot_reload_host_token_matches(const char *token, const char *name);
dcc_intents_t dcc_hot_reload_host_all_intents(void);
int dcc_hot_reload_host_lookup_intent(const char *token, dcc_intents_t *out);
int dcc_hot_reload_host_parse_intents(const char *text, dcc_intents_t *out);
int dcc_hot_reload_host_parse_u16(const char *text, uint16_t *out);
int dcc_hot_reload_host_parse_u32(const char *text, uint32_t *out);
int dcc_hot_reload_host_parse_positive_u32(const char *text, uint32_t *out);
int dcc_hot_reload_host_parse_settle_ms(const char *text, uint32_t *out);
int dcc_hot_reload_host_parse_env_options(dcc_hot_reload_host_options_t *options);
int dcc_hot_reload_host_parse_option_arg(
    int argc,
    char **argv,
    int *index,
    dcc_hot_reload_host_options_t *options,
    int *out_handled
);
int dcc_hot_reload_host_parse_value_option(
    int argc,
    char **argv,
    int *index,
    dcc_hot_reload_host_options_t *options
);
int dcc_hot_reload_host_parse_intents_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
);
int dcc_hot_reload_host_parse_guild_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
);
int dcc_hot_reload_host_parse_health_port_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
);
int dcc_hot_reload_host_parse_u32_arg(
    const char *value,
    uint32_t *out,
    const char *label,
    int positive
);
int dcc_hot_reload_host_parse_settle_arg(
    const char *value,
    dcc_hot_reload_host_options_t *options
);
int dcc_hot_reload_host_apply_scope_env(const dcc_hot_reload_host_options_t *options);
int dcc_hot_reload_host_parse_options(
    int argc,
    char **argv,
    dcc_hot_reload_host_options_t *options
);

const char *dcc_hot_reload_host_token(void);
const char *dcc_hot_reload_host_resolved_worker(const dcc_hot_reload_host_options_t *options);
void dcc_hot_reload_host_build_client_options(
    const dcc_hot_reload_host_options_t *options,
    dcc_client_options_t *out
);
void dcc_hot_reload_host_build_hot_reload_options(
    const dcc_hot_reload_host_options_t *options,
    dcc_hot_reload_options_t *out
);
int dcc_hot_reload_host_file_readable(const char *path);
int dcc_hot_reload_host_is_path_separator(char ch);
int dcc_hot_reload_host_has_path_separator(const char *path);
int dcc_hot_reload_host_worker_executable_direct(const char *path);
int dcc_hot_reload_host_worker_candidate_exists(
    const char *directory,
    size_t directory_len,
    const char *worker_name,
    const char *suffix
);
int dcc_hot_reload_host_worker_found_in_directory(
    const char *directory,
    size_t directory_len,
    const char *worker_name
);
int dcc_hot_reload_host_worker_available(const char *worker_path);
int dcc_hot_reload_host_check_config(const dcc_hot_reload_host_options_t *options);
void dcc_hot_reload_host_sidecar_init(dcc_hot_reload_host_sidecar_t *sidecar);
dcc_status_t dcc_hot_reload_host_sidecar_start(
    const dcc_hot_reload_host_options_t *host_options,
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_host_sidecar_t *sidecar
);
void dcc_hot_reload_host_sidecar_stop(dcc_hot_reload_host_sidecar_t *sidecar);

#endif
