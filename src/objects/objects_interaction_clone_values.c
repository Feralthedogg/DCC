#include "internal/objects/dcc_interaction_clone_internal.h"

#include <stdlib.h>

int dcc_clone_interaction_values(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    if (interaction->values_count == 0) {
        return 0;
    }
    if (interaction->values == NULL || interaction->values_count > (size_t)-1 / sizeof(*copy->values)) {
        return -1;
    }
    copy->values = (const char **)calloc(interaction->values_count, sizeof(*copy->values));
    if (copy->values == NULL) {
        return -1;
    }
    for (size_t i = 0; i < interaction->values_count; i++) {
        copy->values[i] = dcc_clone_string_or_null(interaction->values[i]);
        if (interaction->values[i] != NULL && copy->values[i] == NULL) {
            return -1;
        }
    }
    return 0;
}
