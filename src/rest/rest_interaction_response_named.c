#include <dcc/rest/interactions/responses.h>

dcc_status_t dcc_rest_interaction_response_create_pong(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type(
        client,
        interaction_id,
        interaction_token,
        DCC_INTERACTION_RESPONSE_PONG,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_deferred_message(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type(
        client,
        interaction_id,
        interaction_token,
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_deferred_update(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type(
        client,
        interaction_id,
        interaction_token,
        DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_premium_required(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type(
        client,
        interaction_id,
        interaction_token,
        DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED,
        cb,
        user_data
    );
}
