#ifndef DCC_REST_INTERACTIONS_FOLLOWUPS_INTERACTION_H
#define DCC_REST_INTERACTIONS_FOLLOWUPS_INTERACTION_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_interaction_followup_create_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_create_from_interaction_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_create_multipart_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_get_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit_from_interaction_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_edit_multipart_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_interaction_followup_delete_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
