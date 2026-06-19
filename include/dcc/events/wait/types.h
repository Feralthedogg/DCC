#ifndef DCC_EVENTS_WAIT_TYPES_H
#define DCC_EVENTS_WAIT_TYPES_H

#include <dcc/events/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_event_snapshot {
    size_t size;
    dcc_event_type_t type;
    uint32_t shard_id;
    uint8_t cancelled;
    dcc_event_type_t raw_type;
    uint64_t raw_sequence;
    size_t raw_json_len;
    char name[96];
    char raw_name[96];
} dcc_event_snapshot_t;

typedef enum dcc_event_wait_policy_mode {
    DCC_EVENT_WAIT_POLICY_ANY = 1,
    DCC_EVENT_WAIT_POLICY_SEQUENCE = 2,
    DCC_EVENT_WAIT_POLICY_SET = 3,
    DCC_EVENT_WAIT_POLICY_COUNT = 4,
    DCC_EVENT_WAIT_POLICY_QUORUM = 5
} dcc_event_wait_policy_mode_t;

typedef struct dcc_event_wait_policy {
    size_t size;
    dcc_event_wait_policy_mode_t mode;
    const dcc_event_type_t *types;
    size_t type_count;
    size_t target_count;
    uint32_t timeout_ms;
} dcc_event_wait_policy_t;

#ifdef __cplusplus
}
#endif

#endif
