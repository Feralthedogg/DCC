#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_soundboard_list_user_field(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_soundboard_buffers_t *buffers,
    int *handled
) {
    if (sound == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_USER) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &sound->user,
        buffers->username,
        buffers->username_cap,
        buffers->global_name,
        buffers->global_name_cap,
        buffers->avatar,
        buffers->avatar_cap
    );
    if (status != DCC_OK) {
        return status;
    }
    sound->user_id = sound->user.id;
    *handled = 1;
    return DCC_OK;
}
