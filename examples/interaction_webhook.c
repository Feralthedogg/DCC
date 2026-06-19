#include <dcc/dcc.h>

#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32)
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#endif

typedef struct webhook_state {
    dcc_interaction_server_t *server;
    atomic_uint interaction_requests;
    atomic_uint failed_requests;
    int exit_after_one;
} webhook_state_t;

typedef struct webhook_config {
    const char *public_key;
    const char *address;
    const char *interaction_path;
    uint16_t port;
    uint32_t backlog;
    size_t max_header_size;
    size_t max_body_size;
    uint32_t drain_timeout_ms;
    int exit_after_one;
} webhook_config_t;

#if !defined(_WIN32)
typedef struct webhook_signal_state {
    dcc_interaction_server_t *server;
    uint32_t drain_timeout_ms;
    dcc_status_t drain_status;
    dcc_interaction_server_state_t drain_state;
    atomic_uint signal_seen;
} webhook_signal_state_t;

static int webhook_signal_pipe[2] = {-1, -1};

static void webhook_signal_handler(int signo) {
    unsigned char byte = (unsigned char)(signo & 0xff);
    if (webhook_signal_pipe[1] >= 0) {
        (void)write(webhook_signal_pipe[1], &byte, 1U);
    }
}

static int webhook_signal_set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return flags >= 0 && fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

static void *webhook_signal_thread(void *arg) {
    webhook_signal_state_t *state = (webhook_signal_state_t *)arg;
    unsigned char byte = 0;
    for (;;) {
        ssize_t nread = read(webhook_signal_pipe[0], &byte, 1U);
        if (nread == 1) {
            break;
        }
        if (nread < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
            (void)usleep(10000U);
            continue;
        }
        return NULL;
    }

    if (byte == 0U) {
        return NULL;
    }

    atomic_store(&state->signal_seen, 1U);
    state->drain_state.size = sizeof(state->drain_state);
    state->drain_status =
        dcc_interaction_server_drain(state->server, state->drain_timeout_ms, &state->drain_state);
    if (state->drain_status == DCC_ERR_TIMEOUT) {
        (void)dcc_interaction_server_stop(state->server);
    }
    return NULL;
}

static int webhook_signal_start(
    webhook_signal_state_t *state,
    dcc_interaction_server_t *server,
    uint32_t drain_timeout_ms,
    pthread_t *thread
) {
    memset(state, 0, sizeof(*state));
    state->server = server;
    state->drain_timeout_ms = drain_timeout_ms;
    state->drain_status = DCC_OK;
    state->drain_state.size = sizeof(state->drain_state);
    atomic_init(&state->signal_seen, 0U);

    if (pipe(webhook_signal_pipe) != 0) {
        perror("pipe");
        return 0;
    }
    if (!webhook_signal_set_nonblock(webhook_signal_pipe[0]) ||
        !webhook_signal_set_nonblock(webhook_signal_pipe[1])) {
        perror("fcntl");
        close(webhook_signal_pipe[0]);
        close(webhook_signal_pipe[1]);
        webhook_signal_pipe[0] = -1;
        webhook_signal_pipe[1] = -1;
        return 0;
    }

    (void)signal(SIGINT, webhook_signal_handler);
    (void)signal(SIGTERM, webhook_signal_handler);
    (void)signal(SIGPIPE, SIG_IGN);

    if (pthread_create(thread, NULL, webhook_signal_thread, state) != 0) {
        close(webhook_signal_pipe[0]);
        close(webhook_signal_pipe[1]);
        webhook_signal_pipe[0] = -1;
        webhook_signal_pipe[1] = -1;
        return 0;
    }
    return 1;
}

static void webhook_signal_stop(pthread_t thread, int thread_started) {
    if (webhook_signal_pipe[1] >= 0) {
        unsigned char byte = 0;
        (void)write(webhook_signal_pipe[1], &byte, 1U);
    }
    if (thread_started) {
        (void)pthread_join(thread, NULL);
    }
    if (webhook_signal_pipe[0] >= 0) {
        close(webhook_signal_pipe[0]);
    }
    if (webhook_signal_pipe[1] >= 0) {
        close(webhook_signal_pipe[1]);
    }
    webhook_signal_pipe[0] = -1;
    webhook_signal_pipe[1] = -1;
}
#endif

static int env_bool(const char *name, int fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }
    return strcmp(value, "0") != 0 &&
        strcmp(value, "false") != 0 &&
        strcmp(value, "FALSE") != 0 &&
        strcmp(value, "no") != 0 &&
        strcmp(value, "NO") != 0;
}

static uint16_t env_u16(const char *name, uint16_t fallback) {
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

static size_t env_size(const char *name, size_t fallback, size_t min_value, size_t max_value) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }

    char *end = NULL;
    unsigned long parsed = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed < (unsigned long)min_value ||
        parsed > (unsigned long)max_value) {
        fprintf(stderr, "ignoring invalid %s=%s, using %zu\n", name, value, fallback);
        return fallback;
    }
    return (size_t)parsed;
}

