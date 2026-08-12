#ifndef DCC_REST_APPLICATION_COMMANDS_APPLICATION_EMOJIS_H
#define DCC_REST_APPLICATION_COMMANDS_APPLICATION_EMOJIS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_emoji_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_application_emojis(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_modify_application_emoji(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t emoji_id,
    const dcc_emoji_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
