#ifndef DCC_VOICE_UDP_STATE_INTERNAL_H
#define DCC_VOICE_UDP_STATE_INTERNAL_H

#include "internal/voice/dcc_voice_limits_internal.h"

#include <dcc/dcc.h>
#include <llam/io.h>

#include <stddef.h>
#include <stdint.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#define DCC_VOICE_UDP_STATE_FIELDS \
    char udp_host[DCC_VOICE_UDP_HOST_CAP]; \
    uint16_t udp_port; \
    char discovered_address[DCC_VOICE_DISCOVERY_ADDRESS_CAP]; \
    uint16_t discovered_port; \
    uint8_t select_protocol_sent; \
    uint32_t ssrc; \
    llam_fd_t udp_fd; \
    struct sockaddr_storage udp_addr; \
    socklen_t udp_addr_len; \
    uint16_t rtp_sequence; \
    uint32_t rtp_timestamp; \
    dcc_voice_encryption_mode_t encryption_mode; \
    uint8_t has_secret_key; \
    uint8_t secret_key[32]; \
    uint32_t packet_nonce; \
    size_t ssrc_map_len; \
    uint32_t ssrc_map_ssrcs[DCC_VOICE_SSRC_MAP_CAP]; \
    dcc_snowflake_t ssrc_map_user_ids[DCC_VOICE_SSRC_MAP_CAP]

#endif
