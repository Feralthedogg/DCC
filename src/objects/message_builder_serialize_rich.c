#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>

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
        char *embeds_json = NULL;
        status = dcc_embed_builder_build_array_json(builder->embeds, builder->embeds_count, &embeds_json);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "embeds", embeds_json);
        dcc_embed_builder_json_free(embeds_json);
    } else if (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "embeds", builder->embeds_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) &&
        builder->components_v2_count != 0U) {
        char *components_json = NULL;
        status = dcc_component_v2_validate_array_context(
            builder->components_v2,
            builder->components_v2_count,
            DCC_COMPONENT_V2_CONTEXT_MESSAGE
        );
        if (status == DCC_OK) {
            status = dcc_component_v2_builder_build_array_json(
                builder->components_v2,
                builder->components_v2_count,
                &components_json
            );
        }
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "components", components_json);
        dcc_component_v2_builder_json_free(components_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "components", builder->components_v2_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) &&
        builder->components_count != 0U) {
        char *components_json = NULL;
        status = dcc_component_builder_build_array_json(
            builder->components,
            builder->components_count,
            &components_json
        );
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "components", components_json);
        dcc_component_builder_json_free(components_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "components", builder->components_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "attachments", builder->attachments_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL)) {
        char *poll_json = NULL;
        status = dcc_message_poll_builder_build_json(builder->poll, &poll_json);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "poll", poll_json);
        dcc_message_poll_builder_json_free(poll_json);
    } else if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "poll", builder->poll_json);
    }
    #undef HAS
    return status;
}
