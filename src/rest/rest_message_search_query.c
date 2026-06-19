#include "internal/rest/dcc_rest_message_search_internal.h"

dcc_status_t dcc_rest_message_search_build_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
) {
    if (params == NULL || query == NULL || params->size < sizeof(*params)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_message_search_append_filter_query(params, query);
    if (status == DCC_OK) {
        status = dcc_rest_message_search_append_option_query(params, query);
    }
    return status;
}
