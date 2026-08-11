#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t invite_body(
    const dcc_invite_params_t *params, dcc_endpoint_body_t *out
) {
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_endpoint_body_t){0};
    const uint64_t known = (UINT64_C(1) << 9U) - 1U;
    dcc_endpoint_record_view_t view;
    if (dcc_endpoint_record_read(
            params, offsetof(dcc_invite_params_t, version),
            offsetof(dcc_invite_params_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_invite_params_t, present),
            DCC_INVITE_PARAMS_VERSION, known, &view
        ) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
#define INVITE_FIELD(bit_, field_) \
    dcc_endpoint_present_field_covered( \
        &view, (bit_), offsetof(dcc_invite_params_t, field_), \
        sizeof(params->field_) \
    )
#define INVITE_PARTIAL(field_) \
    dcc_endpoint_field_partially_covered( \
        view.size, offsetof(dcc_invite_params_t, field_), \
        sizeof(params->field_) \
    )
    if (INVITE_PARTIAL(max_age) || INVITE_PARTIAL(max_uses) ||
        INVITE_PARTIAL(temporary) || INVITE_PARTIAL(unique) ||
        INVITE_PARTIAL(target_type) || INVITE_PARTIAL(target_user_id) ||
        INVITE_PARTIAL(target_application_id) ||
        INVITE_PARTIAL(target_users_filename) ||
        INVITE_PARTIAL(target_users_data) || INVITE_PARTIAL(target_users_len) ||
        INVITE_PARTIAL(role_ids) || INVITE_PARTIAL(role_id_count) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_MAX_AGE, max_age) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_MAX_USES, max_uses) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_TEMPORARY, temporary) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_UNIQUE, unique) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_TARGET_TYPE, target_type) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_TARGET_USER_ID, target_user_id) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_TARGET_APPLICATION_ID, target_application_id) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_TARGET_USERS_FILE, target_users_filename) ||
        !INVITE_FIELD(DCC_INVITE_PARAMS_PRESENT_ROLE_IDS, role_ids) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_TARGET_USERS_FILE) != 0U &&
         (!dcc_endpoint_field_covered(view.size,
             offsetof(dcc_invite_params_t, target_users_data),
             sizeof(params->target_users_data)) ||
          !dcc_endpoint_field_covered(view.size,
             offsetof(dcc_invite_params_t, target_users_len),
             sizeof(params->target_users_len)))) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_ROLE_IDS) != 0U &&
         !dcc_endpoint_field_covered(view.size,
             offsetof(dcc_invite_params_t, role_id_count),
             sizeof(params->role_id_count)))) {
        return DCC_ERR_INVALID_ARG;
    }
