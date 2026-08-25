#ifndef DCC_MANAGED_MESSAGE_H
#define DCC_MANAGED_MESSAGE_H

#include <dcc/message.h>
#include <dcc/rest/base.h>
#include <dcc/rest/result.h>
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
    dcc_managed_message_ref_t *out_ref, void *user_data);
typedef dcc_status_t (*dcc_managed_message_save_fn)(
    const dcc_managed_message_ref_t *ref, void *user_data);
typedef enum dcc_managed_message_publish_stage {
  DCC_MANAGED_MESSAGE_STAGE_LOAD = 0,
  DCC_MANAGED_MESSAGE_STAGE_DELETE = 1,
  DCC_MANAGED_MESSAGE_STAGE_CREATE = 2,
  DCC_MANAGED_MESSAGE_STAGE_SAVE = 3,
  DCC_MANAGED_MESSAGE_STAGE_DONE = 4
} dcc_managed_message_publish_stage_t;

#define DCC_MANAGED_MESSAGE_PUBLISH_RESULT_VERSION 1U

typedef struct dcc_managed_message_publish_result {
  size_t size;
  uint32_t version;
  dcc_status_t status;
  dcc_managed_message_publish_stage_t stage;
  const dcc_rest_result_t *rest_result;
  dcc_managed_message_ref_t new_ref;
  dcc_status_t storage_status;
  uint8_t has_new_ref;
} dcc_managed_message_publish_result_t;

typedef void (*dcc_managed_message_publish_fn)(
    dcc_client_t *client, const dcc_managed_message_publish_result_t *result,
    void *user_data);

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
    dcc_client_t *client, const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_fn cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
