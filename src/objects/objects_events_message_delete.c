#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_message_delete_bulk_t *dcc_message_delete_bulk_clone(const dcc_message_delete_bulk_t *bulk_delete) {
    if (bulk_delete == NULL) {
        return NULL;
    }

    dcc_message_delete_bulk_t *copy = (dcc_message_delete_bulk_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *bulk_delete;
    copy->ids = NULL;
    if (bulk_delete->ids != NULL && bulk_delete->ids_count > 0) {
        copy->ids = dcc_clone_snowflake_array_or_null(bulk_delete->ids, bulk_delete->ids_count);
        if (copy->ids == NULL) {
            dcc_message_delete_bulk_free(copy);
            return NULL;
        }
    }

    return copy;
}

void dcc_message_delete_bulk_free(dcc_message_delete_bulk_t *bulk_delete) {
    if (bulk_delete == NULL) {
        return;
    }
    free((void *)bulk_delete->ids);
    free(bulk_delete);
}
