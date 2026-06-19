#ifndef DCC_VOICE_RECEIVE_STATE_INTERNAL_H
#define DCC_VOICE_RECEIVE_STATE_INTERNAL_H

#include "internal/voice/dcc_voice_limits_internal.h"

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

typedef struct dcc_voice_reorder_slot {
    uint8_t used;
    uint16_t sequence;
    uint64_t release_at_ms;
    size_t size;
    uint8_t packet[DCC_VOICE_REORDER_PACKET_CAP];
} dcc_voice_reorder_slot_t;

#define DCC_VOICE_RECEIVE_STATE_FIELDS \
    size_t mix_sample_count; \
    size_t mix_source_count; \
    uint32_t mix_ssrcs[DCC_VOICE_MIX_SOURCE_CAP]; \
    int32_t mix_accumulator[DCC_VOICE_MIX_SAMPLE_CAP]; \
    uint8_t receive_reorder_window; \
    uint32_t receive_jitter_delay_ms; \
    uint8_t receive_reorder_started; \
    uint16_t receive_expected_sequence; \
    uint32_t receive_packets_lost; \
    uint32_t receive_packets_dropped; \
    dcc_voice_reorder_slot_t receive_reorder[DCC_VOICE_REORDER_QUEUE_CAP]; \
    uint32_t receive_jitter_timer_interval_ms; \
    atomic_bool receive_jitter_timer_running; \
    atomic_bool receive_jitter_timer_stop; \
    atomic_bool receive_jitter_timer_completed; \
    atomic_flag receive_jitter_timer_lock; \
    _Atomic(uintptr_t) receive_jitter_timer_handle

#endif
