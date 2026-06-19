#include "internal/voice/dcc_voice_internal.h"

uint8_t dcc_voice_client_receive_jitter_timer_running(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL &&
        atomic_load_explicit(&voice_client->receive_jitter_timer_running, memory_order_acquire)
        ? 1U
        : 0U;
}

uint32_t dcc_voice_client_receive_jitter_timer_interval(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_jitter_timer_interval_ms : 0;
}

uint32_t dcc_voice_client_receive_packets_lost(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_packets_lost : 0;
}

uint32_t dcc_voice_client_receive_packets_dropped(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_packets_dropped : 0;
}
