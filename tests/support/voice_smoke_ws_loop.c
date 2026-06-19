#include "voice_smoke_support.h"
#include "voice_smoke_ws_internal.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static void *voice_ws_loop_server_main(void *arg) {
    voice_ws_loop_server_t *server = (voice_ws_loop_server_t *)arg;
    const char *hello = "{\"op\":8,\"d\":{\"heartbeat_interval\":50}}";
    const char *ready =
        "{\"op\":2,\"d\":{\"ssrc\":123456,\"modes\":[\"aead_xchacha20_poly1305_rtpsize\"]}}";
    const char *description =
        "{\"op\":4,\"d\":{\"secret_key\":["
        "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]}}";
    const char *resumed = "{\"op\":9,\"d\":null}";

    for (int attempt = 0; attempt < 2; ++attempt) {
        int client = voice_ws_accept_upgrade(server->fd);
        if (client < 0) {
            return NULL;
        }
        server->accepts++;

        char client_text[512];
        if (voice_ws_write_server_text_frame(client, hello) == 0 &&
            voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) == 0) {
            if (attempt == 0 &&
                strcmp(
                    client_text,
                    "{\"op\":0,\"d\":{\"user_id\":\"1\",\"server_id\":\"2\","
                    "\"session_id\":\"s\",\"token\":\"t\",\"max_dave_protocol_version\":0}}"
                ) == 0) {
                server->saw_identify = 1;
                (void)voice_ws_write_server_text_frame(client, ready);
                (void)voice_ws_write_server_text_frame(client, description);
                (void)voice_ws_write_server_close_frame(client, 1001);
            } else if (attempt == 1 &&
                strcmp(
                    client_text,
                    "{\"op\":7,\"d\":{\"server_id\":\"2\",\"session_id\":\"s\","
                    "\"token\":\"t\",\"seq_ack\":-1}}"
                ) == 0) {
                server->saw_resume = 1;
                (void)voice_ws_write_server_text_frame(client, resumed);
                (void)voice_ws_write_server_close_frame(client, 1000);
            }
        }
        close(client);
    }

    return NULL;
}

int voice_ws_loop_start_server(voice_ws_loop_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    server->fd = -1;
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
        listen(server->fd, 2) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, voice_ws_loop_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

static void *voice_ws_chaos_server_main(void *arg) {
    voice_ws_chaos_server_t *server = (voice_ws_chaos_server_t *)arg;
    const char *hello = "{\"op\":8,\"d\":{\"heartbeat_interval\":50}}";
    const char *ready =
        "{\"op\":2,\"d\":{\"ssrc\":123456,\"modes\":[\"aead_xchacha20_poly1305_rtpsize\"]}}";
    const char *description =
        "{\"op\":4,\"d\":{\"secret_key\":["
        "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]}}";
    const char *resumed = "{\"op\":9,\"d\":null}";

    for (int attempt = 0; attempt < 2; ++attempt) {
        int client = voice_ws_accept_upgrade(server->fd);
        if (client < 0) {
            atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
            return NULL;
        }
        atomic_fetch_add_explicit(&server->accepts, 1U, memory_order_acq_rel);

        struct timeval timeout;
        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = 2;
        (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        char client_text[512];
        if (voice_ws_write_server_text_frame(client, hello) != 0 ||
            voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) != 0) {
            atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
            close(client);
            return NULL;
        }

        if (attempt == 0) {
            if (strcmp(
                    client_text,
                    "{\"op\":0,\"d\":{\"user_id\":\"1\",\"server_id\":\"2\","
                    "\"session_id\":\"s\",\"token\":\"t\",\"max_dave_protocol_version\":0}}"
                ) != 0) {
                atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
                close(client);
                return NULL;
            }
            atomic_store_explicit(&server->saw_identify, 1U, memory_order_release);
            if (voice_ws_write_server_text_frame(client, ready) != 0 ||
                voice_ws_write_server_text_frame(client, description) != 0 ||
                voice_ws_write_server_close_frame_reason(client, 4015U, "server crashed") != 0) {
                atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
                close(client);
                return NULL;
            }
            atomic_store_explicit(&server->sent_4015, 1U, memory_order_release);
        } else {
            if (strcmp(
                    client_text,
                    "{\"op\":7,\"d\":{\"server_id\":\"2\",\"session_id\":\"s\","
                    "\"token\":\"t\",\"seq_ack\":-1}}"
                ) != 0) {
                atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
                close(client);
                return NULL;
            }
            atomic_store_explicit(&server->saw_resume, 1U, memory_order_release);
            if (voice_ws_write_server_text_frame(client, resumed) != 0 ||
                voice_ws_write_server_close_frame_reason(client, 4014U, "disconnected") != 0) {
                atomic_fetch_add_explicit(&server->bad, 1U, memory_order_acq_rel);
                close(client);
                return NULL;
            }
            atomic_store_explicit(&server->sent_4014, 1U, memory_order_release);
        }
        close(client);
    }

    return NULL;
}

int voice_ws_chaos_start_server(voice_ws_chaos_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    server->fd = -1;
    atomic_init(&server->accepts, 0U);
    atomic_init(&server->saw_identify, 0U);
    atomic_init(&server->saw_resume, 0U);
    atomic_init(&server->sent_4015, 0U);
    atomic_init(&server->sent_4014, 0U);
    atomic_init(&server->bad, 0U);
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
        listen(server->fd, 2) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, voice_ws_chaos_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

void voice_ws_loop_client_task(void *arg) {
    voice_ws_loop_task_t *task = (voice_ws_loop_task_t *)arg;
    task->status = dcc_voice_client_run_websocket_loop_url(task->voice_client, 1, task->url, 1);
    (void)dcc_client_stop(dcc_voice_client_owner(task->voice_client));
}

#endif
