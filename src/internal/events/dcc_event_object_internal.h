#ifndef DCC_EVENT_OBJECT_INTERNAL_H
#define DCC_EVENT_OBJECT_INTERNAL_H

#include "internal/events/dcc_event_object_data_internal.h"

#include <dcc/dcc.h>

#include <stdint.h>

struct dcc_event {
    dcc_event_type_t type;
    uint32_t shard_id;
    uint8_t cancelled;
    dcc_raw_event_t raw;
    const dcc_gateway_event_data_t *gateway;
    uint64_t cache_fields;
    dcc_event_object_data_t data;
};

#endif
