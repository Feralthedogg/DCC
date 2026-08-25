#ifndef DCC_REST_TASK10_INTERNAL_H
#define DCC_REST_TASK10_INTERNAL_H

#include "internal/rest/dcc_rest_endpoint_internal.h"

dcc_status_t dcc_task10_prepare(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_endpoint_auth_policy_t auth, uint64_t forced_flags,
    dcc_rest_request_t **out_request, dcc_rest_call_options_t *resolved);
dcc_status_t dcc_task10_submit(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *path, char *json, const dcc_rest_call_options_t *resolved,
    dcc_rest_request_t **out_request);
dcc_status_t dcc_task10_submit_empty(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *path, const dcc_rest_call_options_t *resolved,
    dcc_rest_request_t **out_request);

dcc_status_t dcc_task10_build_current_user(
    const dcc_current_user_params_t *params, char **out_json);
dcc_status_t dcc_task10_build_current_voice(
    const dcc_current_user_voice_state_params_t *params, char **out_json);
dcc_status_t dcc_task10_build_user_voice(
    const dcc_user_voice_state_params_t *params, char **out_json);
dcc_status_t dcc_task10_build_group_dm(
    const dcc_group_dm_recipient_params_t *params, char **out_json);
dcc_status_t dcc_task10_build_test_entitlement(
    const dcc_test_entitlement_params_t *params, char **out_json);
dcc_status_t dcc_task10_entitlement_query(
    const dcc_rest_entitlement_query_t *query, char **out_query);
dcc_status_t dcc_task10_subscription_query(
    const dcc_rest_sku_subscription_query_t *query,
    dcc_rest_auth_mode_t auth_mode, char **out_query);
dcc_status_t dcc_task10_build_role_metadata(
    const dcc_application_role_connection_metadata_params_t *metadata,
    size_t metadata_count, char **out_json);
dcc_status_t dcc_task10_build_role_connection(
    const dcc_application_role_connection_params_t *params, char **out_json);

#endif
