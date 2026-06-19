#ifndef DCC_JSON_GATEWAY_EVENT_NAME_HELPERS_H
#define DCC_JSON_GATEWAY_EVENT_NAME_HELPERS_H

#include <stddef.h>

int dcc_json_event_prefix_eq(const char *name, size_t cap, const char *prefix, size_t prefix_len);
int dcc_json_event_name_eq(const char *name, size_t cap, const char *literal, size_t literal_len);

#endif
