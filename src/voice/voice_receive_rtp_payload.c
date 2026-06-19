#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>
#include <string.h>

dcc_status_t dcc_voice_rtp_media_view_parse(
    dcc_voice_rtp_media_view_t *view,
    dcc_voice_encryption_mode_t encryption_mode,
    const uint8_t *packet,
    size_t packet_size
) {
    if (view == NULL || packet == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    memset(view, 0, sizeof(*view));
    view->packet = packet;
    view->packet_size = packet_size;

    if (packet_size < DCC_VOICE_RTP_HEADER_SIZE || (packet[0] >> 6U) != 2U) {
        return DCC_ERR_INVALID_ARG;
    }

    uint8_t payload_type = (uint8_t)(packet[1] & 0x7FU);
    if (payload_type >= 72U && payload_type <= 76U) {
        return DCC_OK;
    }

    size_t csrc_count = (size_t)(packet[0] & 0x0FU);
    if (csrc_count > (SIZE_MAX - DCC_VOICE_RTP_HEADER_SIZE) / 4U) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t header_size = DCC_VOICE_RTP_HEADER_SIZE + csrc_count * 4U;
    if (header_size > packet_size) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t aad_size = header_size;
    size_t encrypted_extension_size = 0;
    if ((packet[0] & 0x10U) != 0) {
        if (packet_size - header_size < 4U) {
            return DCC_ERR_INVALID_ARG;
        }

        size_t extension_words = dcc_voice_read_be16(packet + header_size + 2U);
        if (extension_words > (SIZE_MAX - header_size - 4U) / 4U) {
            return DCC_ERR_INVALID_ARG;
        }

        encrypted_extension_size = extension_words * 4U;
        if (encryption_mode == DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE) {
            aad_size = header_size + 4U;
            if (aad_size > packet_size) {
                return DCC_ERR_INVALID_ARG;
            }
            header_size = aad_size;
        } else {
            header_size += 4U + encrypted_extension_size;
        }
        if (header_size > packet_size) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    size_t payload_size = packet_size - header_size;
    if ((packet[0] & 0x20U) != 0) {
        if (payload_size == 0) {
            return DCC_ERR_INVALID_ARG;
        }
        size_t padding_size = packet[packet_size - 1U];
        if (padding_size == 0 || padding_size > payload_size) {
            return DCC_ERR_INVALID_ARG;
        }
        payload_size -= padding_size;
    }

    view->payload = packet + header_size;
    view->payload_size = payload_size;
    view->aad_size = aad_size;
    view->encrypted_extension_size = encrypted_extension_size;
    view->ssrc = dcc_voice_read_be32(packet + 8U);
    view->should_decode = 1;
    return DCC_OK;
}
