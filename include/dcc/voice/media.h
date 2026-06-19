#ifndef DCC_VOICE_MEDIA_H
#define DCC_VOICE_MEDIA_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_voice_client_receive_pcm(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const uint8_t *audio,
    size_t audio_size
);
DCC_API dcc_status_t dcc_voice_client_receive_combined_pcm(
    dcc_voice_client_t *voice_client,
    const uint8_t *audio,
    size_t audio_size
);
DCC_API dcc_status_t dcc_voice_client_mix_pcm(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const int16_t *pcm,
    size_t pcm_sample_count
);
DCC_API dcc_status_t dcc_voice_client_flush_mixed_pcm(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_clear_mixed_pcm(dcc_voice_client_t *voice_client);
DCC_API size_t dcc_voice_client_mixed_source_count(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_decoder(
    dcc_voice_client_t *voice_client,
    dcc_voice_decode_fn decoder,
    void *decoder_user_data
);
DCC_API dcc_status_t dcc_voice_client_set_encoder(
    dcc_voice_client_t *voice_client,
    dcc_voice_encode_fn encoder,
    void *encoder_user_data
);
DCC_API dcc_status_t dcc_voice_client_enable_opus(
    dcc_voice_client_t *voice_client,
    const dcc_voice_opus_options_t *options
);
DCC_API dcc_status_t dcc_voice_client_disable_opus(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_receive_rtp_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size
);
DCC_API dcc_status_t dcc_voice_client_receive_rtp_packet_at(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size,
    uint64_t now_ms
);
DCC_API dcc_status_t dcc_voice_client_set_receive_reorder_window(
    dcc_voice_client_t *voice_client,
    uint8_t window_packets
);
DCC_API uint8_t dcc_voice_client_receive_reorder_window(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_receive_jitter_delay(
    dcc_voice_client_t *voice_client,
    uint32_t delay_ms
);
DCC_API uint32_t dcc_voice_client_receive_jitter_delay(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_tick_receive_jitter(dcc_voice_client_t *voice_client, uint64_t now_ms);
DCC_API dcc_status_t dcc_voice_client_start_receive_jitter_timer(
    dcc_voice_client_t *voice_client,
    uint32_t interval_ms
);
DCC_API dcc_status_t dcc_voice_client_stop_receive_jitter_timer(dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_receive_jitter_timer_running(const dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_receive_jitter_timer_interval(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_flush_receive_reorder(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_clear_receive_reorder(dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_receive_packets_lost(const dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_receive_packets_dropped(const dcc_voice_client_t *voice_client);

#ifdef __cplusplus
}
#endif

#endif
