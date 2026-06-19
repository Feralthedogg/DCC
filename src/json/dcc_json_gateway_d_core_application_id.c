#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_application_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    out->interaction.application_id = value;
    out->entitlement.application_id = value;
    out->integration.application_id = value;
    out->application_command_permissions_update.application_id = value;
    out->guild_application_command_update.application_id = value;
    out->embedded_activity_update.application_id = value;
    out->gateway.application_id = value;
    out->guild.application_id = value;
    out->has_interaction = 1;
    out->has_entitlement = 1;
    out->has_integration = 1;
    out->has_application_command_permissions_update = 1;
    out->has_guild_application_command_update = 1;
    out->has_embedded_activity_update = 1;
    out->has_guild = 1;
    (void)has_value;
    return DCC_OK;
}
