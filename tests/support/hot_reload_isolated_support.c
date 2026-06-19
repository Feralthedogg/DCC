#include "hot_reload_isolated_support.h"

#if !defined(_WIN32)

#include "internal/events/dcc_events_internal.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

void *dcc_hot_reload_test_wait_main(void *arg) {
    dcc_hot_reload_test_wait_state_t *state = (dcc_hot_reload_test_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

void *dcc_hot_reload_test_server_main(void *arg) {
    dcc_hot_reload_test_server_t *server = (dcc_hot_reload_test_server_t *)arg;
    size_t max_requests = server->max_requests != 0U ? server->max_requests : 5U;
    for (size_t i = 0; i < max_requests && i < 5U; ++i) {
        struct pollfd pfd = { .fd = server->fd, .events = POLLIN };
        if (poll(&pfd, 1, 5000) <= 0) {
            return NULL;
        }
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            return NULL;
        }
        ssize_t n = read(client, server->request[i], sizeof(server->request[i]) - 1U);
        if (n > 0) {
            server->request_len[i] = (size_t)n;
            server->request[i][n] = '\0';
            server->request_count++;
        }
        const char response[] =
            "HTTP/1.1 204 No Content\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n\r\n";
        (void)write(client, response, sizeof(response) - 1U);
        close(client);
    }
    return NULL;
}

int dcc_hot_reload_test_server_start(dcc_hot_reload_test_server_t *server) {
    memset(server, 0, sizeof(*server));
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }
    int one = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(server->fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 ||
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
    return 0;
}

int dcc_hot_reload_test_copy_file(const char *source, const char *target) {
    FILE *in = fopen(source, "rb");
    if (in == NULL) {
        return -1;
    }
    FILE *out = fopen(target, "wb");
    if (out == NULL) {
        fclose(in);
        return -1;
    }
    unsigned char buffer[16384];
    int ok = 0;
    for (;;) {
        size_t n = fread(buffer, 1, sizeof(buffer), in);
        if (n > 0U && fwrite(buffer, 1, n, out) != n) {
            ok = -1;
            break;
        }
        if (n < sizeof(buffer)) {
            ok = ferror(in) ? -1 : ok;
            break;
        }
    }
    if (fclose(out) != 0) {
        ok = -1;
    }
    fclose(in);
    if (ok != 0) {
        unlink(target);
    } else {
        (void)chmod(target, 0700);
    }
    return ok;
}

int dcc_hot_reload_test_write_marker_file(const char *target, const char *marker) {
    FILE *out = fopen(target, "wb");
    if (out == NULL) {
        return -1;
    }
    size_t len = strlen(marker);
    int ok = fwrite(marker, 1, len, out) == len ? 0 : -1;
    if (fclose(out) != 0) {
        ok = -1;
    }
    if (ok != 0) {
        unlink(target);
    }
    return ok;
}

int dcc_hot_reload_test_dispatch_raw_slash(dcc_client_t *client, uint64_t sequence) {
    static const char slash_json[] =
        "{\"op\":0,\"s\":8,\"t\":\"INTERACTION_CREATE\",\"d\":{"
        "\"id\":\"555\",\"application_id\":\"666\",\"type\":2,"
        "\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"retry-token\","
        "\"version\":1,\"data\":{\"id\":\"777\",\"name\":\"retry\",\"type\":1}}}";
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.raw.type = DCC_EVENT_SLASH_COMMAND;
    event.raw.sequence = sequence;
    event.raw.name = "INTERACTION_CREATE";
    event.raw.json = slash_json;
    event.raw.json_len = sizeof(slash_json) - 1U;
    event.data.interaction.id = 555U;
    event.data.interaction.application_id = 666U;
    event.data.interaction.guild_id = 333U;
    event.data.interaction.channel_id = 222U;
    event.data.interaction.type = 2U;
    event.data.interaction.command_type = 1U;
    event.data.interaction.token = "retry-token";
    event.data.interaction.name = "retry";
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

#endif
