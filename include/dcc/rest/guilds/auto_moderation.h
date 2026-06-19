#ifndef DCC_REST_GUILDS_AUTO_MODERATION_H
#define DCC_REST_GUILDS_AUTO_MODERATION_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_auto_moderation_rules(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_auto_moderation_rule_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_auto_moderation_rule_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
