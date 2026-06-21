#include <dcc/interaction_helpers.h>
#include <dcc/rest/interactions/responses.h>

dcc_status_t dcc_interaction_reply_message(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        message,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_reply_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_interaction_reply_message(client, interaction, &message, cb, user_data);
}

dcc_status_t dcc_interaction_reply_ephemeral_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_message_builder_t message = {
        .content = content,
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL,
        .has_content = 1U,
        .has_flags = 1U,
    };
    return dcc_interaction_reply_message(client, interaction, &message, cb, user_data);
}

dcc_status_t dcc_interaction_reply_embed(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t ephemeral,
    dcc_rest_cb cb,
    void *user_data
) {
    if (title == NULL && description == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_embed_builder_t embed = {
        .title = title,
        .description = description,
        .color = color,
        .has_color = 1U,
    };
    dcc_message_builder_t message = {
        .embeds = &embed,
        .embeds_count = 1U,
    };
    if (ephemeral) {
        message.flags = DCC_MESSAGE_FLAG_EPHEMERAL;
        message.has_flags = 1U;
    }
    return dcc_interaction_reply_message(client, interaction, &message, cb, user_data);
}

dcc_status_t dcc_interaction_reply_error(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_interaction_reply_embed(
        client,
        interaction,
        title != NULL ? title : "Error",
        description,
        0xED4245U,
        1U,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_reply_success(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_interaction_reply_embed(
        client,
        interaction,
        title != NULL ? title : "Done",
        description,
        0x57F287U,
        1U,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_defer(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_deferred_message_from_interaction(
        client,
        interaction,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_defer_ephemeral(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_message_builder_t message = {
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL,
        .has_flags = 1U,
    };
    return dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,
        &message,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_update_message(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE,
        message,
        cb,
        user_data
    );
}

dcc_status_t dcc_interaction_show_modal(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_modal_from_interaction(
        client,
        interaction,
        modal,
        cb,
        user_data
    );
}
