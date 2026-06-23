#ifndef DCC_SUGAR_REPLY_STATUS_H
#define DCC_SUGAR_REPLY_STATUS_H

#include <dcc/sugar/reply/basic.h>

#define DCC_REPLY_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    dcc_ctx_reply_embed((ctx_), (title_), (description_), (color_), (ephemeral_), NULL, NULL)

#define DCC_REPLY_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    dcc_ctx_reply_embed((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))

#define DCC_REPLY_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EMBED((ctx_), (title_), (description_), (color_), 0U)

#define DCC_REPLY_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_EMBED_CB((ctx_), (title_), (description_), (color_), 0U, (cb_), (user_data_))

#define DCC_REPLY_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EMBED((ctx_), (title_), (description_), (color_), 1U)

#define DCC_REPLY_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_EMBED_CB((ctx_), (title_), (description_), (color_), 1U, (cb_), (user_data_))

#define DCC_REPLY_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))

#define DCC_REPLY_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))

#define DCC_REPLY_ERROR(ctx_, title_, description_) \
    dcc_ctx_reply_error((ctx_), (title_), (description_), NULL, NULL)

#define DCC_REPLY_ERROR_CB(ctx_, title_, description_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), (title_), (description_), (cb_), (user_data_))

#define DCC_ERROR(ctx_, title_, description_) \
    DCC_REPLY_ERROR((ctx_), (title_), (description_))

#define DCC_ERROR_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_REPLY_ERROR_CB((ctx_), (title_), (description_), (cb_), (user_data_))

#define DCC_REPLY_VALIDATION_ERROR(ctx_, status_) \
    dcc_ctx_reply_validation_error((ctx_), (status_), NULL, NULL)

#define DCC_REPLY_VALIDATION_ERROR_CB(ctx_, status_, cb_, user_data_) \
    dcc_ctx_reply_validation_error((ctx_), (status_), (cb_), (user_data_))

#define DCC_REPLY_VALIDATION_RESULT(ctx_, status_, error_) \
    dcc_ctx_reply_validation_result((ctx_), (status_), (error_), NULL, NULL)

#define DCC_REPLY_VALIDATION_RESULT_CB(ctx_, status_, error_, cb_, user_data_) \
    dcc_ctx_reply_validation_result((ctx_), (status_), (error_), (cb_), (user_data_))

#define DCC_REPLY_SUCCESS(ctx_, title_, description_) \
    dcc_ctx_reply_success((ctx_), (title_), (description_), NULL, NULL)

#define DCC_REPLY_SUCCESS_CB(ctx_, title_, description_, cb_, user_data_) \
    dcc_ctx_reply_success((ctx_), (title_), (description_), (cb_), (user_data_))

#define DCC_SUCCESS(ctx_, title_, description_) \
    DCC_REPLY_SUCCESS((ctx_), (title_), (description_))

#define DCC_SUCCESS_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_REPLY_SUCCESS_CB((ctx_), (title_), (description_), (cb_), (user_data_))

#define DCC_REPLY_DONE(ctx_, description_) \
    dcc_ctx_ok((ctx_), (description_), NULL, NULL)

#define DCC_REPLY_DONE_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_ok((ctx_), (description_), (cb_), (user_data_))

#define DCC_DONE(ctx_, description_) \
    DCC_REPLY_DONE((ctx_), (description_))

#define DCC_DONE_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_DONE_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_ACK(ctx_, description_) \
    DCC_REPLY_DONE((ctx_), (description_))

#define DCC_REPLY_ACK_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_DONE_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_ACK(ctx_, description_) \
    DCC_REPLY_ACK((ctx_), (description_))

#define DCC_ACK_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_ACK_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_FAIL(ctx_, description_) \
    dcc_ctx_fail((ctx_), (description_), NULL, NULL)

#define DCC_REPLY_FAIL_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_fail((ctx_), (description_), (cb_), (user_data_))

#define DCC_FAIL(ctx_, description_) \
    DCC_REPLY_FAIL((ctx_), (description_))

#define DCC_FAIL_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_FAIL_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_DENY(ctx_, description_) \
    dcc_ctx_reply_error((ctx_), "Permission denied", (description_), NULL, NULL)

#define DCC_REPLY_DENY_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), "Permission denied", (description_), (cb_), (user_data_))

#define DCC_DENY(ctx_, description_) \
    DCC_REPLY_DENY((ctx_), (description_))

#define DCC_DENY_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_DENY_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_BAD_INPUT(ctx_, description_) \
    dcc_ctx_reply_error((ctx_), "Invalid input", (description_), NULL, NULL)

#define DCC_REPLY_BAD_INPUT_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), "Invalid input", (description_), (cb_), (user_data_))

#define DCC_BAD_INPUT(ctx_, description_) \
    DCC_REPLY_BAD_INPUT((ctx_), (description_))

#define DCC_BAD_INPUT_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_BAD_INPUT_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_NOT_FOUND(ctx_, description_) \
    dcc_ctx_reply_error((ctx_), "Not found", (description_), NULL, NULL)

#define DCC_REPLY_NOT_FOUND_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), "Not found", (description_), (cb_), (user_data_))

#define DCC_NOT_FOUND(ctx_, description_) \
    DCC_REPLY_NOT_FOUND((ctx_), (description_))

#define DCC_NOT_FOUND_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_NOT_FOUND_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_BUSY(ctx_, description_) \
    dcc_ctx_reply_error((ctx_), "Please wait", (description_), NULL, NULL)

#define DCC_REPLY_BUSY_CB(ctx_, description_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), "Please wait", (description_), (cb_), (user_data_))

#define DCC_BUSY(ctx_, description_) \
    DCC_REPLY_BUSY((ctx_), (description_))

#define DCC_BUSY_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_BUSY_CB((ctx_), (description_), (cb_), (user_data_))

#define DCC_REPLY_INTERNAL_ERROR(ctx_) \
    dcc_ctx_reply_error((ctx_), "Something went wrong", "Try again later.", NULL, NULL)

#define DCC_REPLY_INTERNAL_ERROR_CB(ctx_, cb_, user_data_) \
    dcc_ctx_reply_error((ctx_), "Something went wrong", "Try again later.", (cb_), (user_data_))

#define DCC_INTERNAL_ERROR(ctx_) \
    DCC_REPLY_INTERNAL_ERROR((ctx_))

#define DCC_INTERNAL_ERROR_CB(ctx_, cb_, user_data_) \
    DCC_REPLY_INTERNAL_ERROR_CB((ctx_), (cb_), (user_data_))

#endif /* DCC_SUGAR_REPLY_STATUS_H */
