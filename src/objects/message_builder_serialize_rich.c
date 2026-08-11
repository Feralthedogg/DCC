#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_component_serialize_internal.h"
#include "internal/objects/dcc_embed_builder_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>

static dcc_status_t dcc_message_json_append_measured_raw_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    size_t value_len
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status == DCC_OK) {
        status = dcc_message_json_append_length(buffer, value_len);
    }
    return status;
}

dcc_status_t dcc_message_builder_append_rich_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_status_t status = DCC_OK;
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) && builder->embeds_count != 0U) {
        if (buffer->count_only != 0U) {
            size_t embeds_len = 0U;
            status = dcc_embed_builder_measure_array_json(
                builder->embeds, builder->embeds_count, &embeds_len
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_measured_raw_member(
                    buffer, first, "embeds", embeds_len
                );
            }
        } else {
            char *embeds_json = NULL;
            status = dcc_embed_builder_build_array_json(
                builder->embeds, builder->embeds_count, &embeds_json
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_raw_member(
                    buffer, first, "embeds", embeds_json
                );
            }
            dcc_embed_builder_json_free(embeds_json);
        }
    } else if (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "embeds", builder->embeds_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) &&
        builder->components_v2_count != 0U) {
        status = dcc_component_v2_validate_array_context(
            builder->components_v2,
            builder->components_v2_count,
            DCC_COMPONENT_V2_CONTEXT_MESSAGE
        );
        if (status == DCC_OK && buffer->count_only != 0U) {
            size_t components_len = 0U;
            status = dcc_component_v2_builder_measure_array_json(
                builder->components_v2, builder->components_v2_count,
                &components_len
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_measured_raw_member(
                    buffer, first, "components", components_len
                );
            }
        } else if (status == DCC_OK) {
            char *components_json = NULL;
            status = dcc_component_v2_builder_build_array_json(
                builder->components_v2, builder->components_v2_count,
                &components_json
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_raw_member(
                    buffer, first, "components", components_json
                );
            }
            dcc_component_v2_builder_json_free(components_json);
        }
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "components", builder->components_v2_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) &&
        builder->components_count != 0U) {
        if (buffer->count_only != 0U) {
            size_t components_len = 0U;
            status = dcc_component_builder_measure_array_json(
                builder->components, builder->components_count,
                &components_len
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_measured_raw_member(
                    buffer, first, "components", components_len
                );
            }
        } else {
            char *components_json = NULL;
            status = dcc_component_builder_build_array_json(
                builder->components, builder->components_count,
                &components_json
            );
            if (status == DCC_OK) {
                status = dcc_message_json_append_raw_member(
                    buffer, first, "components", components_json
                );
            }
            dcc_component_builder_json_free(components_json);
        }
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "components", builder->components_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "attachments", builder->attachments_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS)) {
        status = dcc_message_json_member_prefix(buffer, first, "attachments");
        if (status == DCC_OK) {
            status = dcc_message_attachments_append_json(
                builder->attachments, builder->attachment_count, buffer
            );
        }
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL)) {
        status = dcc_message_json_member_prefix(buffer, first, "poll");
        if (status == DCC_OK) {
            status = dcc_message_poll_append_json(builder->poll, buffer);
        }
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "poll", builder->poll_json);
    }
    #undef HAS
    return status;
}