#undef INVITE_PARTIAL
#undef INVITE_FIELD
    if (((view.present & DCC_INVITE_PARAMS_PRESENT_MAX_AGE) != 0U && params->max_age > 604800U) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_MAX_USES) != 0U && params->max_uses > 100U) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_TEMPORARY) != 0U && params->temporary > 1U) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_UNIQUE) != 0U && params->unique > 1U) ||
        ((view.present & DCC_INVITE_PARAMS_PRESENT_ROLE_IDS) != 0U &&
            ((params->role_id_count != 0U && params->role_ids == NULL) ||
             params->role_id_count > SIZE_MAX / sizeof(*params->role_ids))))
        return DCC_ERR_INVALID_ARG;
    int has_type = (view.present & DCC_INVITE_PARAMS_PRESENT_TARGET_TYPE) != 0U;
    int has_user = (view.present & DCC_INVITE_PARAMS_PRESENT_TARGET_USER_ID) != 0U;
    int has_app = (view.present & DCC_INVITE_PARAMS_PRESENT_TARGET_APPLICATION_ID) != 0U;
    if ((!has_type && (has_user || has_app)) ||
        (has_type && params->target_type == DCC_INVITE_TARGET_STREAM &&
            (!has_user || has_app || params->target_user_id == 0U)) ||
        (has_type && params->target_type == DCC_INVITE_TARGET_EMBEDDED_APPLICATION &&
            (!has_app || has_user || params->target_application_id == 0U)) ||
        (has_type && params->target_type != DCC_INVITE_TARGET_STREAM &&
            params->target_type != DCC_INVITE_TARGET_EMBEDDED_APPLICATION))
        return DCC_ERR_INVALID_ARG;
    if ((view.present & DCC_INVITE_PARAMS_PRESENT_ROLE_IDS) != 0U) {
        for (size_t i = 0U; i < params->role_id_count; ++i) {
            if (params->role_ids[i] == 0U) return DCC_ERR_INVALID_ARG;
            for (size_t j = 0U; j < i; ++j)
                if (params->role_ids[j] == params->role_ids[i])
                    return DCC_ERR_INVALID_ARG;
        }
    }
    int with_file = (view.present & DCC_INVITE_PARAMS_PRESENT_TARGET_USERS_FILE) != 0U;
    if (with_file &&
        (params->target_users_filename == NULL || params->target_users_filename[0] == '\0' ||
         strchr(params->target_users_filename, '\r') != NULL ||
         strchr(params->target_users_filename, '\n') != NULL ||
         params->target_users_data == NULL || params->target_users_len == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t json = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&json, "{");
    int first = 1;
#define APPEND(bit_, expr_) do { if (status == DCC_OK && (view.present & (bit_)) != 0U) status = (expr_); } while (0)
    APPEND(DCC_INVITE_PARAMS_PRESENT_MAX_AGE,
        dcc_rest_json_append_u64_member(&json, &first, "max_age", params->max_age));
    APPEND(DCC_INVITE_PARAMS_PRESENT_MAX_USES,
        dcc_rest_json_append_u64_member(&json, &first, "max_uses", params->max_uses));
    APPEND(DCC_INVITE_PARAMS_PRESENT_TEMPORARY,
        dcc_rest_json_append_bool_member(&json, &first, "temporary", params->temporary));
    APPEND(DCC_INVITE_PARAMS_PRESENT_UNIQUE,
        dcc_rest_json_append_bool_member(&json, &first, "unique", params->unique));
    APPEND(DCC_INVITE_PARAMS_PRESENT_TARGET_TYPE,
        dcc_rest_json_append_u64_member(&json, &first, "target_type", params->target_type));
    APPEND(DCC_INVITE_PARAMS_PRESENT_TARGET_USER_ID,
        dcc_rest_json_append_u64_string_member(&json, &first, "target_user_id", params->target_user_id));
    APPEND(DCC_INVITE_PARAMS_PRESENT_TARGET_APPLICATION_ID,
        dcc_rest_json_append_u64_string_member(&json, &first, "target_application_id", params->target_application_id));
    APPEND(DCC_INVITE_PARAMS_PRESENT_ROLE_IDS,
        dcc_rest_json_append_snowflake_string_array_member(&json, &first, "role_ids", params->role_ids, params->role_id_count));
#undef APPEND
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&json);
        return status;
    }
    if (!with_file) {
        *out = (dcc_endpoint_body_t){json.data, json.len, "application/json"};
        return DCC_OK;
    }
    dcc_rest_multipart_field_t field = {"payload_json", json.data};
    dcc_rest_multipart_file_t file = {
        "target_users_file", params->target_users_filename, "text/csv",
        params->target_users_data, params->target_users_len
    };
    char *multipart = NULL;
    size_t multipart_len = 0U;
    status = dcc_rest_build_multipart_body(
        &field, 1U, &file, 1U, &multipart, &multipart_len
    );
    dcc_rest_buffer_deinit(&json);
    if (status == DCC_OK)
        *out = (dcc_endpoint_body_t){multipart, multipart_len, DCC_REST_MULTIPART_CONTENT_TYPE};
    return status;
}

dcc_status_t dcc_rest_get_channel_invites(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CHANNEL_INVITES_GET, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    if (status != DCC_OK || client == NULL || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[80];
    status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/invites",
        (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_get_channel_invites", DCC_REST_GET, path, NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}

dcc_status_t dcc_rest_create_channel_invite(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_invite_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_INVITE_CREATE, DCC_REST_POST);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_ALLOWED
    );
    if (status != DCC_OK || client == NULL || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_body_t body = {0};
    status = invite_body(params, &body);
    char path[80];
    if (status == DCC_OK) status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/invites", (unsigned long long)channel_id
    );
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_create_channel_invite", DCC_REST_POST, path, &body,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    dcc_endpoint_body_deinit(&body);
    return status;
}
