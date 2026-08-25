#include "internal/app/dcc_app_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"

#include <dcc/rest/channels/core.h>
#include <dcc/rest/guild_members/roles.h>
#include <dcc/rest/threads/create.h>

#include <stdio.h>

#define DCC_CTX_RESULT_RETURN(callback_, user_data_, endpoint_, ...)           \
  do {                                                                         \
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;              \
    options.callback = (callback_);                                            \
    options.user_data = (user_data_);                                          \
    return (endpoint_)(__VA_ARGS__, &options, NULL);                           \
  } while (0)

dcc_status_t dcc_ctx_add_member_role(dcc_ctx_t *ctx, dcc_snowflake_t user_id,
                                     dcc_snowflake_t role_id,
                                     dcc_rest_result_fn cb, void *user_data) {
  if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_guild_id(ctx) == 0U ||
      user_id == 0U || role_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_CTX_RESULT_RETURN(cb, user_data, dcc_rest_add_guild_member_role,
                        dcc_app_client(dcc_ctx_app(ctx)), dcc_ctx_guild_id(ctx),
                        user_id, role_id);
}

dcc_status_t dcc_ctx_remove_member_role(dcc_ctx_t *ctx, dcc_snowflake_t user_id,
                                        dcc_snowflake_t role_id,
                                        dcc_rest_result_fn cb,
                                        void *user_data) {
  if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_guild_id(ctx) == 0U ||
      user_id == 0U || role_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_CTX_RESULT_RETURN(cb, user_data, dcc_rest_remove_guild_member_role,
                        dcc_app_client(dcc_ctx_app(ctx)), dcc_ctx_guild_id(ctx),
                        user_id, role_id);
}

