#ifndef DCC_EVENT_WAITER_STATE_INTERNAL_H
#define DCC_EVENT_WAITER_STATE_INTERNAL_H

#include <dcc/dcc.h>
#include <llam/runtime.h>

#include <stddef.h>
#include <stdint.h>

typedef enum dcc_event_wait_kind {
    DCC_EVENT_WAIT_SNAPSHOT = 0,
    DCC_EVENT_WAIT_GATEWAY_DATA,
    DCC_EVENT_WAIT_MESSAGE,
    DCC_EVENT_WAIT_GUILD,
    DCC_EVENT_WAIT_CHANNEL,
    DCC_EVENT_WAIT_INTERACTION,
    DCC_EVENT_WAIT_INTERACTION_OR_CLOSE,
    DCC_EVENT_WAIT_VOICE_STATE,
    DCC_EVENT_WAIT_MEMBER,
    DCC_EVENT_WAIT_ROLE,
    DCC_EVENT_WAIT_USER
} dcc_event_wait_kind_t;

#define DCC_EVENT_WAIT_MASK_WORDS (((size_t)DCC_EVENT_MAX + 63U) / 64U)

typedef struct dcc_event_waiter_node {
    dcc_event_type_t type;
    uint64_t type_mask[DCC_EVENT_WAIT_MASK_WORDS];
    uint64_t set_seen_mask[DCC_EVENT_WAIT_MASK_WORDS];
    const dcc_event_type_t *sequence;
    size_t sequence_count;
    size_t sequence_index;
    dcc_event_snapshot_t *sequence_snapshots;
    const dcc_event_type_t *set_types;
    size_t set_count;
    size_t set_completed;
    dcc_event_snapshot_t *set_snapshots;
    size_t count_target;
    size_t count_completed;
    dcc_event_snapshot_t *count_snapshots;
    dcc_event_wait_kind_t kind;
    llam_channel_t *done;
    dcc_event_snapshot_t snapshot;
    void *owned;
    dcc_status_t owned_status;
    uint8_t match_any;
    uint8_t match_sequence;
    uint8_t match_set;
    uint8_t match_count;
    uint8_t match_quorum;
    uint8_t completed;
    struct dcc_event_waiter_node *next;
} dcc_event_waiter_node_t;

#endif
