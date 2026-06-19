#ifndef DCC_MESSAGE_JSON_MEMBERS_INTERNAL_H
#define DCC_MESSAGE_JSON_MEMBERS_INTERNAL_H

#include "internal/objects/dcc_message_json_buffer_internal.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_message_json_member_prefix(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_message_json_append_string_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_message_json_append_u64_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint64_t value
);
dcc_status_t dcc_message_json_append_bool_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint8_t value
);
dcc_status_t dcc_message_json_append_raw_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
