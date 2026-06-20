#include <dcc/objects/clone/interactions.h>

#include <string.h>

const dcc_interaction_form_field_t *dcc_interaction_form_field_by_custom_id(
    const dcc_interaction_t *interaction,
    const char *custom_id
) {
    if (interaction == NULL || custom_id == NULL || interaction->form_fields == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < interaction->form_fields_count; ++i) {
        if (interaction->form_fields[i].custom_id != NULL &&
            strcmp(interaction->form_fields[i].custom_id, custom_id) == 0) {
            return &interaction->form_fields[i];
        }
    }
    return NULL;
}

const char *dcc_interaction_form_value(
    const dcc_interaction_t *interaction,
    const char *custom_id
) {
    const dcc_interaction_form_field_t *field =
        dcc_interaction_form_field_by_custom_id(interaction, custom_id);
    if (field == NULL || field->value_type != DCC_INTERACTION_FORM_VALUE_STRING) {
        return NULL;
    }
    return field->value;
}

int dcc_interaction_form_boolean(
    const dcc_interaction_t *interaction,
    const char *custom_id,
    uint8_t *out
) {
    const dcc_interaction_form_field_t *field =
        dcc_interaction_form_field_by_custom_id(interaction, custom_id);
    if (field == NULL || out == NULL || field->value_type != DCC_INTERACTION_FORM_VALUE_BOOLEAN) {
        return 0;
    }
    *out = field->boolean_value;
    return 1;
}
