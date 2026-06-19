#include "internal/rest/dcc_rest_query_collections_internal.h"

dcc_status_t dcc_rest_query_append_search_has_repeated(
    dcc_rest_buffer_t *query,
    const dcc_message_search_has_t *values,
    size_t count
) {
    static const char *names[] = {
        "image",
        "sound",
        "video",
        "file",
        "sticker",
        "embed",
        "link",
        "poll",
        "snapshot"
    };

    if (query == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (count == 0) {
        return DCC_OK;
    }
    if (values == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        if ((unsigned)values[i] >= (unsigned)(sizeof(names) / sizeof(names[0]))) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_rest_query_append_string(query, "has", names[values[i]]);
    }
    return status;
}
