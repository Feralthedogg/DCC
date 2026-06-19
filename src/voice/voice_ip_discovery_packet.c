#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_build_ip_discovery_packet(
    const dcc_voice_client_t *voice_client,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->ssrc == 0) {
        return DCC_ERR_STATE;
    }
    if (out_len != NULL) {
        *out_len = DCC_VOICE_IP_DISCOVERY_PACKET_SIZE;
    }
    if (out_cap < DCC_VOICE_IP_DISCOVERY_PACKET_SIZE) {
        return DCC_ERR_NOMEM;
    }

    memset(out, 0, DCC_VOICE_IP_DISCOVERY_PACKET_SIZE);
    dcc_voice_write_be16(out, 1U);
    dcc_voice_write_be16(out + 2U, DCC_VOICE_IP_DISCOVERY_PAYLOAD_LENGTH);
    dcc_voice_write_be32(out + 4U, voice_client->ssrc);
    return DCC_OK;
}

dcc_status_t dcc_voice_parse_ip_discovery_packet(
    const uint8_t *packet,
    size_t packet_size,
    char *address_out,
    size_t address_cap,
    uint16_t *port_out
) {
    if (packet == NULL || address_out == NULL || address_cap == 0 || port_out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (packet_size < DCC_VOICE_IP_DISCOVERY_PACKET_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t address_len = 0;
    while (address_len < DCC_VOICE_IP_DISCOVERY_ADDRESS_SIZE &&
           packet[8U + address_len] != '\0') {
        address_len++;
    }
    if (address_len >= address_cap) {
        return DCC_ERR_NOMEM;
    }
    memcpy(address_out, packet + 8U, address_len);
    address_out[address_len] = '\0';
    *port_out = dcc_voice_read_be16(packet + 72U);
    return DCC_OK;
}
