#ifndef DCC_MESSAGE_BUILDER_SERIALIZE_INTERNAL_H
#define DCC_MESSAGE_BUILDER_SERIALIZE_INTERNAL_H

#include "internal/objects/dcc_message_json_buffer_internal.h"

#include <dcc/message.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_message_builder_validate_for_json(const dcc_message_builder_t *builder);
dcc_status_t dcc_allowed_mentions_validate_for_json(const dcc_allowed_mentions_builder_t *builder);
dcc_status_t dcc_allowed_mentions_append_json(
    const dcc_allowed_mentions_builder_t *builder,
    dcc_message_json_buffer_t *buffer
);
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
dcc_status_t dcc_message_builder_append_members_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
);
dcc_status_t dcc_message_builder_append_json(
    dcc_message_json_buffer_t *buffer,
    const dcc_message_builder_t *builder
);
dcc_status_t dcc_message_reference_validate(
    const dcc_message_reference_t *reference,
    dcc_message_reference_type_t *out_type
);
dcc_status_t dcc_message_attachment_array_validate(
    const dcc_message_attachment_t *attachments,
    size_t attachment_count,
    size_t *out_stride
);
dcc_status_t dcc_message_reference_append_json(
    const dcc_message_reference_t *reference,
    dcc_message_json_buffer_t *buffer
);
dcc_status_t dcc_message_attachments_append_json(
    const dcc_message_attachment_t *attachments,
    size_t attachment_count,
    dcc_message_json_buffer_t *buffer
);
dcc_status_t dcc_message_builder_measure_json(
    const dcc_message_builder_t *builder,
    size_t *out_json_len
);

#ifdef __cplusplus
}
#endif

#endif
