#include "internal/voice/dcc_voice_internal.h"
#include "internal/dcc_platform_resolve.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <string.h>

dcc_status_t dcc_voice_client_connect_udp(
    dcc_voice_client_t *voice_client,
    const char *host,
    uint16_t port
) {
    if (voice_client == NULL || host == NULL || host[0] == '\0' || port == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char port_text[16];
    int port_len = snprintf(port_text, sizeof(port_text), "%u", (unsigned)port);
    if (port_len <= 0 || (size_t)port_len >= sizeof(port_text)) {
        return DCC_ERR_INVALID_ARG;
    }

    struct addrinfo hints;
    struct addrinfo *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (dcc_platform_getaddrinfo(host, port_text, &hints, &result) != DCC_OK) {
        return DCC_ERR_NETWORK;
    }

    llam_fd_t fd = LLAM_INVALID_FD;
    struct sockaddr_storage remote_addr;
    socklen_t remote_addr_len = 0;
    memset(&remote_addr, 0, sizeof(remote_addr));

    for (struct addrinfo *ai = result; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_addr == NULL || ai->ai_addrlen > sizeof(remote_addr)) {
            continue;
        }

        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (LLAM_FD_IS_INVALID(fd)) {
            continue;
        }

        memcpy(&remote_addr, ai->ai_addr, ai->ai_addrlen);
        remote_addr_len = (socklen_t)ai->ai_addrlen;
        break;
    }

    dcc_platform_freeaddrinfo(result);
    if (LLAM_FD_IS_INVALID(fd)) {
        return DCC_ERR_NETWORK;
    }

    dcc_status_t status = dcc_voice_copy_text(voice_client->udp_host, sizeof(voice_client->udp_host), host);
    if (status != DCC_OK) {
        (void)llam_close(fd);
        return status;
    }

    dcc_voice_client_disconnect_udp(voice_client);
    voice_client->udp_fd = fd;
    voice_client->udp_addr = remote_addr;
    voice_client->udp_addr_len = remote_addr_len;
    voice_client->udp_port = port;
    voice_client->discovered_address[0] = '\0';
    voice_client->discovered_port = 0;
    voice_client->select_protocol_sent = 0;
    return DCC_OK;
}

void dcc_voice_client_disconnect_udp(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }
    if (!LLAM_FD_IS_INVALID(voice_client->udp_fd)) {
        (void)llam_close(voice_client->udp_fd);
    }
    voice_client->udp_fd = LLAM_INVALID_FD;
    memset(&voice_client->udp_addr, 0, sizeof(voice_client->udp_addr));
    voice_client->udp_addr_len = 0;
}

intptr_t dcc_voice_client_udp_fd(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || LLAM_FD_IS_INVALID(voice_client->udp_fd)) {
        return -1;
    }
    return (intptr_t)voice_client->udp_fd;
}
