#ifndef DCC_REST_APPLICATION_COMMANDS_APPLICATION_EMOJIS_H
#define DCC_REST_APPLICATION_COMMANDS_APPLICATION_EMOJIS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_application_emojis(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_application_emoji_params(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_application_emoji_params(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
