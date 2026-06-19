#include "voice_smoke_support.h"
#include "voice_smoke_ws_internal.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

void *voice_client_wait_thread_main(void *arg) {
    voice_client_wait_state_t *state = (voice_client_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

uint64_t voice_rusage_cpu_us(void) {
    struct rusage usage;
    memset(&usage, 0, sizeof(usage));
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        return 0;
    }
    uint64_t user_us = (uint64_t)usage.ru_utime.tv_sec * UINT64_C(1000000) +
        (uint64_t)usage.ru_utime.tv_usec;
    uint64_t sys_us = (uint64_t)usage.ru_stime.tv_sec * UINT64_C(1000000) +
        (uint64_t)usage.ru_stime.tv_usec;
    return user_us + sys_us;
}

uint64_t voice_u64_delta(uint64_t after, uint64_t before) {
    return after >= before ? after - before : 0;
}

static void voice_ws_server_idle_hold(voice_ws_server_t *server) {
    if (server == NULL || server->idle_max_us == 0) {
        return;
    }

    atomic_store_explicit(&server->idle_started, 1U, memory_order_release);
    useconds_t waited = 0;
    while (atomic_load_explicit(&server->idle_release, memory_order_acquire) == 0U &&
           waited < server->idle_max_us) {
        usleep(1000U);
        waited += 1000U;
    }
}

static int voice_ws_handle_udp_discovery(voice_ws_server_t *server, uint16_t *client_port_out) {
    unsigned char request[74];
    struct sockaddr_storage peer;
    socklen_t peer_len = sizeof(peer);
    ssize_t n = recvfrom(server->udp_fd, request, sizeof(request), 0, (struct sockaddr *)&peer, &peer_len);
    if (n != (ssize_t)sizeof(request) ||
        request[0] != 0x00 ||
        request[1] != 0x01 ||
        request[2] != 0x00 ||
        request[3] != 0x46 ||
        request[4] != 0x00 ||
        request[5] != 0x01 ||
        request[6] != 0xe2 ||
        request[7] != 0x40) {
        return -1;
    }

    uint16_t client_port = 0;
    if (peer.ss_family == AF_INET) {
        client_port = ntohs(((const struct sockaddr_in *)&peer)->sin_port);
    }
    if (client_port == 0) {
        return -1;
    }

    unsigned char response[74];
    memset(response, 0, sizeof(response));
    response[0] = 0x00;
    response[1] = 0x02;
    response[2] = 0x00;
    response[3] = 0x46;
    response[4] = request[4];
    response[5] = request[5];
    response[6] = request[6];
    response[7] = request[7];
    memcpy(response + 8, "127.0.0.1", strlen("127.0.0.1"));
    response[72] = (unsigned char)((client_port >> 8U) & 0xffU);
    response[73] = (unsigned char)(client_port & 0xffU);
    if (sendto(server->udp_fd, response, sizeof(response), 0, (const struct sockaddr *)&peer, peer_len) !=
        (ssize_t)sizeof(response)) {
        return -1;
    }

    server->saw_discovery = 1;
    *client_port_out = client_port;
    return 0;
}

static void *voice_ws_server_main(void *arg) {
    voice_ws_server_t *server = (voice_ws_server_t *)arg;
    int client = voice_ws_accept_upgrade(server->fd);
    if (client < 0) {
        return NULL;
    }

    const char *hello = "{\"op\":8,\"d\":{\"heartbeat_interval\":50}}";
    const char *description =
        "{\"op\":4,\"d\":{\"secret_key\":["
        "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]}}";
    char ready[160];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"op\":2,\"d\":{\"ssrc\":123456,\"ip\":\"127.0.0.1\",\"port\":%u,"
        "\"modes\":[\"aead_xchacha20_poly1305_rtpsize\"]}}",
        (unsigned)server->udp_port
    );
    if (ready_len <= 0 || (size_t)ready_len >= sizeof(ready)) {
        close(client);
        return NULL;
    }

    char client_text[512];
    if (voice_ws_write_server_text_frame(client, hello) == 0 &&
        voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) == 0 &&
        strcmp(
            client_text,
            "{\"op\":0,\"d\":{\"user_id\":\"1\",\"server_id\":\"2\","
            "\"session_id\":\"s\",\"token\":\"t\",\"max_dave_protocol_version\":0}}"
        ) == 0) {
        uint16_t discovered_port = 0;
        server->saw_identify = 1;
        (void)voice_ws_write_server_text_frame(client, ready);
        if (voice_ws_handle_udp_discovery(server, &discovered_port) == 0) {
            for (int i = 0; i < 4 && (!server->saw_select_protocol || !server->saw_heartbeat); ++i) {
                if (voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) != 0) {
                    break;
                }
                if (strstr(client_text, "\"op\":1") != NULL &&
                    strstr(client_text, "\"address\":\"127.0.0.1\"") != NULL) {
                    char expected_port[48];
                    snprintf(expected_port, sizeof(expected_port), "\"port\":%u", (unsigned)discovered_port);
                    if (strstr(client_text, expected_port) != NULL &&
                        strstr(client_text, "\"mode\":\"aead_xchacha20_poly1305_rtpsize\"") != NULL) {
                        server->saw_select_protocol = 1;
                        (void)voice_ws_write_server_text_frame(client, description);
                    }
                } else if (strstr(client_text, "\"op\":3") != NULL &&
                    strstr(client_text, "\"seq_ack\":-1") != NULL) {
                    server->saw_heartbeat = 1;
                }
            }
            if (!server->saw_heartbeat) {
                for (int i = 0; i < 4 && !server->saw_heartbeat; ++i) {
                    if (voice_ws_read_client_text_frame(client, client_text, sizeof(client_text)) != 0) {
                        break;
                    }
                    if (strstr(client_text, "\"op\":3") != NULL &&
                        strstr(client_text, "\"seq_ack\":-1") != NULL) {
                        server->saw_heartbeat = 1;
                    }
                }
            }
            if (server->saw_select_protocol && server->saw_heartbeat) {
                voice_ws_server_idle_hold(server);
            }
        }
        (void)voice_ws_write_server_close_frame(client, 1001);
    }

    close(client);
    return NULL;
}

int voice_ws_start_server(voice_ws_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    server->fd = -1;
    server->udp_fd = -1;
    atomic_init(&server->idle_started, 0U);
    atomic_init(&server->idle_release, 0U);
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }
    server->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->udp_fd < 0) {
        close(server->fd);
        return -1;
    }

    int yes = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    (void)setsockopt(server->udp_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(server->fd, 1) != 0) {
        close(server->fd);
        close(server->udp_fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        close(server->udp_fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(server->udp_fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(server->fd);
        close(server->udp_fd);
        return -1;
    }
    len = sizeof(addr);
    if (getsockname(server->udp_fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        close(server->udp_fd);
        return -1;
    }
    server->udp_port = ntohs(addr.sin_port);

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 2;
    (void)setsockopt(server->udp_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if (pthread_create(thread, NULL, voice_ws_server_main, server) != 0) {
        close(server->fd);
        close(server->udp_fd);
        return -1;
    }
    return 0;
}

void voice_ws_client_task(void *arg) {
    voice_ws_task_t *task = (voice_ws_task_t *)arg;
    task->status = dcc_voice_client_run_websocket_url(task->voice_client, 1, task->url, 0);
    (void)dcc_client_stop(dcc_voice_client_owner(task->voice_client));
}

#endif
