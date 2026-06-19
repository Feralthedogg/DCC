#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct identify_budget_api_server {
    int fd;
    unsigned short port;
    atomic_uint requests;
    atomic_uint bad;
} identify_budget_api_server_t;

static int write_all(int fd, const void *data, size_t len) {
    const char *cursor = (const char *)data;
    while (len > 0U) {
        ssize_t n = write(fd, cursor, len);
        if (n <= 0) {
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

static int read_http_request(int fd, char *buf, size_t cap) {
    size_t used = 0;
    while (used + 1U < cap) {
        ssize_t n = read(fd, buf + used, cap - used - 1U);
        if (n <= 0) {
            return -1;
        }
        used += (size_t)n;
        buf[used] = '\0';
        if (strstr(buf, "\r\n\r\n") != NULL) {
            return 0;
        }
    }
    return -1;
}

static int start_listener(int *out_fd, unsigned short *out_port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    int yes = 1;
    (void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, 2) != 0) {
        close(fd);
        return -1;
    }

    socklen_t len = (socklen_t)sizeof(addr);
    if (getsockname(fd, (struct sockaddr *)&addr, &len) != 0) {
        close(fd);
        return -1;
    }
    *out_fd = fd;
    *out_port = ntohs(addr.sin_port);
    return 0;
}

static void *api_server_main(void *arg) {
    identify_budget_api_server_t *server = (identify_budget_api_server_t *)arg;
    int client = accept(server->fd, NULL, NULL);
    if (client < 0) {
        atomic_store(&server->bad, 1U);
        return NULL;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    char request[2048];
    if (read_http_request(client, request, sizeof(request)) != 0 ||
        strstr(request, "GET /gateway/bot HTTP/1.1") == NULL ||
        strstr(request, "Authorization: Bot mock-token") == NULL) {
        atomic_store(&server->bad, 1U);
    } else {
        atomic_fetch_add(&server->requests, 1U);
    }

    const char *body =
        "{\"url\":\"ws://127.0.0.1:9\","
        "\"shards\":2,"
        "\"session_start_limit\":{\"total\":2,\"remaining\":1,"
        "\"reset_after\":60000,\"max_concurrency\":1}}";
    char response[512];
    int response_len = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        strlen(body),
        body
    );
    if (response_len <= 0 || (size_t)response_len >= sizeof(response) ||
        write_all(client, response, (size_t)response_len) != 0) {
        atomic_store(&server->bad, 1U);
    }

    close(client);
    return NULL;
}

int main(void) {
    identify_budget_api_server_t api;
    memset(&api, 0, sizeof(api));
    atomic_init(&api.requests, 0U);
    atomic_init(&api.bad, 0U);
    if (start_listener(&api.fd, &api.port) != 0) {
        fprintf(stderr, "failed to start identify budget API server\n");
        return 1;
    }

    pthread_t api_thread;
    if (pthread_create(&api_thread, NULL, api_server_main, &api) != 0) {
        close(api.fd);
        return 1;
    }

    char api_base[128];
    snprintf(api_base, sizeof(api_base), "http://127.0.0.1:%u", (unsigned)api.port);
    (void)setenv("DCC_DISCORD_API_BASE", api_base, 1);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
        },
        .shard_count = 2,
        .total_shard_count = 2,
        .gateway_max_concurrency = 1,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
    }

    (void)pthread_join(api_thread, NULL);
    close(api.fd);
    (void)unsetenv("DCC_DISCORD_API_BASE");

    dcc_gateway_info_t gateway_info = {
        .size = sizeof(gateway_info),
    };
    dcc_status_t gateway_info_status = cluster != NULL
        ? dcc_cluster_gateway_info(cluster, &gateway_info)
        : DCC_ERR_INVALID_ARG;
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = cluster != NULL
        ? dcc_cluster_health_summary(cluster, &summary)
        : DCC_ERR_INVALID_ARG;

    int health_ok = 1;
    for (uint32_t i = 0; cluster != NULL && i < 2U; ++i) {
        dcc_cluster_shard_info_t info = {
            .size = sizeof(info),
        };
        dcc_status_t info_status = dcc_cluster_shard_info(cluster, i, &info);
        if (info_status != DCC_OK ||
            info.shard_id != i ||
            info.shard_count != 2U ||
            info.status != DCC_CLUSTER_SHARD_ERROR ||
            info.starts != 1U ||
            info.restart_attempts != 0U ||
            info.restart_budget_used != 0U ||
            info.ready_events != 0U ||
            strcmp(info.last_error, "not enough remaining gateway identify sessions") != 0) {
            fprintf(
                stderr,
                "identify budget shard %u mismatch: info=%s id=%u/%u status=%s starts=%llu "
                "attempts=%llu budget=%llu ready=%llu last=%s\n",
                (unsigned)i,
                dcc_status_string(info_status),
                info.shard_id,
                info.shard_count,
                dcc_cluster_shard_status_string(info.status),
                (unsigned long long)info.starts,
                (unsigned long long)info.restart_attempts,
                (unsigned long long)info.restart_budget_used,
                (unsigned long long)info.ready_events,
                info.last_error
            );
            health_ok = 0;
        }
    }

    int ok = status == DCC_ERR_RATE_LIMITED &&
        gateway_info_status == DCC_OK &&
        summary_status == DCC_OK &&
        health_ok &&
        gateway_info.recommended_shards == 2U &&
        gateway_info.max_concurrency == 1U &&
        gateway_info.total_identifies == 2U &&
        gateway_info.remaining_identifies == 1U &&
        gateway_info.reset_after_ms == 60000ULL &&
        summary.total_shards == 2U &&
        summary.created_shards == 0U &&
        summary.starting_shards == 0U &&
        summary.ready_shards == 0U &&
        summary.stopped_shards == 0U &&
        summary.error_shards == 2U &&
        summary.starts == 2U &&
        summary.restart_attempts == 0U &&
        summary.restart_budget_used == 0U &&
        summary.ready_events == 0U &&
        dcc_cluster_wait(cluster) == DCC_ERR_STATE &&
        atomic_load(&api.requests) == 1U &&
        atomic_load(&api.bad) == 0U;

    if (!ok) {
        fprintf(
            stderr,
            "identify budget smoke failed: status=%s gateway_info=%s summary=%s health=%d "
            "requests=%u api_bad=%u shards=%u concurrency=%u remaining=%u total=%u reset=%llu "
            "summary_error=%u starts=%llu attempts=%llu budget=%llu ready=%llu\n",
            dcc_status_string(status),
            dcc_status_string(gateway_info_status),
            dcc_status_string(summary_status),
            health_ok,
            atomic_load(&api.requests),
            atomic_load(&api.bad),
            gateway_info.recommended_shards,
            gateway_info.max_concurrency,
            gateway_info.remaining_identifies,
            gateway_info.total_identifies,
            (unsigned long long)gateway_info.reset_after_ms,
            summary.error_shards,
            (unsigned long long)summary.starts,
            (unsigned long long)summary.restart_attempts,
            (unsigned long long)summary.restart_budget_used,
            (unsigned long long)summary.ready_events
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
