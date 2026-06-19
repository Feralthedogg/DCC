#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"

#include <dcc/component.h>
#include <dcc/embed.h>

dcc_status_t dcc_message_builder_append_rich_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
) {
    dcc_status_t status = DCC_OK;
    if (builder->embeds_count != 0) {
        char *embeds_json = NULL;
        status = dcc_embed_builder_build_array_json(builder->embeds, builder->embeds_count, &embeds_json);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "embeds", embeds_json);
        dcc_embed_builder_json_free(embeds_json);
    } else if (builder->embeds_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "embeds", builder->embeds_json);
    }
    if (status == DCC_OK && builder->components_count != 0) {
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
    } else if (status == DCC_OK && builder->components_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "components", builder->components_json);
    }
    if (status == DCC_OK && builder->attachments_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "attachments", builder->attachments_json);
    }
    if (status == DCC_OK && builder->poll != NULL) {
        char *poll_json = NULL;
        status = dcc_message_poll_builder_build_json(builder->poll, &poll_json);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_message_json_append_raw_member(buffer, first, "poll", poll_json);
        dcc_message_poll_builder_json_free(poll_json);
    } else if (status == DCC_OK && builder->poll_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "poll", builder->poll_json);
    }
    return status;
}
