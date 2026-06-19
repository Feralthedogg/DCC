#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>

void dcc_voice_u32_saturating_inc(uint32_t *value) {
    if (value != NULL && *value < UINT32_MAX) {
        (*value)++;
    }
}

int dcc_voice_sequence_before(uint16_t sequence, uint16_t expected) {
    return (int16_t)(sequence - expected) < 0;
}

uint16_t dcc_voice_sequence_distance(uint16_t expected, uint16_t sequence) {
    return (uint16_t)(sequence - expected);
}

uint64_t dcc_voice_jitter_release_at(uint64_t now_ms, uint32_t delay_ms) {
    return now_ms > UINT64_MAX - (uint64_t)delay_ms ? UINT64_MAX : now_ms + (uint64_t)delay_ms;
}
