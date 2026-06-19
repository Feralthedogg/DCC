#include "internal/rest/dcc_rest_message_reactions_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_delete_all_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu/reactions",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_all_message_reactions_for_emoji(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_message_reaction_path(&path, channel_id, message_id, reaction, NULL);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