static const char *env_string(const char *name, const char *fallback) {
    const char *value = getenv(name);
    return value != NULL && value[0] != '\0' ? value : fallback;
}

static void webhook_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "Usage: %s [--check] [--help]\n"
        "\n"
        "Environment:\n"
        "  DISCORD_PUBLIC_KEY              Discord application public key in hex\n"
        "  DCC_INTERACTION_ADDRESS         bind address, default 127.0.0.1\n"
        "  DCC_INTERACTION_PORT            bind port, default 8080\n"
        "  DCC_INTERACTION_PATH            signed interaction route, default /interactions\n"
        "  DCC_INTERACTION_BACKLOG         listen backlog, default 128\n"
        "  DCC_INTERACTION_MAX_HEADER      max HTTP header bytes, default 8192\n"
        "  DCC_INTERACTION_MAX_BODY        max HTTP body bytes, default 16384\n"
        "  DCC_INTERACTION_DRAIN_TIMEOUT_MS graceful signal drain timeout, default 10000\n",
        argv0
    );
}

static int webhook_parse_args(int argc, char **argv, int *check_only) {
    *check_only = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            webhook_usage(stdout, argv[0]);
            return 1;
        }
        if (strcmp(argv[i], "--check") == 0) {
            *check_only = 1;
            continue;
        }
        fprintf(stderr, "unknown option: %s\n", argv[i]);
        webhook_usage(stderr, argv[0]);
        return -1;
    }
    return 0;
}

static void webhook_config_load(webhook_config_t *config) {
    config->public_key = getenv("DISCORD_PUBLIC_KEY");
    config->address = env_string("DCC_INTERACTION_ADDRESS", "127.0.0.1");
    config->interaction_path = env_string("DCC_INTERACTION_PATH", "/interactions");
    config->port = env_u16("DCC_INTERACTION_PORT", 8080U);
    config->backlog = (uint32_t)env_size("DCC_INTERACTION_BACKLOG", 128U, 1U, 65535U);
    config->max_header_size = env_size("DCC_INTERACTION_MAX_HEADER", 8192U, 1024U, 1048576U);
    config->max_body_size = env_size("DCC_INTERACTION_MAX_BODY", 16384U, 1024U, 1048576U);
    config->drain_timeout_ms =
        (uint32_t)env_size("DCC_INTERACTION_DRAIN_TIMEOUT_MS", 10000U, 0U, 3600000U);
    config->exit_after_one = env_bool("DCC_INTERACTION_EXIT_AFTER_ONE", 0);
}

static void webhook_server_options_from_config(
    const webhook_config_t *config,
    dcc_interaction_server_options_t *options
) {
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->public_key_hex = config->public_key;
    options->address = config->address;
    options->port = config->port;
    options->backlog = config->backlog;
    options->max_header_size = config->max_header_size;
    options->max_body_size = config->max_body_size;
}

static void print_request(dcc_interaction_request_t *request, const dcc_interaction_t *interaction) {
    printf(
        "%s %s type=%u event=%s id=%llu app=%llu\n",
        dcc_interaction_request_method(request),
        dcc_interaction_request_path(request),
        interaction != NULL ? interaction->type : 0U,
        dcc_event_type_name(dcc_interaction_request_event_type(request)),
        interaction != NULL ? (unsigned long long)interaction->id : 0ULL,
        interaction != NULL ? (unsigned long long)interaction->application_id : 0ULL
    );
}

