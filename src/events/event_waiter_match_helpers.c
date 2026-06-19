#include "internal/events/dcc_event_waiter_match_internal.h"

#include <stdio.h>
#include <string.h>

static void dcc_event_copy_text(char *out, size_t out_cap, const char *value) {
    if (out == NULL || out_cap == 0) {
        return;
    }
    if (value == NULL) {
        out[0] = '\0';
        return;
    }
    snprintf(out, out_cap, "%s", value);
}

void dcc_event_snapshot_fill(dcc_event_snapshot_t *snapshot, const dcc_event_t *event) {
    if (snapshot == NULL || event == NULL) {
        return;
    }
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->size = sizeof(*snapshot);
    snapshot->type = event->type;
    snapshot->shard_id = event->shard_id;
    snapshot->cancelled = event->cancelled;
    snapshot->raw_type = event->raw.type;
    snapshot->raw_sequence = event->raw.sequence;
    snapshot->raw_json_len = event->raw.json_len;
    const char *name = event->raw.name != NULL
        ? event->raw.name
        : dcc_event_type_name(event->type);
    dcc_event_copy_text(snapshot->name, sizeof(snapshot->name), name);
    dcc_event_copy_text(snapshot->raw_name, sizeof(snapshot->raw_name), event->raw.name);
}

void dcc_event_snapshot_copy(dcc_event_snapshot_t *out, const dcc_event_snapshot_t *snapshot) {
    if (out == NULL || snapshot == NULL) {
        return;
    }
    size_t out_size = out->size;
    size_t copy_size = out_size < sizeof(*snapshot) ? out_size : sizeof(*snapshot);
    memcpy(out, snapshot, copy_size);
    out->size = out_size;
}

uint8_t dcc_event_type_valid(dcc_event_type_t type) {
    return type >= 0 && type < DCC_EVENT_MAX;
}

void dcc_event_waiter_add_type(dcc_event_waiter_node_t *waiter, dcc_event_type_t type) {
    if (waiter == NULL || !dcc_event_type_valid(type)) {
        return;
    }
    size_t index = (size_t)type;
    waiter->type_mask[index / 64U] |= UINT64_C(1) << (index % 64U);
}

uint8_t dcc_event_waiter_mask_has(
    const uint64_t mask[DCC_EVENT_WAIT_MASK_WORDS],
    dcc_event_type_t type
) {
    if (mask == NULL || !dcc_event_type_valid(type)) {
        return 0;
    }
    size_t index = (size_t)type;
    return (uint8_t)((mask[index / 64U] >> (index % 64U)) & UINT64_C(1));
}

void dcc_event_waiter_mask_add(
    uint64_t mask[DCC_EVENT_WAIT_MASK_WORDS],
    dcc_event_type_t type
) {
    if (mask == NULL || !dcc_event_type_valid(type)) {
        return;
    }
    size_t index = (size_t)type;
    mask[index / 64U] |= UINT64_C(1) << (index % 64U);
}
