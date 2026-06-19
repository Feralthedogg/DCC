#ifndef DCC_REST_RESOURCES_DIRECT_MESSAGES_H
#define DCC_REST_RESOURCES_DIRECT_MESSAGES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_direct_message(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_direct_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
