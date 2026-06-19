#include "internal/voice/dcc_voice_internal.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <poll.h>
#include <sys/socket.h>
#endif

#include <limits.h>

#ifndef POLLIN
#define POLLIN 0x0001
#endif

dcc_status_t dcc_voice_client_discover_udp_ip(
    dcc_voice_client_t *voice_client,
    char *address_out,
    size_t address_cap,
    uint16_t *port_out,
    uint32_t timeout_ms
) {
    if (voice_client == NULL || address_out == NULL || address_cap == 0 || port_out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (LLAM_FD_IS_INVALID(voice_client->udp_fd) ||
        voice_client->udp_addr_len == 0 ||
        voice_client->ssrc == 0) {
        return DCC_ERR_STATE;
    }

    uint8_t packet[DCC_VOICE_IP_DISCOVERY_PACKET_SIZE];
    size_t packet_len = 0;
    dcc_status_t status = dcc_voice_client_build_ip_discovery_packet(
        voice_client,
        packet,
        sizeof(packet),
        &packet_len
    );
    if (status != DCC_OK) {
        return status;
    }

    ssize_t sent = llam_sendto(
        voice_client->udp_fd,
        packet,
        packet_len,
        0,
        (const struct sockaddr *)&voice_client->udp_addr,
        voice_client->udp_addr_len
    );
    if (sent != (ssize_t)packet_len) {
        return DCC_ERR_NETWORK;
    }

    short revents = 0;
    int poll_timeout = timeout_ms > (uint32_t)INT_MAX ? INT_MAX : (int)timeout_ms;
    int poll_result = llam_poll_fd(voice_client->udp_fd, POLLIN, poll_timeout, &revents);
    if (poll_result < 0) {
        return DCC_ERR_NETWORK;
    }
    if (poll_result == 0 || (revents & POLLIN) == 0) {
        return DCC_ERR_TIMEOUT;
    }

    uint8_t response[DCC_VOICE_IP_DISCOVERY_PACKET_SIZE];
    ssize_t received = llam_recvfrom(voice_client->udp_fd, response, sizeof(response), 0, NULL, NULL);
    if (received < (ssize_t)DCC_VOICE_IP_DISCOVERY_PACKET_SIZE) {
        return DCC_ERR_NETWORK;
    }

    status = dcc_voice_parse_ip_discovery_packet(
        response,
        (size_t)received,
        address_out,
        address_cap,
        port_out
    );
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_voice_copy_text(
        voice_client->discovered_address,
        sizeof(voice_client->discovered_address),
        address_out
    );
    if (status != DCC_OK) {
        return status;
    }
    voice_client->discovered_port = *port_out;
    return DCC_OK;
}
