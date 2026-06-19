#ifndef DCC_REST_RESOURCES_ENTITLEMENTS_H
#define DCC_REST_RESOURCES_ENTITLEMENTS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_skus(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_entitlements(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_entitlements_page(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids,
    size_t sku_id_count,
    dcc_snowflake_t before_id,
    dcc_snowflake_t after_id,
    uint8_t limit,
    dcc_snowflake_t guild_id,
    uint8_t exclude_ended,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_test_entitlement_params(
    dcc_client_t *client,
    const dcc_test_entitlement_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_test_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_consume_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
