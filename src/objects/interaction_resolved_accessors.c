#include "internal/events/dcc_event_state_internal.h"

const dcc_user_t *dcc_interaction_resolved_user(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->resolved.users == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.users_count; i++) {
        if (interaction->resolved.users[i].id == id) {
            return &interaction->resolved.users[i];
        }
    }
    return NULL;
}

const dcc_member_t *dcc_interaction_resolved_member(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t user_id
) {
    if (interaction == NULL || user_id == 0 || interaction->resolved.members == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.members_count; i++) {
        if (interaction->resolved.members[i].user.id == user_id) {
            return &interaction->resolved.members[i];
        }
    }
    return NULL;
}
