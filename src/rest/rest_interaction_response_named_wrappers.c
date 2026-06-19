#include <dcc/rest/interactions/responses.h>

dcc_status_t dcc_rest_interaction_response_create_pong_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type_from_interaction(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_PONG,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_deferred_message_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type_from_interaction(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_deferred_update_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type_from_interaction(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_premium_required_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_type_from_interaction(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED,
        cb,
        user_data
    );
}
