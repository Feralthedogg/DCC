#ifndef DCC_REST_INTERACTIONS_ORIGINAL_RESPONSE_H
#define DCC_REST_INTERACTIONS_ORIGINAL_RESPONSE_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_interaction_original_response_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit_multipart(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_get_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit_from_interaction_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_edit_multipart_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_original_response_delete_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
