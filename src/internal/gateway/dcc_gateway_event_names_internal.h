#ifndef DCC_GATEWAY_EVENT_NAMES_INTERNAL_H
#define DCC_GATEWAY_EVENT_NAMES_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_gateway_event_name_entry {
    const char *name;
    size_t len;
    dcc_event_type_t type;
} dcc_gateway_event_name_entry_t;

extern const dcc_gateway_event_name_entry_t dcc_gateway_event_names[];
extern const size_t dcc_gateway_event_name_count;

#ifdef __cplusplus
}
#endif

#endif
