#ifndef DCC_REST_INTERACTIONS_RESPONSES_H
#define DCC_REST_INTERACTIONS_RESPONSES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_interaction_response_create(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_options(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const char *json_body,
    uint8_t with_response,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_type(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_interaction_response_type_t type,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_pong(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_deferred_message(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_deferred_update(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_premium_required(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_modal(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_autocomplete(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_from_interaction_message_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_type_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_pong_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_deferred_message_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_deferred_update_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_premium_required_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_modal_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_response_create_autocomplete_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
