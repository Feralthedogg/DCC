#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_soundboard_list_id_field(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (sound == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t *target = NULL;
    int sync_user = 0;
    if (key_id == DCC_JSON_KEY_ID || key_id == DCC_JSON_KEY_SOUND_ID) {
        target = &sound->id;
    } else if (key_id == DCC_JSON_KEY_GUILD_ID) {
        target = &sound->guild_id;
    } else if (key_id == DCC_JSON_KEY_USER_ID) {
        target = &sound->user_id;
        sync_user = 1;
    } else if (key_id == DCC_JSON_KEY_EMOJI_ID) {
        target = &sound->emoji_id;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    *target = has_value ? value : 0U;
    if (sync_user) {
        sound->user.id = sound->user_id;
    }
    *handled = 1;
    return DCC_OK;
}
