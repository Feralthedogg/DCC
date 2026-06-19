#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_message_reactions_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_message_reaction_path(&path, channel_id, message_id, reaction, NULL);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data);
    free(path);
    return status;
}

dcc_status_t dcc_rest_get_message_reactions_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "after", after);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "before", before);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_message_reactions(
            client,
            channel_id,
            message_id,
            reaction,
            query.data,
            cb,
            user_data
        );
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
