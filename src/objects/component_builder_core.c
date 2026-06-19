#include "internal/objects/dcc_component_builder_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_component_builder_init(dcc_component_builder_t *builder, dcc_component_type_t type) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->type = type;
    }
}

void dcc_component_builder_json_free(char *json) {
    free(json);
}
