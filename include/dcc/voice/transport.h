#ifndef DCC_VOICE_TRANSPORT_H
#define DCC_VOICE_TRANSPORT_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_voice_client_connect_udp(
    dcc_voice_client_t *voice_client,
    const char *host,
    uint16_t port
);
DCC_API dcc_status_t dcc_voice_client_build_ip_discovery_packet(
    const dcc_voice_client_t *voice_client,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_parse_ip_discovery_packet(
    const uint8_t *packet,
    size_t packet_size,
    char *address_out,
    size_t address_cap,
    uint16_t *port_out
);
DCC_API dcc_status_t dcc_voice_client_discover_udp_ip(
    dcc_voice_client_t *voice_client,
    char *address_out,
    size_t address_cap,
    uint16_t *port_out,
    uint32_t timeout_ms
);
DCC_API void dcc_voice_client_disconnect_udp(dcc_voice_client_t *voice_client);
DCC_API intptr_t dcc_voice_client_udp_fd(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_rtp_state(
    dcc_voice_client_t *voice_client,
    uint16_t sequence,
    uint32_t timestamp
);
DCC_API uint16_t dcc_voice_client_rtp_sequence(const dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_rtp_timestamp(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_encryption_mode(
    dcc_voice_client_t *voice_client,
    dcc_voice_encryption_mode_t mode
);
DCC_API dcc_voice_encryption_mode_t dcc_voice_client_encryption_mode(
    const dcc_voice_client_t *voice_client
);
DCC_API dcc_status_t dcc_voice_client_set_secret_key(
    dcc_voice_client_t *voice_client,
    const uint8_t *key,
    size_t key_len
);
DCC_API void dcc_voice_client_clear_secret_key(dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_has_secret_key(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_packet_nonce(
    dcc_voice_client_t *voice_client,
    uint32_t nonce
);
DCC_API uint32_t dcc_voice_client_packet_nonce(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_send_opus_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size,
    uint64_t duration_ms
);
DCC_API dcc_status_t dcc_voice_client_send_pcm(
    dcc_voice_client_t *voice_client,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms
);
DCC_API dcc_status_t dcc_voice_client_insert_marker(
    dcc_voice_client_t *voice_client,
    const char *track_meta
);
DCC_API dcc_status_t dcc_voice_client_skip_to_next_marker(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_clear_send_queue(dcc_voice_client_t *voice_client);
DCC_API size_t dcc_voice_client_send_queue_size(const dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_tracks_remaining(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_drain_send_queue(
    dcc_voice_client_t *voice_client,
    size_t max_packets
);

#ifdef __cplusplus
}
#endif

#endif
