#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_guild_member_body(
    const dcc_guild_member_params_t *params,
    int include_access_token,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        params->user_id == 0 ||
        ((params->flags & DCC_GUILD_MEMBER_UPDATE_ROLES) != 0 &&
         params->role_count != 0 &&
         params->roles == NULL) ||
        (include_access_token && (params->access_token == NULL || params->access_token[0] == '\0'))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK && (params->flags & DCC_GUILD_MEMBER_UPDATE_TIMEOUT) != 0) {
        status = dcc_rest_json_append_nullable_string_member(
            &body,
            &first,
            "communication_disabled_until",
            params->communication_disabled_until
        );
    }
    if (status == DCC_OK && (params->flags & DCC_GUILD_MEMBER_UPDATE_NICK) != 0) {
        status = dcc_rest_json_append_nullable_string_member(&body, &first, "nick", params->nick);
    }
    if (status == DCC_OK && (params->flags & DCC_GUILD_MEMBER_UPDATE_ROLES) != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body,
            &first,
            "roles",
            params->roles,
            params->role_count
        );
    }
    if (status == DCC_OK && (params->flags & DCC_GUILD_MEMBER_UPDATE_VOICE) != 0) {
        status = dcc_rest_json_append_bool_member(&body, &first, "mute", params->mute);
    }
    if (status == DCC_OK && (params->flags & DCC_GUILD_MEMBER_UPDATE_VOICE) != 0) {
        status = dcc_rest_json_append_bool_member(&body, &first, "deaf", params->deaf);
    }
    if (status == DCC_OK && include_access_token) {
        status = dcc_rest_json_append_string_member(&body, &first, "access_token", params->access_token);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
