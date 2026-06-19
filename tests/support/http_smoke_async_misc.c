#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_rate_limit_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_rate_limited_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start local rate limit server: %s\n", strerror(errno));
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(server.fd);
        return 1;
    }

    rest_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    (void)dcc_client_on(client, DCC_EVENT_RATE_LIMITED, rate_limited_cb, &seen, NULL);

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/rate", (unsigned)server.port);
    st = dcc_rest_request(client, "POST", url, "{\"x\":1}", rest_cb, &seen);

    (void)pthread_join(thread, NULL);
    close(server.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_rest_request rate limit failed: %s\n", dcc_status_string(st));
        return 1;
    }
    if (!seen.called ||
        !seen.rate_limited_called ||
        seen.status != 200 ||
        seen.error != DCC_OK ||
        seen.retry_after < 0.019 ||
        seen.retry_after > 0.021 ||
        !seen.global ||
        server.request_count != 2U ||
        strcmp(seen.method, "POST") != 0 ||
        strcmp(seen.path, url) != 0 ||
        seen.body_len != 5 ||
        strcmp(seen.body, "hello") != 0) {
        fprintf(stderr,
                "unexpected rate limit retry result: called=%d event=%d status=%u error=%s retry=%f global=%u requests=%u method=%s path=%s body_len=%llu body=%s\n",
                seen.called,
                seen.rate_limited_called,
                seen.status,
                dcc_status_string(seen.error),
                seen.retry_after,
                seen.global,
                server.request_count,
                seen.method,
                seen.path,
                (unsigned long long)seen.body_len,
                seen.body);
        return 1;
    }
    return 0;
}

int run_public_rest_direct_message_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

    if (start_direct_message_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start direct message flow server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_direct_message(client, 444, "{\"content\":\"hi\"}", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        server.request_count != 2 ||
        strcmp(server.methods[0], "POST") != 0 ||
        strcmp(server.paths[0], "/users/@me/channels") != 0 ||
        strcmp(server.bodies[0], "{\"recipient_id\":\"444\"}") != 0 ||
        strcmp(server.methods[1], "POST") != 0 ||
        strcmp(server.paths[1], "/channels/222/messages") != 0 ||
        strcmp(server.bodies[1], "{\"content\":\"hi\"}") != 0 ||
        dcc_client_get_dm_channel(client, 444) != 222) {
        fprintf(stderr,
                "unexpected direct message flow: st=%s called=%d status=%u requests=%u first=%s %s %s second=%s %s %s cached=%llu\n",
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.request_count,
                server.methods[0],
                server.paths[0],
                server.bodies[0],
                server.methods[1],
                server.paths[1],
                server.bodies[1],
                (unsigned long long)dcc_client_get_dm_channel(client, 444));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start direct message cache server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_direct_message(client, 444, "{\"content\":\"again\"}", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        server.request_count != 1 ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/channels/222/messages") != 0 ||
        strcmp(server.body, "{\"content\":\"again\"}") != 0) {
        fprintf(stderr,
                "unexpected direct message cached flow: st=%s called=%d requests=%u method=%s path=%s body=%s\n",
                dcc_status_string(st),
                seen.called,
                server.request_count,
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}

#endif
