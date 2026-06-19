#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_query_collections_internal.h"

dcc_status_t dcc_rest_message_search_append_filter_query(
    const dcc_message_search_params_t *params,
    dcc_rest_buffer_t *query
) {
    dcc_status_t status = dcc_rest_query_append_string(query, "content", params->content);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(query, "author_id", params->author_ids, params->author_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(query, "mentions", params->mentions, params->mention_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(query, "channel_id", params->channel_ids, params->channel_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(query, "mentions_role_id", params->mention_role_ids, params->mention_role_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(
            query,
            "replied_to_user_id",
            params->replied_to_user_ids,
            params->replied_to_user_id_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64_repeated(
            query,
            "replied_to_message_id",
            params->replied_to_message_ids,
            params->replied_to_message_id_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(query, "author_type", params->author_types, params->author_type_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_search_has_repeated(query, params->has, params->has_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(query, "embed_type", params->embed_types, params->embed_type_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(
            query,
            "embed_provider",
            params->embed_providers,
            params->embed_provider_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(
            query,
            "link_hostname",
            params->link_hostnames,
            params->link_hostname_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(
            query,
            "attachment_filename",
            params->attachment_filenames,
            params->attachment_filename_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string_repeated(
            query,
            "attachment_extension",
            params->attachment_extensions,
            params->attachment_extension_count
        );
    }
    return status;
}
