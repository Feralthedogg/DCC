#include "internal/voice/dcc_voice_internal.h"
#include "internal/dcc_platform_datagram.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_voice_client_send_queued_packet(
    dcc_voice_client_t *voice_client,
    const dcc_voice_send_item_t *item,
    uint16_t sequence,
    uint32_t timestamp
) {
    if (voice_client == NULL || item == NULL || item->type != DCC_VOICE_SEND_ITEM_PACKET) {
        return DCC_ERR_INVALID_ARG;
    }
    if (LLAM_FD_IS_INVALID(voice_client->udp_fd)) {
        return DCC_OK;
    }
    if (voice_client->ssrc == 0 || item->size > SIZE_MAX - DCC_VOICE_RTP_HEADER_SIZE) {
        return DCC_ERR_STATE;
    }

    int encrypt_packet =
        voice_client->encryption_mode == DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE;
    if (encrypt_packet && !voice_client->has_secret_key) {
        return DCC_ERR_STATE;
    }

    size_t rtp_size = DCC_VOICE_RTP_HEADER_SIZE + item->size;
    if (encrypt_packet) {
        if (item->size > SIZE_MAX - DCC_VOICE_RTP_HEADER_SIZE - DCC_VOICE_AEAD_TAG_SIZE -
                DCC_VOICE_RTPSIZE_NONCE_SIZE) {
            return DCC_ERR_STATE;
        }
        rtp_size = DCC_VOICE_RTP_HEADER_SIZE + item->size + DCC_VOICE_AEAD_TAG_SIZE +
            DCC_VOICE_RTPSIZE_NONCE_SIZE;
    }

    uint8_t *rtp = (uint8_t *)malloc(rtp_size);
    if (rtp == NULL) {
        return DCC_ERR_NOMEM;
    }

    rtp[0] = 0x80U;
    rtp[1] = DCC_VOICE_RTP_PAYLOAD_TYPE;
    dcc_voice_write_be16(rtp + 2U, sequence);
    dcc_voice_write_be32(rtp + 4U, timestamp);
    dcc_voice_write_be32(rtp + 8U, voice_client->ssrc);
    if (encrypt_packet) {
        uint8_t nonce[DCC_VOICE_XCHACHA20_NONCE_SIZE];
        memset(nonce, 0, sizeof(nonce));
        dcc_voice_write_be32(nonce, voice_client->packet_nonce);
        size_t encrypted_len = 0;
        dcc_status_t crypto_status = dcc_voice_xchacha20poly1305(
            1,
            rtp + DCC_VOICE_RTP_HEADER_SIZE,
            rtp_size - DCC_VOICE_RTP_HEADER_SIZE - DCC_VOICE_RTPSIZE_NONCE_SIZE,
            &encrypted_len,
            item->data,
            item->size,
            rtp,
            DCC_VOICE_RTP_HEADER_SIZE,
            nonce,
            voice_client->secret_key
        );
        if (crypto_status != DCC_OK || encrypted_len != item->size + DCC_VOICE_AEAD_TAG_SIZE) {
            free(rtp);
            return crypto_status != DCC_OK ? crypto_status : DCC_ERR_STATE;
        }
        memcpy(
            rtp + DCC_VOICE_RTP_HEADER_SIZE + encrypted_len,
            nonce,
            DCC_VOICE_RTPSIZE_NONCE_SIZE
        );
    } else {
        memcpy(rtp + DCC_VOICE_RTP_HEADER_SIZE, item->data, item->size);
    }

    ssize_t sent = dcc_platform_sendto(
        voice_client->udp_fd,
        rtp,
        rtp_size,
        0,
        (const struct sockaddr *)&voice_client->udp_addr,
        voice_client->udp_addr_len
    );
    free(rtp);
    return sent < 0 || (size_t)sent != rtp_size ? DCC_ERR_NETWORK : DCC_OK;
}
