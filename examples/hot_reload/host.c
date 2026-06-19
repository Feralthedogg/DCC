#include <dcc/dcc.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct hot_reload_host_args {
    const char *module_path;
    const char *worker_path;
} hot_reload_host_args_t;

typedef struct hot_reload_health_sidecar {
    dcc_interaction_server_t *server;
} hot_reload_health_sidecar_t;

static int hot_reload_setenv(const char *name, const char *value, int overwrite) {
#if defined(_WIN32)
    if (!overwrite && getenv(name) != NULL) {
        return 0;
    }
    return _putenv_s(name, value);
#else
    return setenv(name, value, overwrite);
#endif
}

static long hot_reload_process_id(void) {
#if defined(_WIN32)
    return (long)_getpid();
#else
    return (long)getpid();
#endif
}

static int hot_reload_parse_args(int argc, char **argv, hot_reload_host_args_t *out) {
    if (out == NULL) {
        return -1;
    }
    out->module_path = NULL;
    out->worker_path = NULL;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            printf("usage: %s [module.so] [worker]\n", argv[0]);
            return 1;
        } else if (arg[0] == '-' && out->module_path == NULL) {
            fprintf(stderr, "unknown option: %s\n", arg);
            return -1;
        } else if (out->module_path == NULL) {
            out->module_path = arg;
        } else if (out->worker_path == NULL) {
            out->worker_path = arg;
        } else {
            fprintf(stderr, "unexpected argument: %s\n", arg);
            return -1;
        }
    }
    if (out->module_path == NULL) {
        out->module_path = "build/dcc_hot_reload_test_bot.so";
    }
    if (out->worker_path == NULL) {
        out->worker_path = "build/dcc_hot_reload_worker";
    }
    return 0;
}

static const char *hot_reload_env_string(const char *name, const char *fallback) {
    const char *value = getenv(name);
    return value != NULL && value[0] != '\0' ? value : fallback;
}

static uint16_t hot_reload_env_u16(const char *name, uint16_t fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }

    char *end = NULL;
    unsigned long parsed = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed > 65535UL) {
        fprintf(stderr, "ignoring invalid %s=%s, using %u\n", name, value, (unsigned)fallback);
        return fallback;
    }
    return (uint16_t)parsed;
}

static void hot_reload_load_env_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t') {
            cursor++;
        }
        if (*cursor == '#' || *cursor == '\0' || *cursor == '\n' || *cursor == '\r') {
            continue;
        }

        char *eq = strchr(cursor, '=');
        if (eq == NULL) {
            continue;
        }
        *eq = '\0';

        char *key = cursor;
        char *value = eq + 1;
        key[strcspn(key, " \t\r\n")] = '\0';
        value[strcspn(value, "\r\n")] = '\0';

        if (key[0] != '\0' && getenv(key) == NULL) {
            hot_reload_setenv(key, value, 0);
        }
    }

    fclose(file);
}

