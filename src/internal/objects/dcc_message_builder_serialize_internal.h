#ifndef DCC_MESSAGE_BUILDER_SERIALIZE_INTERNAL_H
#define DCC_MESSAGE_BUILDER_SERIALIZE_INTERNAL_H

#include "internal/objects/dcc_message_json_buffer_internal.h"

#include <dcc/message.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_message_builder_validate_for_json(const dcc_message_builder_t *builder);
dcc_status_t dcc_message_builder_append_basic_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
);
dcc_status_t dcc_message_builder_append_rich_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
);

#ifdef __cplusplus
}
#endif

#endif
