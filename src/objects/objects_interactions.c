#include "internal/objects/dcc_interaction_clone_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_interaction_t *dcc_interaction_clone(const dcc_interaction_t *interaction) {
    if (interaction == NULL) {
        return NULL;
    }

    dcc_interaction_t *copy = (dcc_interaction_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *interaction;
    copy->token = NULL;
    copy->name = NULL;
    copy->custom_id = NULL;
    copy->locale = NULL;
    copy->guild_locale = NULL;
    copy->values = NULL;
    copy->form_fields = NULL;
    copy->options = NULL;
    copy->authorizing_integration_owners = NULL;
    copy->entitlements = NULL;
    copy->focused_option = NULL;
    memset(&copy->resolved, 0, sizeof(copy->resolved));

    if (dcc_clone_interaction_string_fields(copy, interaction) != 0 ||
        dcc_clone_interaction_values(copy, interaction) != 0 ||
        dcc_clone_interaction_form_fields(copy, interaction) != 0 ||
        dcc_clone_interaction_options(copy, interaction) != 0 ||
        dcc_clone_interaction_collections(copy, interaction) != 0) {
        dcc_interaction_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_interaction_free(dcc_interaction_t *interaction) {
    if (interaction == NULL) {
        return;
    }
    free((void *)interaction->token);
    free((void *)interaction->name);
    free((void *)interaction->custom_id);
    free((void *)interaction->locale);
    free((void *)interaction->guild_locale);
    if (interaction->values != NULL) {
        for (size_t i = 0; i < interaction->values_count; i++) {
            free((void *)interaction->values[i]);
        }
        free((void *)interaction->values);
    }
    if (interaction->form_fields != NULL) {
        dcc_interaction_form_field_t *fields =
            (dcc_interaction_form_field_t *)interaction->form_fields;
        for (size_t i = 0; i < interaction->form_fields_count; i++) {
            free((void *)fields[i].custom_id);
            free((void *)fields[i].value);
            if (fields[i].values != NULL) {
                for (size_t j = 0; j < fields[i].values_count; j++) {
                    free((void *)fields[i].values[j]);
                }
                free((void *)fields[i].values);
            }
        }
        free(fields);
    }
    free((void *)interaction->authorizing_integration_owners);
    if (interaction->entitlements != NULL) {
        dcc_entitlement_t *entitlements = (dcc_entitlement_t *)interaction->entitlements;
        for (size_t i = 0; i < interaction->entitlements_count; i++) {
            dcc_free_entitlement_fields(&entitlements[i]);
        }
        free(entitlements);
    }
    dcc_free_interaction_option_array(
        (dcc_interaction_option_t *)interaction->options,
        interaction->options_count
    );
    dcc_free_interaction_resolved_fields(&interaction->resolved);
    free(interaction);
}
