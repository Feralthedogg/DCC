#ifndef DCC_REST_INTERACTIONS_FOLLOWUPS_TOKEN_H
#define DCC_REST_INTERACTIONS_FOLLOWUPS_TOKEN_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_interaction_followup_create(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_create_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_create_multipart(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit_multipart(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