static dcc_status_t hot_reload_health_sidecar_start(
    hot_reload_health_sidecar_t *sidecar,
    dcc_hot_reload_t *hot_reload
) {
    if (sidecar == NULL || hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    static const char default_public_key[] =
        "0000000000000000000000000000000000000000000000000000000000000000";
    const char *address = hot_reload_env_string("DCC_HOT_RELOAD_HEALTH_ADDRESS", "127.0.0.1");
    uint16_t port = hot_reload_env_u16("DCC_HOT_RELOAD_HEALTH_PORT", 0U);

    dcc_interaction_server_options_t options;
    memset(&options, 0, sizeof(options));
    options.size = sizeof(options);
    options.public_key_hex = default_public_key;
    options.address = address;
    options.port = port;

    dcc_status_t status = dcc_interaction_server_create(&options, &sidecar->server);
    if (status == DCC_OK) {
        status = dcc_hot_reload_add_health_route(sidecar->server, "/hot-reloadz", hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_interaction_server_start(sidecar->server);
    }
    if (status != DCC_OK) {
        dcc_interaction_server_destroy(sidecar->server);
        sidecar->server = NULL;
        return status;
    }

    char url[256];
    (void)snprintf(
        url,
        sizeof(url),
        "http://%s:%u/hot-reloadz",
        address,
        (unsigned)dcc_interaction_server_port(sidecar->server)
    );
    printf("[hot-reload] health sidecar listening on %s\n", url);
    return DCC_OK;
}

static void hot_reload_health_sidecar_stop(hot_reload_health_sidecar_t *sidecar) {
    if (sidecar == NULL || sidecar->server == NULL) {
        return;
    }
    (void)dcc_interaction_server_stop(sidecar->server);
    dcc_interaction_server_destroy(sidecar->server);
    sidecar->server = NULL;
}

static void hot_reload_log_fn(dcc_log_level_t level, const char *message, void *user_data) {
    (void)user_data;
    fprintf(stderr, "[dcc:%d] %s\n", (int)level, message != NULL ? message : "");
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    hot_reload_host_args_t args;
    int arg_status = hot_reload_parse_args(argc, argv, &args);
    if (arg_status != 0) {
        return arg_status > 0 ? 0 : 1;
    }

    hot_reload_load_env_file(".env");
    hot_reload_load_env_file("../../.env");

    const char *module_path = args.module_path;
    const char *worker_path = args.worker_path;
    const char *token = getenv("BOT_TOKEN");
    if (token == NULL || token[0] == '\0') {
        token = getenv("DISCORD_TOKEN");
    }
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "BOT_TOKEN or DISCORD_TOKEN environment variable not set (put it in .env or export it)\n");
        return 1;
    }

    dcc_status_t status = DCC_OK;
    printf("DCC hot reload test host starting\n");
    printf("Host pid: %ld\n", hot_reload_process_id());
    printf("Watching module: %s\n", module_path);
    printf("Worker executable: %s\n", worker_path);
#if !defined(_WIN32)
    char resolved_module_path[PATH_MAX];
    if (realpath(module_path, resolved_module_path) != NULL) {
        printf("Watching module absolute: %s\n", resolved_module_path);
    }
#endif
    printf("Rebuild it with: make\n");

    dcc_client_options_t client_options;
    memset(&client_options, 0, sizeof(client_options));
    client_options.size = sizeof(client_options);
    client_options.token = token;
    client_options.intents = DCC_INTENT_GUILDS;
    client_options.shard_id = 0;
    client_options.shard_count = 1;
    client_options.log_fn = hot_reload_log_fn;

    dcc_client_t *client = NULL;
    status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "client create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_hot_reload_options_t hot_reload_options;
    memset(&hot_reload_options, 0, sizeof(hot_reload_options));
    hot_reload_options.size = sizeof(hot_reload_options);
    hot_reload_options.poll_interval_ms = 250;
    hot_reload_options.settle_interval_ms = 100;
    hot_reload_options.backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER;
    hot_reload_options.worker_path = worker_path;
    hot_reload_options.worker_health_timeout_ms = 3000;
    hot_reload_options.worker_drain_timeout_ms = 500;

    dcc_hot_reload_t *hot_reload = NULL;
    hot_reload_health_sidecar_t sidecar;
    memset(&sidecar, 0, sizeof(sidecar));

    status = dcc_hot_reload_create(client, module_path, &hot_reload_options, &hot_reload);
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = hot_reload_health_sidecar_start(&sidecar, hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_client_wait(client);
    }

    (void)dcc_hot_reload_stop(hot_reload);
    hot_reload_health_sidecar_stop(&sidecar);
    dcc_hot_reload_destroy(hot_reload);
    (void)dcc_client_stop(client);
    dcc_client_destroy(client);

    if (status != DCC_OK && status != DCC_ERR_CANCELED) {
        fprintf(stderr, "hot reload host failed: %s\n", dcc_status_string(status));
        return 1;
    }

    return 0;
}
