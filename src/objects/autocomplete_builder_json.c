#include "internal/objects/dcc_autocomplete_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdlib.h>

dcc_status_t dcc_autocomplete_builder_build_json(
    const dcc_autocomplete_builder_t *builder,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    dcc_builder_abi_view_t view;
    if (dcc_autocomplete_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_choices = dcc_builder_abi_view_has(
        &view, DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES
    );
    size_t choices_count = has_choices ? builder->choices_count : 0U;
    const dcc_autocomplete_choice_t *choices = has_choices ? builder->choices : NULL;
    if ((choices_count != 0U && choices == NULL) ||
        choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t stride = 0U;
    dcc_status_t status = dcc_autocomplete_choice_array_begin(
        choices, choices_count, &stride
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_autocomplete_json_buffer_t buffer = {0};
    status = dcc_autocomplete_json_append_cstr(
        &buffer,
        "{\"type\":8,\"data\":{\"choices\":["
    );
    for (size_t i = 0; status == DCC_OK && i < choices_count; ++i) {
        if (i != 0) {
            status = dcc_autocomplete_json_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            const dcc_autocomplete_choice_t *choice = (const dcc_autocomplete_choice_t *)
                dcc_builder_abi_array_at(choices, stride, i);
            dcc_builder_abi_view_t choice_view;
            status = dcc_autocomplete_choice_abi_validate(choice, &choice_view);
            if (status == DCC_OK && choice_view.size != stride) {
                status = DCC_ERR_INVALID_ARG;
            }
            if (status == DCC_OK) {
                status = dcc_autocomplete_append_choice(choice, &buffer);
            }
        }
    }
    if (status == DCC_OK) {
        status = dcc_autocomplete_json_append_cstr(&buffer, "]}}");
    }
    if (status != DCC_OK) {
        dcc_autocomplete_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

void dcc_autocomplete_builder_json_free(char *json) {
    free(json);
}
