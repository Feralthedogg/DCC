#ifndef DCC_REST_RESOURCES_TEMPLATES_H
#define DCC_REST_RESOURCES_TEMPLATES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_template(
    dcc_client_t *client,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_from_template(
    dcc_client_t *client,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_from_template_params(
    dcc_client_t *client,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_templates(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_template_params(
    dcc_client_t *client,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_template_params(
    dcc_client_t *client,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_sync_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
