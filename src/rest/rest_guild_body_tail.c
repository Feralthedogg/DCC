#include "internal/rest/dcc_rest_guild_body_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_guild_body_append_tail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_guild_params_t *params
) {
    dcc_status_t status = DCC_OK;
    if (params->safety_alerts_channel_id != 0) {
        status = dcc_rest_json_append_u64_string_member(
            body,
            first,
            "safety_alerts_channel_id",
            params->safety_alerts_channel_id
        );
    }
    if (status == DCC_OK && params->banner != NULL && params->banner[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, first, "banner", params->banner);
    }
    if (status == DCC_OK && params->discovery_splash != NULL && params->discovery_splash[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, first, "discovery_splash", params->discovery_splash);
    }
    if (status == DCC_OK && params->splash != NULL && params->splash[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, first, "splash", params->splash);
    }
    if (status == DCC_OK && params->icon != NULL && params->icon[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, first, "icon", params->icon);
    }
    return status;
}
