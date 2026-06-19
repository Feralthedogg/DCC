#include "internal/objects/dcc_interaction_clone_internal.h"

#include <stdlib.h>
#include <string.h>

static int dcc_clone_interaction_owners(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    if (interaction->authorizing_integration_owners_count == 0) {
        return 0;
    }
    if (interaction->authorizing_integration_owners == NULL ||
        interaction->authorizing_integration_owners_count >
            (size_t)-1 / sizeof(*interaction->authorizing_integration_owners)) {
        return -1;
    }
    dcc_interaction_authorizing_integration_owner_t *owners =
        (dcc_interaction_authorizing_integration_owner_t *)malloc(
            interaction->authorizing_integration_owners_count * sizeof(*owners)
        );
    if (owners == NULL) {
        return -1;
    }
    memcpy(
        owners,
        interaction->authorizing_integration_owners,
        interaction->authorizing_integration_owners_count * sizeof(*owners)
    );
    copy->authorizing_integration_owners = owners;
    return 0;
}

static int dcc_clone_interaction_entitlements(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    if (interaction->entitlements_count == 0) {
        return 0;
    }
    if (interaction->entitlements == NULL ||
        interaction->entitlements_count > (size_t)-1 / sizeof(*interaction->entitlements)) {
        return -1;
    }
    dcc_entitlement_t *entitlements =
        (dcc_entitlement_t *)calloc(interaction->entitlements_count, sizeof(*entitlements));
    if (entitlements == NULL) {
        return -1;
    }
    copy->entitlements = entitlements;
    for (size_t i = 0; i < interaction->entitlements_count; i++) {
        if (dcc_clone_entitlement_fields(&entitlements[i], &interaction->entitlements[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

int dcc_clone_interaction_collections(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    return dcc_clone_interaction_owners(copy, interaction) != 0 ||
            dcc_clone_interaction_entitlements(copy, interaction) != 0 ||
            dcc_clone_interaction_resolved_fields(&copy->resolved, &interaction->resolved) != 0
        ? -1
        : 0;
}
