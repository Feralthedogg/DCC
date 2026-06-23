#include "internal/dcc_hot_reload_host.h"

int dcc_hot_reload_host_check_config(const dcc_hot_reload_host_options_t *options) {
    const char *token = dcc_hot_reload_host_token();
    const char *worker_path = dcc_hot_reload_host_resolved_worker(options);
    const char *guild_id_text = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_GUILD_ID");
    dcc_snowflake_t guild_id = 0;
    int guild_id_invalid = 0;
    int module_readable = 0;
    int worker_found = 0;
    int ok = 1;

    if (options->module_path == NULL || options->module_path[0] == '\0') {
        fprintf(stderr, "missing module path: pass --module or set DCC_BOT_MODULE\n");
        ok = 0;
    } else {
        module_readable = dcc_hot_reload_host_file_readable(options->module_path);
        if (!module_readable) {
            fprintf(stderr, "module is not readable: %s\n", options->module_path);
            ok = 0;
        }
    }
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "missing token: set DCC_TOKEN, BOT_TOKEN, or DISCORD_TOKEN\n");
        ok = 0;
    }
    if (worker_path == NULL || worker_path[0] == '\0') {
        fprintf(stderr, "missing worker path\n");
        ok = 0;
    } else {
        worker_found = dcc_hot_reload_host_worker_available(worker_path);
        if (!worker_found) {
            if (dcc_hot_reload_host_has_path_separator(worker_path)) {
                fprintf(stderr, "worker is not executable or not found: %s\n", worker_path);
            } else {
                fprintf(stderr, "worker was not found on PATH: %s\n", worker_path);
            }
            ok = 0;
        }
    }
    if (guild_id_text != NULL && dcc_hot_reload_host_parse_snowflake(guild_id_text, &guild_id) != 0) {
        fprintf(stderr, "invalid DCC_HOT_RELOAD_GUILD_ID value: %s\n", guild_id_text);
        guild_id_invalid = 1;
        ok = 0;
    }

    printf("module=%s\n", options->module_path != NULL ? options->module_path : "");
    printf("module_readable=%s\n", module_readable ? "yes" : "no");
    printf("worker=%s\n", worker_path != NULL ? worker_path : "");
    printf("worker_found=%s\n", worker_found ? "yes" : "no");
    printf("command_scope=%s\n", guild_id_invalid ? "invalid" : (guild_id != 0 ? "guild" : "global"));
    printf("guild_id=%s\n", guild_id != 0 ? guild_id_text : "");
    printf("intents=%llu\n", (unsigned long long)options->intents);
    printf("health_sidecar=%s\n", options->health_enabled ? "enabled" : "disabled");
    printf("health_address=%s\n", options->health_address != NULL ? options->health_address : "");
    printf("health_port=%u\n", (unsigned int)options->health_port);
    printf("health_path=/hot-reloadz\n");
    printf("poll_interval_ms=%u\n", (unsigned int)options->poll_interval_ms);
    printf("settle_interval_ms=%u\n", (unsigned int)options->settle_interval_ms);
    printf("worker_health_timeout_ms=%u\n", (unsigned int)options->worker_health_timeout_ms);
    printf("worker_drain_timeout_ms=%u\n", (unsigned int)options->worker_drain_timeout_ms);
    printf("token=%s\n", token != NULL && token[0] != '\0' ? "set" : "missing");
    return ok ? 0 : 1;
}
