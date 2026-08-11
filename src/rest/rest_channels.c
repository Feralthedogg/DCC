#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t channel_prepare(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_endpoint_audit_policy_t audit, dcc_rest_request_t **out_request,
    dcc_rest_call_options_t *resolved
) {
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, resolved, DCC_ENDPOINT_AUTH_POLICY_BOT, audit
    );
    if (status != DCC_OK || client == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    return DCC_OK;
}

dcc_status_t dcc_rest_get_channel(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CHANNEL_GET, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = channel_prepare(
        client, options, DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved
    );
    if (status != DCC_OK || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[64];
    status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu",
        (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_get_channel", DCC_REST_GET, path, NULL, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}

dcc_status_t dcc_rest_create_guild_channel(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_channel_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_CREATE, DCC_REST_POST);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = channel_prepare(
        client, options, DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved
    );
    if (status != DCC_OK || guild_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *json = NULL;
    status = dcc_rest_build_channel_body(params, 0U, &json);
    char path[80];
    if (status == DCC_OK) status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/channels", (unsigned long long)guild_id
    );
    dcc_endpoint_body_t body = {
        json, json != NULL ? strlen(json) : 0U, "application/json"
    };
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_create_guild_channel", DCC_REST_POST, path, &body,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(json);
    return status;
}

dcc_status_t dcc_rest_get_guild_channels(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CHANNELS_GET, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = channel_prepare(
        client, options, DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved
    );
    if (status != DCC_OK || guild_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[80];
    status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/channels", (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_get_guild_channels", DCC_REST_GET, path, NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}

dcc_status_t dcc_rest_modify_channel(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_channel_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_EDIT, DCC_REST_PATCH);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = channel_prepare(
        client, options, DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved
    );
    if (status != DCC_OK || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *json = NULL;
    status = dcc_rest_build_channel_body(params, 1U, &json);
    char path[64];
    if (status == DCC_OK) status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu", (unsigned long long)channel_id
    );
    dcc_endpoint_body_t body = {
        json, json != NULL ? strlen(json) : 0U, "application/json"
    };
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_modify_channel", DCC_REST_PATCH, path, &body,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(json);
    return status;
}

dcc_status_t dcc_rest_delete_channel(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_DELETE, DCC_REST_DELETE);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = channel_prepare(
        client, options, DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved
    );
    if (status != DCC_OK || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[64];
    status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu",
        (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_delete_channel", DCC_REST_DELETE, path, NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}
