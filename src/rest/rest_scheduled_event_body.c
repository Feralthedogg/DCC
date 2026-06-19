#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_scheduled_event_append_body(
    dcc_rest_buffer_t *body,
    const dcc_rest_scheduled_event_body_context_t *context
) {
    if (body == NULL || context == NULL || context->params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_scheduled_event_params_t *params = context->params;
    dcc_status_t build_status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (build_status == DCC_OK && context->include_id != 0) {
        build_status = dcc_rest_json_append_u64_string_member(body, &first, "id", params->event_id);
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_string_member(body, &first, "name", params->name);
    }
    if (build_status == DCC_OK && params->description != NULL && params->description[0] != '\0') {
        build_status = dcc_rest_json_append_string_member(body, &first, "description", params->description);
    }
    if (build_status == DCC_OK && params->image != NULL && params->image[0] != '\0') {
        build_status = dcc_rest_json_append_string_member(body, &first, "image", params->image);
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_u64_member(body, &first, "privacy_level", context->privacy_level);
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_u64_member(body, &first, "status", context->status_value);
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_u64_member(body, &first, "entity_type", context->entity_type);
    }
    if (build_status == DCC_OK && params->entity_id != 0) {
        build_status = dcc_rest_json_append_u64_string_member(body, &first, "entity_id", params->entity_id);
    }
    if (build_status == DCC_OK && params->channel_id != 0) {
        build_status = dcc_rest_json_append_u64_string_member(body, &first, "channel_id", params->channel_id);
    } else if (build_status == DCC_OK && context->include_id != 0) {
        build_status = dcc_rest_json_append_raw_member(body, &first, "channel_id", "null");
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_u64_string_member(body, &first, "guild_id", params->guild_id);
    }
    if (build_status == DCC_OK && params->creator_id != 0) {
        build_status = dcc_rest_json_append_u64_string_member(body, &first, "creator_id", params->creator_id);
    }
    if (build_status == DCC_OK &&
        params->scheduled_start_time != NULL &&
        params->scheduled_start_time[0] != '\0') {
        build_status = dcc_rest_json_append_string_member(
            body,
            &first,
            "scheduled_start_time",
            params->scheduled_start_time
        );
    }
    if (build_status == DCC_OK &&
        params->scheduled_end_time != NULL &&
        params->scheduled_end_time[0] != '\0') {
        build_status = dcc_rest_json_append_string_member(
            body,
            &first,
            "scheduled_end_time",
            params->scheduled_end_time
        );
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_json_member_sep(body, &first);
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_buffer_append_cstr(body, "\"entity_metadata\":{");
    }
    int metadata_first = 1;
    if (build_status == DCC_OK && params->location != NULL && params->location[0] != '\0') {
        build_status = dcc_rest_json_append_string_member(body, &metadata_first, "location", params->location);
    } else if (build_status == DCC_OK) {
        build_status = dcc_rest_json_append_raw_member(body, &metadata_first, "location", "null");
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_buffer_append_cstr(body, "}");
    }
    if (build_status == DCC_OK) {
        build_status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return build_status;
}
