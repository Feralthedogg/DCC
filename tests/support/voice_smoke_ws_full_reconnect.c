#include "voice_smoke_support.h"
#include "voice_smoke_ws_internal.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static void *voice_full_reconnect_ws_server_main(void *arg) {
    voice_full_reconnect_ws_server_t *server = (voice_full_reconnect_ws_server_t *)arg;
    const char *hello = "{\"op\":8,\"d\":{\"heartbeat_interval\":50}}";

    for (int attempt = 0; attempt < 5; ++attempt) {
        int client = voice_ws_accept_upgrade(server->fd);
        if (client < 0) {
            return NULL;
        }
        atomic_fetch_add_explicit(&server->accepts, 1U, memory_order_acq_rel);

        struct timeval timeout;
        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = 2;
        (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        char client_text[512];
        if (voice_ws_write_server_text_frame(client, hello) == 0 &&
            voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) == 0) {
            if (attempt == 0 &&
                strstr(client_text, "\"op\":0") != NULL &&
                strstr(client_text, "\"user_id\":\"1\"") != NULL &&
                strstr(client_text, "\"server_id\":\"2\"") != NULL) {
                atomic_store_explicit(&server->saw_identify, 1U, memory_order_release);
            } else if (attempt > 0 &&
                strstr(client_text, "\"op\":7") != NULL &&
                strstr(client_text, "\"server_id\":\"2\"") != NULL) {
                atomic_store_explicit(&server->saw_resume, 1U, memory_order_release);
            }
        }
        (void)voice_ws_write_server_close_frame(client, 1001);
        close(client);
    }

    return NULL;
}

int voice_full_reconnect_ws_start_server(
    voice_full_reconnect_ws_server_t *server,
    pthread_t *thread
) {
    memset(server, 0, sizeof(*server));
    server->fd = -1;
    atomic_init(&server->accepts, 0U);
    atomic_init(&server->saw_identify, 0U);
    atomic_init(&server->saw_resume, 0U);
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }

    int yes = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(server->fd, 8) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, voice_full_reconnect_ws_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

static void *voice_full_reconnect_gateway_server_main(void *arg) {
    voice_full_reconnect_gateway_server_t *server = (voice_full_reconnect_gateway_server_t *)arg;
    int client = voice_ws_accept_upgrade(server->fd);
    if (client < 0) {
        return NULL;
    }

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 4;
    (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if (voice_ws_write_server_text_frame(client, "{\"op\":10,\"d\":{\"heartbeat_interval\":50}}") != 0) {
        close(client);
        return NULL;
    }

    char client_text[1024];
    for (int i = 0; i < 6; ++i) {
        if (voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) != 0) {
            close(client);
            return NULL;
        }
        if (strstr(client_text, "\"op\":2") != NULL && strstr(client_text, "\"token\":\"mock-token\"") != NULL) {
            atomic_store_explicit(&server->saw_identify, 1U, memory_order_release);
            break;
        }
        if (strstr(client_text, "\"op\":1") != NULL) {
            (void)voice_ws_write_server_text_frame(client, "{\"op\":11,\"d\":null}");
        }
    }

    if (!atomic_load_explicit(&server->saw_identify, memory_order_acquire)) {
        close(client);
        return NULL;
    }

    char ready[384];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"session_id\":\"mock-session\","
        "\"user\":{\"id\":\"1\",\"username\":\"self\",\"discriminator\":\"0\",\"bot\":true},"
        "\"guilds\":[],\"resume_gateway_url\":\"ws://127.0.0.1:%u/?v=10&encoding=json\"}}",
        (unsigned)server->port
    );
    if (ready_len <= 0 || (size_t)ready_len >= sizeof(ready) ||
        voice_ws_write_server_text_frame(client, ready) != 0) {
        close(client);
        return NULL;
    }

    for (unsigned waited_ms = 0; waited_ms < 12000U; waited_ms += 100U) {
        if (voice_ws_read_client_text_frame_timeout(client, client_text, sizeof(client_text), 100U) != 0) {
            continue;
        }
        if (strstr(client_text, "\"op\":1") != NULL) {
            (void)voice_ws_write_server_text_frame(client, "{\"op\":11,\"d\":null}");
        } else if (strstr(client_text, "\"op\":4") != NULL &&
            strstr(client_text, "\"guild_id\":\"2\"") != NULL &&
            strstr(client_text, "\"channel_id\":\"222\"") != NULL &&
            strstr(client_text, "\"self_mute\":false") != NULL &&
            strstr(client_text, "\"self_deaf\":false") != NULL) {
            atomic_store_explicit(&server->saw_full_voice_state_update, 1U, memory_order_release);
            break;
        }
    }

    close(client);
    return NULL;
}

int voice_full_reconnect_gateway_start_server(
    voice_full_reconnect_gateway_server_t *server,
    pthread_t *thread
) {
    memset(server, 0, sizeof(*server));
    server->fd = -1;
    atomic_init(&server->saw_identify, 0U);
    atomic_init(&server->saw_full_voice_state_update, 0U);
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }

    int yes = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(server->fd, 1) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, voice_full_reconnect_gateway_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

#endif
