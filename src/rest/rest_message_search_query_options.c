#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

dcc_status_t dcc_rest_message_search_append_option_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
) {
    dcc_status_t status = dcc_rest_query_append_u64(query, "max_id", params->max_id);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(query, "min_id", params->min_id);
    }
    if (status == DCC_OK && params->has_pinned != 0) {
        status = dcc_rest_query_append_bool(query, "pinned", params->pinned);
    }
    if (status == DCC_OK && params->has_mention_everyone != 0) {
        status = dcc_rest_query_append_bool(query, "mention_everyone", params->mention_everyone);
    }
    if (status == DCC_OK && params->has_include_nsfw != 0) {
        status = dcc_rest_query_append_bool(query, "include_nsfw", params->include_nsfw);
    }
    if (status == DCC_OK && params->has_limit != 0) {
        status = dcc_rest_query_append_u64_value(query, "limit", params->limit);
    }
    if (status == DCC_OK && params->has_offset != 0) {
        status = dcc_rest_query_append_u64_value(query, "offset", params->offset);
    }
    if (status == DCC_OK && params->has_slop != 0) {
        status = dcc_rest_query_append_u64_value(query, "slop", params->slop);
    }
    if (status == DCC_OK && params->has_sort_by != 0) {
        if (params->sort_by > DCC_MESSAGE_SEARCH_SORT_RELEVANCE) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_rest_query_append_string(
            query,
            "sort_by",
            params->sort_by == DCC_MESSAGE_SEARCH_SORT_RELEVANCE ? "relevance" : "timestamp"
        );
    }
    if (status == DCC_OK && params->has_sort_order != 0) {
        if (params->sort_order > DCC_MESSAGE_SEARCH_ORDER_ASC) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_rest_query_append_string(
            query,
            "sort_order",
            params->sort_order == DCC_MESSAGE_SEARCH_ORDER_ASC ? "asc" : "desc"
        );
    }
    return status;
}
