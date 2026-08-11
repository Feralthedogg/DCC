#ifndef DCC_BUILDER_ABI_INTERNAL_H
#define DCC_BUILDER_ABI_INTERNAL_H

#include <dcc/application_command.h>
#include <dcc/autocomplete.h>
#include <dcc/embed.h>
#include <dcc/message.h>
#include <dcc/modal.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_builder_abi_view {
    size_t size;
    uint32_t version;
    uint64_t present;
} dcc_builder_abi_view_t;

int dcc_builder_abi_field_covered(size_t size, size_t offset, size_t width);
int dcc_builder_abi_view_has(const dcc_builder_abi_view_t *view, uint64_t bit);
dcc_status_t dcc_builder_abi_array_begin(
    const void *values,
    size_t count,
    size_t alignment,
    size_t minimum_size,
    size_t expected_version,
    uint64_t known_bits,
    size_t *out_stride
);
const void *dcc_builder_abi_array_at(const void *values, size_t stride, size_t index);

dcc_status_t dcc_message_builder_abi_validate(
    const dcc_message_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_message_reference_abi_validate(
    const dcc_message_reference_t *reference,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_message_attachment_abi_validate(
    const dcc_message_attachment_t *attachment,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_message_attachment_array_begin(
    const dcc_message_attachment_t *values,
    size_t count,
    size_t *out_stride
);
dcc_status_t dcc_poll_builder_abi_validate(
    const dcc_poll_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_allowed_mentions_builder_abi_validate(
    const dcc_allowed_mentions_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_embed_builder_abi_validate(
    const dcc_embed_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_modal_builder_abi_validate(
    const dcc_modal_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_autocomplete_choice_abi_validate(
    const dcc_autocomplete_choice_t *choice,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_autocomplete_builder_abi_validate(
    const dcc_autocomplete_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_application_command_option_builder_abi_validate(
    const dcc_application_command_option_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_application_command_builder_abi_validate(
    const dcc_application_command_builder_t *builder,
    dcc_builder_abi_view_t *out
);
dcc_status_t dcc_embed_builder_array_begin(
    const dcc_embed_builder_t *values,
    size_t count,
    size_t *out_stride
);
dcc_status_t dcc_autocomplete_choice_array_begin(
    const dcc_autocomplete_choice_t *values,
    size_t count,
    size_t *out_stride
);
dcc_status_t dcc_application_command_option_builder_array_begin(
    const dcc_application_command_option_builder_t *values,
    size_t count,
    size_t *out_stride
);
dcc_status_t dcc_application_command_builder_array_begin(
    const dcc_application_command_builder_t *values,
    size_t count,
    size_t *out_stride
);

#define DCC_BUILDER_ABI_FIELD(type_, view_, field_) \
    dcc_builder_abi_field_covered( \
        (view_)->size, offsetof(type_, field_), sizeof(((type_ *)0)->field_) \
    )

#define DCC_BUILDER_ABI_MINIMUM(type_) \
    (offsetof(type_, present) + sizeof(((type_ *)0)->present))

#endif
