#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_soundboard_list_text_field(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_soundboard_buffers_t *buffers,
    int *handled
) {
    if (sound == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;
    if (key_id == DCC_JSON_KEY_NAME) {
        target = buffers->name;
        target_cap = buffers->name_cap;
        field = &sound->name;
    } else if (key_id == DCC_JSON_KEY_EMOJI_NAME) {
        target = buffers->emoji_name;
        target_cap = buffers->emoji_name_cap;
        field = &sound->emoji_name;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        target,
        target_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    *field = has_value ? target : NULL;
    *handled = 1;
    return DCC_OK;
}
