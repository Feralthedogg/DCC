#include "internal/rest/dcc_rest_entitlement_body_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/applications/%llu/entitlements", (unsigned long long)application_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_test_entitlement_params(
    dcc_client_t *client,
    const dcc_test_entitlement_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_test_entitlement_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_test_entitlement(client, params->application_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_delete_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/entitlements/%llu",
        (unsigned long long)application_id,
        (unsigned long long)entitlement_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_consume_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[136];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/entitlements/%llu/consume",
        (unsigned long long)application_id,
        (unsigned long long)entitlement_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, NULL, cb, user_data) : status;
}