dcc_status_t
dcc_ctx_create_thread_from_message(dcc_ctx_t *ctx,
                                   const dcc_thread_params_t *params,
                                   dcc_rest_result_fn cb, void *user_data) {
  if (ctx == NULL || dcc_ctx_app(ctx) == NULL ||
      dcc_ctx_channel_id(ctx) == 0U || dcc_ctx_message_id(ctx) == 0U ||
      params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_CTX_RESULT_RETURN(cb, user_data, dcc_rest_create_thread_from_message,
                        dcc_app_client(dcc_ctx_app(ctx)),
                        dcc_ctx_channel_id(ctx), dcc_ctx_message_id(ctx),
                        params);
}

dcc_status_t dcc_ctx_create_thread_from_message_name(dcc_ctx_t *ctx,
                                                     const char *name,
                                                     dcc_rest_result_fn cb,
                                                     void *user_data) {
  if (name == NULL || name[0] == '\0') {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME;
  params.name = name;
  return dcc_ctx_create_thread_from_message(ctx, &params, cb, user_data);
}

static dcc_status_t
dcc_ctx_modify_current_thread(dcc_ctx_t *ctx, uint64_t present,
                              uint8_t archived, uint8_t locked,
                              dcc_rest_result_fn cb, void *user_data) {
  if (ctx == NULL || dcc_ctx_app(ctx) == NULL ||
      dcc_ctx_channel_id(ctx) == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
  params.kind = DCC_CHANNEL_PARAMS_THREAD;
  params.payload.thread.present = present;
  params.payload.thread.archived = archived;
  params.payload.thread.locked = locked;
  DCC_CTX_RESULT_RETURN(cb, user_data, dcc_rest_modify_channel,
                        dcc_app_client(dcc_ctx_app(ctx)),
                        dcc_ctx_channel_id(ctx), &params);
}

dcc_status_t dcc_ctx_archive_current_thread(dcc_ctx_t *ctx,
                                            dcc_rest_result_fn cb,
                                            void *user_data) {
  return dcc_ctx_modify_current_thread(ctx, DCC_CHANNEL_THREAD_PRESENT_ARCHIVED,
                                       1U, 0U, cb, user_data);
}

dcc_status_t dcc_ctx_lock_current_thread(dcc_ctx_t *ctx, dcc_rest_result_fn cb,
                                         void *user_data) {
  return dcc_ctx_modify_current_thread(ctx,
                                       DCC_CHANNEL_THREAD_PRESENT_ARCHIVED |
                                           DCC_CHANNEL_THREAD_PRESENT_LOCKED,
                                       1U, 1U, cb, user_data);
}

dcc_status_t dcc_ctx_unlock_current_thread(dcc_ctx_t *ctx,
                                           dcc_rest_result_fn cb,
                                           void *user_data) {
  return dcc_ctx_modify_current_thread(ctx, DCC_CHANNEL_THREAD_PRESENT_LOCKED,
                                       0U, 0U, cb, user_data);
}

dcc_status_t dcc_ctx_add_author_role(dcc_ctx_t *ctx, dcc_snowflake_t role_id,
                                     dcc_rest_result_fn cb, void *user_data) {
  return ctx != NULL && dcc_ctx_user_id(ctx) != 0U
             ? dcc_ctx_add_member_role(ctx, dcc_ctx_user_id(ctx), role_id, cb,
                                       user_data)
             : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_remove_author_role(dcc_ctx_t *ctx, dcc_snowflake_t role_id,
                                        dcc_rest_result_fn cb,
                                        void *user_data) {
  return ctx != NULL && dcc_ctx_user_id(ctx) != 0U
             ? dcc_ctx_remove_member_role(ctx, dcc_ctx_user_id(ctx), role_id,
                                          cb, user_data)
             : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_send(dcc_ctx_t *ctx, const dcc_message_builder_t *message,
                          dcc_rest_cb cb, void *user_data) {
  if (ctx == NULL || message == NULL || ctx->app == NULL ||
      dcc_ctx_channel_id(ctx) == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_send(ctx->app, dcc_ctx_channel_id(ctx), message, cb,
                      user_data);
}

dcc_status_t dcc_ctx_send_with_id(dcc_ctx_t *ctx,
                                  const dcc_message_builder_t *message,
                                  dcc_app_message_id_cb cb, void *user_data) {
  if (ctx == NULL || message == NULL || ctx->app == NULL ||
      dcc_ctx_channel_id(ctx) == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_send_with_id(ctx->app, dcc_ctx_channel_id(ctx), message, cb,
                              user_data);
}

dcc_status_t dcc_ctx_send_with_thread(dcc_ctx_t *ctx,
                                      const dcc_message_builder_t *message,
                                      const dcc_thread_params_t *thread,
                                      dcc_app_message_thread_cb cb,
                                      void *user_data) {
  if (ctx == NULL || message == NULL || thread == NULL || ctx->app == NULL ||
      dcc_ctx_channel_id(ctx) == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_send_with_thread(ctx->app, dcc_ctx_channel_id(ctx), message,
                                  thread, cb, user_data);
}

dcc_status_t dcc_ctx_send_with_thread_name(dcc_ctx_t *ctx,
                                           const dcc_message_builder_t *message,
                                           const char *thread_name,
                                           dcc_app_message_thread_cb cb,
                                           void *user_data) {
  if (ctx == NULL || message == NULL || thread_name == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_ctx_send_with_thread(
      ctx, message,
      &((dcc_thread_params_t[]){
          {.size = sizeof(dcc_thread_params_t), .name = thread_name}})[0],
      cb, user_data);
}

dcc_status_t dcc_ctx_send_text(dcc_ctx_t *ctx, const char *content,
                               dcc_rest_cb cb, void *user_data) {
  if (ctx == NULL || content == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_message_builder_t message = {
      .size = sizeof(message),
      .version = DCC_MESSAGE_BUILDER_VERSION,
      .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
      .content = content,
  };
  return dcc_ctx_send(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_send_text_with_id(dcc_ctx_t *ctx, const char *content,
                                       dcc_app_message_id_cb cb,
                                       void *user_data) {
  if (ctx == NULL || content == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_message_builder_t message = {
      .size = sizeof(message),
      .version = DCC_MESSAGE_BUILDER_VERSION,
      .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
      .content = content,
  };
  return dcc_ctx_send_with_id(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_send_text_with_thread(dcc_ctx_t *ctx, const char *content,
                                           const char *thread_name,
                                           dcc_app_message_thread_cb cb,
                                           void *user_data) {
  if (ctx == NULL || content == NULL || thread_name == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_message_builder_t message = {
      .size = sizeof(message),
      .version = DCC_MESSAGE_BUILDER_VERSION,
      .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
      .content = content,
  };
  return dcc_ctx_send_with_thread_name(ctx, &message, thread_name, cb,
                                       user_data);
}

dcc_status_t dcc_ctx_ok(dcc_ctx_t *ctx, const char *description,
                        dcc_rest_result_fn cb, void *user_data) {
  return dcc_ctx_reply_success(ctx, "Done", description, cb, user_data);
}

dcc_status_t dcc_ctx_fail(dcc_ctx_t *ctx, const char *description,
                          dcc_rest_result_fn cb, void *user_data) {
  return dcc_ctx_reply_error(ctx, "Error", description, cb, user_data);
}

dcc_status_t dcc_ctx_reply_validation_error(dcc_ctx_t *ctx, dcc_status_t status,
                                            dcc_rest_result_fn cb,
                                            void *user_data) {
  const char *description = "Check the command or form input and try again.";
  if (status == DCC_ERR_NOT_FOUND) {
    description = "A required input is missing. Fill it in and try again.";
  } else if (status == DCC_ERR_INVALID_ARG) {
    description =
        "One of the inputs is invalid. Check the values and try again.";
  }
  return dcc_ctx_reply_error(ctx, "Invalid input", description, cb, user_data);
}

dcc_status_t
dcc_ctx_reply_validation_result(dcc_ctx_t *ctx, dcc_status_t status,
                                const dcc_ctx_validation_error_t *error,
                                dcc_rest_result_fn cb, void *user_data) {
  if (error == NULL || error->status == DCC_OK || error->field == NULL ||
      error->field[0] == '\0') {
    return dcc_ctx_reply_validation_error(ctx, status, cb, user_data);
  }

  const char *message = error->message != NULL && error->message[0] != '\0'
                            ? error->message
                            : "Check this input and try again.";
  char description[256];
  (void)snprintf(description, sizeof(description), "`%s`: %s", error->field,
                 message);
  return dcc_ctx_reply_error(ctx, "Invalid input", description, cb, user_data);
}
