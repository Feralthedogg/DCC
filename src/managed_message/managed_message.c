#include <dcc/managed_message.h>
#include <dcc/rest/messages/create.h>
#include <dcc/rest/messages/edit.h>
#include <dcc/rest/response_helpers.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct dcc_managed_message_publish_state {
  dcc_client_t *client;
  dcc_snowflake_t channel_id;
  dcc_managed_message_ref_t new_ref;
  char *payload_json;
  dcc_managed_message_save_fn save;
  void *storage_user_data;
  dcc_managed_message_publish_fn callback;
  void *user_data;
} dcc_managed_message_publish_state_t;

static void
dcc_managed_message_state_free(dcc_managed_message_publish_state_t *state) {
  if (state != NULL) {
    dcc_message_builder_json_free(state->payload_json);
    free(state);
  }
}

static void
dcc_managed_message_finish(dcc_managed_message_publish_state_t *state,
                           dcc_status_t status,
                           dcc_managed_message_publish_stage_t stage,
                           const dcc_rest_result_t *rest_result,
                           dcc_status_t storage_status, uint8_t has_new_ref) {
  dcc_managed_message_publish_result_t result = {
      .size = sizeof(result),
      .version = DCC_MANAGED_MESSAGE_PUBLISH_RESULT_VERSION,
      .status = status,
      .stage = stage,
      .rest_result = rest_result,
      .new_ref = state->new_ref,
      .storage_status = storage_status,
      .has_new_ref = has_new_ref,
  };
  if (state->callback != NULL) {
    state->callback(state->client, &result, state->user_data);
  }
  dcc_managed_message_state_free(state);
}

static dcc_status_t
dcc_managed_message_result_id(const dcc_rest_result_t *result,
                              dcc_snowflake_t *out_id) {
  if (result == NULL || dcc_rest_result_status(result) != DCC_OK) {
    return result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  }
  dcc_rest_response_t response = {
      .size = sizeof(response),
      .status = result->http_status,
      .error = DCC_OK,
      .body = result->body,
      .body_len = result->body_len,
  };
  return dcc_rest_response_message_id(&response, out_id);
}

static void dcc_managed_message_create_result(dcc_client_t *client,
                                              const dcc_rest_result_t *result,
                                              void *user_data) {
  (void)client;
  dcc_managed_message_publish_state_t *state =
      (dcc_managed_message_publish_state_t *)user_data;
  dcc_status_t status =
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  if (status != DCC_OK) {
    dcc_managed_message_finish(state, status, DCC_MANAGED_MESSAGE_STAGE_CREATE,
                               result, DCC_OK, 0U);
    return;
  }
  dcc_snowflake_t message_id = 0U;
  status = dcc_managed_message_result_id(result, &message_id);
  if (status != DCC_OK) {
    dcc_managed_message_finish(state, status, DCC_MANAGED_MESSAGE_STAGE_CREATE,
                               result, DCC_OK, 0U);
    return;
  }
  state->new_ref.channel_id = state->channel_id;
  state->new_ref.message_id = message_id;
  dcc_status_t storage_status = DCC_OK;
  if (state->save != NULL) {
    storage_status = state->save(&state->new_ref, state->storage_user_data);
  }
  if (storage_status != DCC_OK) {
    dcc_managed_message_finish(state, storage_status,
                               DCC_MANAGED_MESSAGE_STAGE_SAVE, result,
                               storage_status, 1U);
    return;
  }
  dcc_managed_message_finish(state, DCC_OK, DCC_MANAGED_MESSAGE_STAGE_DONE,
                             result, DCC_OK, 1U);
}

static dcc_status_t
dcc_managed_message_create(dcc_managed_message_publish_state_t *state,
                           uint8_t accepted_composite) {
  char path[80];
  dcc_status_t status =
      dcc_rest_format_path(path, sizeof(path), "/channels/%llu/messages",
                           (unsigned long long)state->channel_id);
  dcc_endpoint_body_t body = {
      .data = state->payload_json,
      .len = state->payload_json != NULL ? strlen(state->payload_json) : 0U,
      .content_type = "application/json",
  };
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = dcc_managed_message_create_result;
  options.user_data = state;
  if (status == DCC_OK) {
    status = dcc_endpoint_submit_named(state->client, "dcc_rest_create_message",
                                       DCC_REST_POST, path, &body, &options,
                                       DCC_ENDPOINT_PATH_PUBLIC, NULL);
  }
  if (status != DCC_OK && accepted_composite != 0U) {
    dcc_managed_message_finish(state, status, DCC_MANAGED_MESSAGE_STAGE_CREATE,
                               NULL, DCC_OK, 0U);
  }
  return status;
}

static void dcc_managed_message_delete_result(dcc_client_t *client,
                                              const dcc_rest_result_t *result,
                                              void *user_data) {
  (void)client;
  dcc_managed_message_publish_state_t *state =
      (dcc_managed_message_publish_state_t *)user_data;
  dcc_status_t status =
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  if (result != NULL && result->http_status == 404U) {
    status = DCC_OK;
  }
  if (status != DCC_OK) {
    dcc_managed_message_finish(state, status, DCC_MANAGED_MESSAGE_STAGE_DELETE,
                               result, DCC_OK, 0U);
    return;
  }
  (void)dcc_managed_message_create(state, 1U);
}

dcc_status_t dcc_managed_message_publish_latest(
    dcc_client_t *client, const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_fn callback, void *user_data) {
  if (client == NULL || options == NULL || options->size < sizeof(*options) ||
      options->channel_id == 0U || options->message == NULL) {
    return DCC_ERR_INVALID_ARG;
  }

  dcc_managed_message_ref_t old_ref = {.channel_id = options->channel_id};
  if (options->load != NULL) {
    dcc_status_t status = options->load(&old_ref, options->storage_user_data);
    if (status == DCC_ERR_NOT_FOUND) {
      old_ref = (dcc_managed_message_ref_t){
          .channel_id = options->channel_id,
      };
    } else if (status != DCC_OK) {
      return status;
    }
  }
  if (old_ref.channel_id == 0U) {
    old_ref.channel_id = options->channel_id;
  }

  dcc_managed_message_publish_state_t *state =
      (dcc_managed_message_publish_state_t *)calloc(1U, sizeof(*state));
  if (state == NULL) {
    return DCC_ERR_NOMEM;
  }
  state->client = client;
  state->channel_id = options->channel_id;
  state->save = options->save;
  state->storage_user_data = options->storage_user_data;
  state->callback = callback;
  state->user_data = user_data;
  dcc_status_t status =
      dcc_message_builder_build_json(options->message, &state->payload_json);
  if (status != DCC_OK) {
    dcc_managed_message_state_free(state);
    return status;
  }

  if (options->keep_previous == 0U && old_ref.message_id != 0U) {
    dcc_rest_call_options_t call_options = DCC_REST_CALL_OPTIONS_INIT;
    call_options.callback = dcc_managed_message_delete_result;
    call_options.user_data = state;
    status = dcc_rest_delete_message(client, old_ref.channel_id,
                                     old_ref.message_id, &call_options, NULL);
  } else {
    status = dcc_managed_message_create(state, 0U);
  }
  if (status != DCC_OK) {
    dcc_managed_message_state_free(state);
  }
  return status;
}
