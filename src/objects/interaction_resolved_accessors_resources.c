#include "internal/events/dcc_event_state_internal.h"

const dcc_role_t *dcc_interaction_resolved_role(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->resolved.roles == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.roles_count; i++) {
        if (interaction->resolved.roles[i].id == id) {
            return &interaction->resolved.roles[i];
        }
    }
    return NULL;
}

const dcc_channel_t *dcc_interaction_resolved_channel(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->resolved.channels == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.channels_count; i++) {
        if (interaction->resolved.channels[i].id == id) {
            return &interaction->resolved.channels[i];
        }
    }
    return NULL;
}

const dcc_message_t *dcc_interaction_resolved_message(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->resolved.messages == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.messages_count; i++) {
        if (interaction->resolved.messages[i].id == id) {
            return &interaction->resolved.messages[i];
        }
    }
    return NULL;
}

const dcc_attachment_t *dcc_interaction_resolved_attachment(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->resolved.attachments == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.attachments_count; i++) {
        if (interaction->resolved.attachments[i].id == id) {
            return &interaction->resolved.attachments[i];
        }
    }
    return NULL;
}
