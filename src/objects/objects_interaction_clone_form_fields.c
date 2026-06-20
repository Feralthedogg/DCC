#include "internal/objects/dcc_interaction_clone_internal.h"

#include <stdlib.h>

static int dcc_clone_interaction_form_field_values(
    dcc_interaction_form_field_t *copy,
    const dcc_interaction_form_field_t *field
) {
    if (field->values_count == 0) {
        return 0;
    }
    if (field->values == NULL || field->values_count > (size_t)-1 / sizeof(*copy->values)) {
        return -1;
    }

    copy->values = (const char **)calloc(field->values_count, sizeof(*copy->values));
    if (copy->values == NULL) {
        return -1;
    }
    for (size_t i = 0; i < field->values_count; ++i) {
        copy->values[i] = dcc_clone_string_or_null(field->values[i]);
        if (field->values[i] != NULL && copy->values[i] == NULL) {
            return -1;
        }
    }
    return 0;
}

int dcc_clone_interaction_form_fields(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    if (interaction->form_fields_count == 0) {
        return 0;
    }
    if (interaction->form_fields == NULL ||
        interaction->form_fields_count > (size_t)-1 / sizeof(*copy->form_fields)) {
        return -1;
    }

    dcc_interaction_form_field_t *fields =
        (dcc_interaction_form_field_t *)calloc(
            interaction->form_fields_count,
            sizeof(*fields)
        );
    if (fields == NULL) {
        return -1;
    }

    copy->form_fields = fields;
    for (size_t i = 0; i < interaction->form_fields_count; ++i) {
        fields[i] = interaction->form_fields[i];
        fields[i].custom_id = NULL;
        fields[i].value = NULL;
        fields[i].values = NULL;

        fields[i].custom_id = dcc_clone_string_or_null(interaction->form_fields[i].custom_id);
        if (interaction->form_fields[i].custom_id != NULL && fields[i].custom_id == NULL) {
            return -1;
        }
        fields[i].value = dcc_clone_string_or_null(interaction->form_fields[i].value);
        if (interaction->form_fields[i].value != NULL && fields[i].value == NULL) {
            return -1;
        }
        if (dcc_clone_interaction_form_field_values(&fields[i], &interaction->form_fields[i]) != 0) {
            return -1;
        }
    }
    return 0;
}
