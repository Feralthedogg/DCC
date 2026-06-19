#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_auto_moderation_trigger_metadata(
    const dcc_auto_moderation_metadata_params_t *metadata,
    dcc_rest_buffer_t *body
) {
    if (metadata == NULL || body == NULL || metadata->size < sizeof(*metadata)) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((metadata->keyword_count != 0 && metadata->keywords == NULL) ||
        (metadata->regex_pattern_count != 0 && metadata->regex_patterns == NULL) ||
        (metadata->preset_count != 0 && metadata->presets == NULL) ||
        (metadata->allow_list_count != 0 && metadata->allow_list == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_array_member(
            body,
            &first,
            "keyword_filter",
            metadata->keywords,
            metadata->keyword_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_array_member(
            body,
            &first,
            "regex_patterns",
            metadata->regex_patterns,
            metadata->regex_pattern_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_automod_preset_array_member(
            body,
            &first,
            "presets",
            metadata->presets,
            metadata->preset_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_array_member(
            body,
            &first,
            "allow_list",
            metadata->allow_list,
            metadata->allow_list_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "mention_total_limit", metadata->mention_total_limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(
            body,
            &first,
            "mention_raid_protection_enabled",
            metadata->mention_raid_protection_enabled
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}
