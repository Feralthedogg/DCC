#ifndef DCC_REST_RESOURCES_ENTITLEMENTS_H
#define DCC_REST_RESOURCES_ENTITLEMENTS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_skus(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_entitlements(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_rest_entitlement_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_create_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_test_entitlement_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_consume_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
