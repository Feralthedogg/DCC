#include "voice_smoke_support.h"

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

int voice_udp_smoke(
    dcc_voice_client_t *voice_client,
    voice_smoke_state_t *state,
    voice_decode_state_t *decode_state,
    const uint8_t *queued_audio,
    const uint8_t *secret_key,
    size_t secret_key_size
) {
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        fprintf(stderr, "voice udp receiver socket failed\n");
        return 1;
    }
    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    udp_addr.sin_port = 0;
    if (bind(udp_fd, (const struct sockaddr *)&udp_addr, sizeof(udp_addr)) != 0) {
        fprintf(stderr, "voice udp receiver bind failed\n");
        close(udp_fd);
        return 1;
    }
    socklen_t udp_addr_len = sizeof(udp_addr);
    if (getsockname(udp_fd, (struct sockaddr *)&udp_addr, &udp_addr_len) != 0) {
        fprintf(stderr, "voice udp receiver getsockname failed\n");
        close(udp_fd);
        return 1;
    }
    struct timeval udp_timeout;
    memset(&udp_timeout, 0, sizeof(udp_timeout));
    udp_timeout.tv_sec = 1;
    (void)setsockopt(udp_fd, SOL_SOCKET, SO_RCVTIMEO, &udp_timeout, sizeof(udp_timeout));

    const char *voice_udp_endpoint_frame = "{\"op\":2,\"d\":{\"ssrc\":123456}}";
    if (dcc_voice_client_handle_gateway_frame(
        voice_client,
        voice_udp_endpoint_frame,
        strlen(voice_udp_endpoint_frame)
    ) != DCC_OK ||
        dcc_voice_client_set_rtp_state(voice_client, 0x1234, 0x01020304) != DCC_OK ||
        dcc_voice_client_rtp_sequence(voice_client) != 0x1234 ||
        dcc_voice_client_rtp_timestamp(voice_client) != 0x01020304 ||
        dcc_voice_client_connect_udp(voice_client, "127.0.0.1", ntohs(udp_addr.sin_port)) != DCC_OK ||
        dcc_voice_client_udp_fd(voice_client) < 0 ||
        dcc_voice_client_send_opus_packet(voice_client, queued_audio, 8, 20) != DCC_OK ||
        dcc_voice_client_drain_send_queue(voice_client, 1) != DCC_OK) {
        fprintf(stderr, "voice udp send setup failed\n");
        close(udp_fd);
        return 1;
    }

    uint8_t udp_received[32];
    ssize_t udp_read = recvfrom(udp_fd, udp_received, sizeof(udp_received), 0, NULL, NULL);
    dcc_voice_client_disconnect_udp(voice_client);
    close(udp_fd);
    if (udp_read != 20 ||
        udp_received[0] != 0x80 ||
        udp_received[1] != 0x78 ||
        udp_received[2] != 0x12 ||
        udp_received[3] != 0x34 ||
        udp_received[4] != 0x01 ||
        udp_received[5] != 0x02 ||
        udp_received[6] != 0x03 ||
        udp_received[7] != 0x04 ||
        udp_received[8] != 0x00 ||
        udp_received[9] != 0x01 ||
        udp_received[10] != 0xe2 ||
        udp_received[11] != 0x40 ||
        memcmp(udp_received + 12, queued_audio, 8) != 0 ||
        dcc_voice_client_udp_fd(voice_client) != -1 ||
        dcc_voice_client_rtp_sequence(voice_client) != 0x1235 ||
        dcc_voice_client_rtp_timestamp(voice_client) != 0x010206c4 ||
        !state->udp_buffer_send_seen) {
        fprintf(stderr, "voice udp send validation failed\n");
        return 1;
    }

    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        fprintf(stderr, "voice encrypted udp receiver socket failed\n");
        return 1;
    }
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    udp_addr.sin_port = 0;
    if (bind(udp_fd, (const struct sockaddr *)&udp_addr, sizeof(udp_addr)) != 0) {
        fprintf(stderr, "voice encrypted udp receiver bind failed\n");
        close(udp_fd);
        return 1;
    }
    udp_addr_len = sizeof(udp_addr);
    if (getsockname(udp_fd, (struct sockaddr *)&udp_addr, &udp_addr_len) != 0) {
        fprintf(stderr, "voice encrypted udp receiver getsockname failed\n");
        close(udp_fd);
        return 1;
    }
    memset(&udp_timeout, 0, sizeof(udp_timeout));
    udp_timeout.tv_sec = 1;
    (void)setsockopt(udp_fd, SOL_SOCKET, SO_RCVTIMEO, &udp_timeout, sizeof(udp_timeout));

    if (dcc_voice_client_set_secret_key(voice_client, secret_key, secret_key_size) != DCC_OK ||
        !dcc_voice_client_has_secret_key(voice_client) ||
        dcc_voice_client_set_encryption_mode(
        voice_client,
        DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE
    ) != DCC_OK ||
        dcc_voice_client_encryption_mode(voice_client) !=
        DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE ||
        dcc_voice_client_set_packet_nonce(voice_client, 0x01020304U) != DCC_OK ||
        dcc_voice_client_packet_nonce(voice_client) != 0x01020304U ||
        dcc_voice_client_set_rtp_state(voice_client, 0x2000, 0x0a0b0c0d) != DCC_OK ||
        dcc_voice_client_connect_udp(voice_client, "127.0.0.1", ntohs(udp_addr.sin_port)) != DCC_OK ||
        dcc_voice_client_send_opus_packet(voice_client, queued_audio, 8, 20) != DCC_OK ||
        dcc_voice_client_drain_send_queue(voice_client, 1) != DCC_OK) {
        fprintf(stderr, "voice encrypted udp send setup failed\n");
        close(udp_fd);
        return 1;
    }

    uint8_t encrypted_udp_received[80];
    ssize_t encrypted_udp_read = recvfrom(
        udp_fd,
        encrypted_udp_received,
        sizeof(encrypted_udp_received),
        0,
        NULL,
        NULL
    );
    dcc_voice_client_disconnect_udp(voice_client);
    close(udp_fd);
    if (encrypted_udp_read != 40 ||
        encrypted_udp_received[0] != 0x80 ||
        encrypted_udp_received[1] != 0x78 ||
        encrypted_udp_received[2] != 0x20 ||
        encrypted_udp_received[3] != 0x00 ||
        encrypted_udp_received[4] != 0x0a ||
        encrypted_udp_received[5] != 0x0b ||
        encrypted_udp_received[6] != 0x0c ||
        encrypted_udp_received[7] != 0x0d ||
        encrypted_udp_received[8] != 0x00 ||
        encrypted_udp_received[9] != 0x01 ||
        encrypted_udp_received[10] != 0xe2 ||
        encrypted_udp_received[11] != 0x40 ||
        memcmp(encrypted_udp_received + 12, queued_audio, 8) == 0 ||
        encrypted_udp_received[36] != 0x01 ||
        encrypted_udp_received[37] != 0x02 ||
        encrypted_udp_received[38] != 0x03 ||
        encrypted_udp_received[39] != 0x04 ||
        dcc_voice_client_packet_nonce(voice_client) != 0x01020305U ||
        dcc_voice_client_rtp_sequence(voice_client) != 0x2001 ||
        dcc_voice_client_rtp_timestamp(voice_client) != 0x0a0b0fcd) {
        fprintf(stderr, "voice encrypted udp send validation failed\n");
        return 1;
    }

    if (dcc_voice_client_map_ssrc(voice_client, 123456, 444) != DCC_OK) {
        fprintf(stderr, "voice encrypted ssrc map failed\n");
        return 1;
    }
    decode_state->seen = 0;
    decode_state->expected_payload_size = 8;
    decode_state->expected_first_byte = 0x55;
    if (dcc_voice_client_set_decoder(voice_client, voice_decode_pcm, decode_state) != DCC_OK ||
        dcc_voice_client_receive_rtp_packet(
        voice_client,
        encrypted_udp_received,
        (size_t)encrypted_udp_read
    ) != DCC_OK ||
        !decode_state->seen ||
        decode_state->payload_size != 8) {
        fprintf(stderr, "voice encrypted rtp receive failed\n");
        return 1;
    }

    encrypted_udp_received[12] ^= 0x01U;
    if (dcc_voice_client_receive_rtp_packet(
        voice_client,
        encrypted_udp_received,
        (size_t)encrypted_udp_read
    ) != DCC_ERR_STATE) {
        fprintf(stderr, "voice encrypted rtp tamper check failed\n");
        return 1;
    }
    encrypted_udp_received[12] ^= 0x01U;
    dcc_voice_client_clear_secret_key(voice_client);
    if (dcc_voice_client_has_secret_key(voice_client) ||
        dcc_voice_client_set_encryption_mode(voice_client, DCC_VOICE_ENCRYPTION_NONE) != DCC_OK ||
        dcc_voice_client_encryption_mode(voice_client) != DCC_VOICE_ENCRYPTION_NONE) {
        fprintf(stderr, "voice encryption clear failed\n");
        return 1;
    }
    return 0;
}

#endif
