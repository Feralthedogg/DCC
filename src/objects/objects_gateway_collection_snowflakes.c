#include "internal/objects/dcc_objects_internal.h"

int dcc_gateway_clone_snowflake_collection(
    const dcc_snowflake_t *src,
    size_t count,
    const dcc_snowflake_t **out
) {
    if (out == NULL) {
        return -1;
    }
    *out = NULL;
    if (src == NULL || count == 0) {
        return 0;
    }

    dcc_snowflake_t *copy = dcc_clone_snowflake_array_or_null(src, count);
    if (copy == NULL) {
        return -1;
    }
    *out = copy;
    return 0;
}
