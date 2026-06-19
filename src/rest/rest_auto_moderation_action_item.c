#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_auto_moderation_action(
    const dcc_auto_moderation_action_params_t *action,
    dcc_rest_buffer_t *body
) {
    if (action == NULL ||
        body == NULL ||
        action->size < sizeof(*action) ||
        action->type < DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE ||
        action->type > DCC_AUTO_MODERATION_ACTION_TIMEOUT) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "type", (uint64_t)action->type);
    }

    dcc_rest_buffer_t metadata = {0};
    int metadata_first = 1;
    if (status == DCC_OK) {
        switch (action->type) {
            case DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE:
                if (action->custom_message != NULL && action->custom_message[0] != '\0') {
                    status = dcc_rest_buffer_append_cstr(&metadata, "{");
                    if (status == DCC_OK) {
                        status = dcc_rest_json_append_string_member(
                            &metadata,
                            &metadata_first,
                            "custom_message",
                            action->custom_message
                        );
                    }
                }
                break;
            case DCC_AUTO_MODERATION_ACTION_SEND_ALERT:
                if (action->channel_id != 0) {
                    status = dcc_rest_buffer_append_cstr(&metadata, "{");
                    if (status == DCC_OK) {
                        status = dcc_rest_json_append_u64_string_member(
                            &metadata,
                            &metadata_first,
                            "channel_id",
                            action->channel_id
                        );
                    }
                }
                break;
            case DCC_AUTO_MODERATION_ACTION_TIMEOUT:
                if (action->duration_seconds != 0) {
                    status = dcc_rest_buffer_append_cstr(&metadata, "{");
                    if (status == DCC_OK) {
                        status = dcc_rest_json_append_u64_member(
                            &metadata,
                            &metadata_first,
                            "duration_seconds",
                            action->duration_seconds
                        );
                    }
                }
                break;
            default:
                status = DCC_ERR_INVALID_ARG;
                break;
        }
    }
    if (status == DCC_OK && metadata.len != 0) {
        status = dcc_rest_buffer_append_cstr(&metadata, "}");
    }
    if (status == DCC_OK && metadata.len != 0) {
        status = dcc_rest_json_append_raw_member(body, &first, "metadata", metadata.data);
    }
    dcc_rest_buffer_deinit(&metadata);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}
