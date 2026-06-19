#ifndef DCC_REST_MESSAGE_SEARCH_INTERNAL_H
#define DCC_REST_MESSAGE_SEARCH_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_message_search_build_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
);
dcc_status_t dcc_rest_message_search_append_filter_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
);
dcc_status_t dcc_rest_message_search_append_option_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
);

#ifdef __cplusplus
}
#endif

#endif
