#include "internal/objects/dcc_autocomplete_builder_internal.h"

#include <stdlib.h>

dcc_status_t dcc_autocomplete_builder_build_json(
    const dcc_autocomplete_builder_t *builder,
    char **out_json
) {
    if (builder == NULL ||
        out_json == NULL ||
        (builder->choices_count != 0 && builder->choices == NULL) ||
        builder->choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_autocomplete_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_autocomplete_json_append_cstr(
        &buffer,
        "{\"type\":8,\"data\":{\"choices\":["
    );
    for (size_t i = 0; status == DCC_OK && i < builder->choices_count; ++i) {
        if (i != 0) {
            status = dcc_autocomplete_json_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_autocomplete_append_choice(&builder->choices[i], &buffer);
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