static void interaction_route(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    webhook_state_t *state = (webhook_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_interaction_request_interaction(request);
    (void)server;
    atomic_fetch_add(&state->interaction_requests, 1U);

    if (interaction == NULL) {
        atomic_fetch_add(&state->failed_requests, 1U);
        (void)dcc_interaction_request_reply_text(request, 400, "invalid interaction");
        return;
    }

    print_request(request, interaction);
    if (dcc_interaction_request_is_ping(request)) {
        (void)dcc_interaction_request_reply_pong(request);
    } else {
        if (dcc_interaction_request_reply_ephemeral_message_text(
                request,
                "DCC interaction received"
            ) != DCC_OK) {
            atomic_fetch_add(&state->failed_requests, 1U);
            (void)dcc_interaction_request_reply_text(request, 500, "failed to build response");
        }
    }

    if (state->exit_after_one) {
        (void)dcc_interaction_server_stop(state->server);
    }
}

static int add_routes(dcc_interaction_server_t *server, webhook_state_t *state, const char *interaction_path) {
    dcc_interaction_route_t interaction = {
        .size = sizeof(interaction),
        .method = "POST",
        .path = interaction_path,
        .flags = 0,
        .callback = interaction_route,
        .user_data = state,
    };

    dcc_status_t st = dcc_interaction_server_add_health_route(server, "/healthz");
    if (st == DCC_OK) {
        st = dcc_interaction_server_add_route(server, &interaction);
    }
    if (st != DCC_OK) {
        fprintf(stderr, "route setup failed: %s\n", dcc_status_string(st));
        return 0;
    }
    return 1;
}

static int check_config(const webhook_config_t *config) {
    dcc_interaction_server_options_t options;
    dcc_interaction_server_t *server = NULL;
    webhook_state_t state;
    int public_key_set = config->public_key != NULL && config->public_key[0] != '\0';

    printf("public_key=%s\n", public_key_set ? "set" : "missing");
    printf("address=%s\n", config->address);
    printf("port=%u\n", (unsigned)config->port);
    printf("interaction_path=%s\n", config->interaction_path);
    printf("health_path=/healthz\n");
    printf("backlog=%u\n", (unsigned)config->backlog);
    printf("max_header=%zu\n", config->max_header_size);
    printf("max_body=%zu\n", config->max_body_size);
    printf("drain_timeout_ms=%u\n", (unsigned)config->drain_timeout_ms);

    if (!public_key_set) {
        fprintf(stderr, "set DISCORD_PUBLIC_KEY to run the interaction webhook example\n");
        return 1;
    }

    webhook_server_options_from_config(config, &options);
    dcc_status_t st = dcc_interaction_server_create(&options, &server);
    printf("server_create=%s\n", st == DCC_OK ? "yes" : "no");
    if (st != DCC_OK) {
        fprintf(stderr, "interaction server create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    memset(&state, 0, sizeof(state));
    state.server = server;
    if (!add_routes(server, &state, config->interaction_path)) {
        dcc_interaction_server_destroy(server);
        return 1;
    }
    printf("routes=ready\n");

    dcc_interaction_server_destroy(server);
    return 0;
}

int main(int argc, char **argv) {
    webhook_config_t config;
    int check_only = 0;
    int parse_status = webhook_parse_args(argc, argv, &check_only);
    if (parse_status > 0) {
        return 0;
    }
    if (parse_status < 0) {
        return 1;
    }

    webhook_config_load(&config);
    if (check_only) {
        return check_config(&config);
    }
    if (config.public_key == NULL || config.public_key[0] == '\0') {
        fprintf(stderr, "set DISCORD_PUBLIC_KEY to run the interaction webhook example\n");
        return 1;
    }

    webhook_state_t state;
    memset(&state, 0, sizeof(state));
    state.exit_after_one = config.exit_after_one;
    atomic_init(&state.interaction_requests, 0U);
    atomic_init(&state.failed_requests, 0U);

    dcc_interaction_server_options_t options;
    webhook_server_options_from_config(&config, &options);

    dcc_interaction_server_t *server = NULL;
    dcc_status_t st = dcc_interaction_server_create(&options, &server);
    if (st != DCC_OK) {
        fprintf(stderr, "interaction server create failed: %s\n", dcc_status_string(st));
        return 1;
    }
    state.server = server;

    if (!add_routes(server, &state, config.interaction_path)) {
        dcc_interaction_server_destroy(server);
        return 1;
    }

#if !defined(_WIN32)
    webhook_signal_state_t signal_state;
    pthread_t signal_thread;
    int signal_thread_started = webhook_signal_start(
        &signal_state,
        server,
        config.drain_timeout_ms,
        &signal_thread
    );
    if (!signal_thread_started) {
        dcc_interaction_server_destroy(server);
        return 1;
    }
#endif

    st = dcc_interaction_server_start(server);
    if (st == DCC_OK) {
        printf(
            "interaction webhook listening on http://%s:%u%s health=/healthz\n",
            config.address,
            (unsigned)dcc_interaction_server_port(server),
            config.interaction_path
        );
        st = dcc_interaction_server_wait(server);
    }

#if !defined(_WIN32)
    webhook_signal_stop(signal_thread, signal_thread_started);
    if (atomic_load(&signal_state.signal_seen) != 0U) {
        printf(
            "interaction webhook drain on signal: status=%s stopping=%u draining=%u active=%llu\n",
            dcc_status_string(signal_state.drain_status),
            signal_state.drain_state.stopping,
            signal_state.drain_state.draining,
            (unsigned long long)signal_state.drain_state.active_requests
        );
    }
#endif

    dcc_interaction_server_stats_t stats = {
        .size = sizeof(stats),
    };
    (void)dcc_interaction_server_stats(server, &stats);
    printf(
        "interaction webhook stopped: status=%s interactions=%u failed=%u accepted=%llu completed=%llu 4xx=%llu 5xx=%llu\n",
        dcc_status_string(st),
        atomic_load(&state.interaction_requests),
        atomic_load(&state.failed_requests),
        (unsigned long long)stats.accepted_connections,
        (unsigned long long)stats.completed_requests,
        (unsigned long long)stats.response_4xx,
        (unsigned long long)stats.response_5xx
    );
    if (st != DCC_OK) {
        fprintf(stderr, "last_error=%s\n", dcc_interaction_server_last_error(server));
    }

    dcc_interaction_server_destroy(server);
    return st == DCC_OK ? 0 : 1;
}
