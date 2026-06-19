#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_soundboard_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (sound == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_VOLUME) {
        double value = 0.0;
        dcc_status_t status = dcc_json_parse_double_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        sound->volume = value;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_AVAILABLE) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        sound->available = value ? 1U : 0U;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
