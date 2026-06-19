#include "internal/events/dcc_event_state_internal.h"

const dcc_interaction_resolved_permission_t *dcc_interaction_resolved_permission(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t user_id
) {
    if (interaction == NULL || user_id == 0 || interaction->resolved.member_permissions == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->resolved.member_permissions_count; i++) {
        if (interaction->resolved.member_permissions[i].user_id == user_id) {
            return &interaction->resolved.member_permissions[i];
        }
    }
    return NULL;
}

dcc_snowflake_t dcc_interaction_authorizing_integration_owner(
    const dcc_interaction_t *interaction,
    uint32_t type
) {
    if (interaction == NULL || interaction->authorizing_integration_owners == NULL) {
        return 0;
    }

    for (size_t i = 0; i < interaction->authorizing_integration_owners_count; i++) {
        if (interaction->authorizing_integration_owners[i].type == type) {
            return interaction->authorizing_integration_owners[i].owner_id;
        }
    }
    return 0;
}

const dcc_entitlement_t *dcc_interaction_entitlement(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
) {
    if (interaction == NULL || id == 0 || interaction->entitlements == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->entitlements_count; i++) {
        if (interaction->entitlements[i].id == id) {
            return &interaction->entitlements[i];
        }
    }
    return NULL;
}
