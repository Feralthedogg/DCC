#ifndef DCC_MANAGED_MESSAGE_H
#define DCC_MANAGED_MESSAGE_H

#include <dcc/message.h>
#include <dcc/rest/base.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_managed_message_ref {
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
} dcc_managed_message_ref_t;

typedef dcc_status_t (*dcc_managed_message_load_fn)(
    dcc_managed_message_ref_t *out_ref,
    void *user_data
);
typedef dcc_status_t (*dcc_managed_message_save_fn)(
    const dcc_managed_message_ref_t *ref,
    void *user_data
);
typedef void (*dcc_managed_message_publish_cb)(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    const dcc_managed_message_ref_t *new_ref,
    dcc_status_t storage_status,
    void *user_data
);

typedef struct dcc_managed_message_options {
    size_t size;
    dcc_snowflake_t channel_id;
    const dcc_message_builder_t *message;
    dcc_managed_message_load_fn load;
    dcc_managed_message_save_fn save;
    void *storage_user_data;
    uint8_t keep_previous;
} dcc_managed_message_options_t;

DCC_API dcc_status_t dcc_managed_message_publish_latest(
    dcc_client_t *client,
    const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
