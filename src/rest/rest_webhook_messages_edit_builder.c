#include <dcc/rest.h>

dcc_status_t dcc_rest_modify_webhook_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_rest_modify_webhook_message(
            client,
            webhook_id,
            webhook_token,
            message_id,
            query,
            json,
            cb,
            user_data
        );
    }
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_rest_modify_webhook_message_builder_thread(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_rest_modify_webhook_message_thread(
            client,
            webhook_id,
            webhook_token,
            message_id,
            thread_id,
            json,
            cb,
            user_data
        );
    }
    dcc_message_builder_json_free(json);
    return status;
}
