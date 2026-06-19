#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_interaction_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_APP_PERMISSIONS) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.app_permissions = has_value ? value : 0U;
        out->has_interaction = 1;
    } else if (key_id == DCC_JSON_KEY_ATTACHMENT_SIZE_LIMIT) {
        uint32_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u32_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.attachment_size_limit =
            has_value && value != 0U ? value : DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT;
        out->has_interaction = 1;
    } else if (key_id == DCC_JSON_KEY_VERSION) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (has_value && value <= UINT32_MAX) {
            out->interaction.version = (uint32_t)value;
            out->has_interaction = 1;
        }
    } else if (key_id == DCC_JSON_KEY_CONTEXT) {
        uint32_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u32_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.context = has_value ? value : 0U;
        out->interaction.has_context = has_value ? 1U : 0U;
        out->has_interaction = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
