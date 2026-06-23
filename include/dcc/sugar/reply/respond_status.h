#ifndef DCC_SUGAR_REPLY_RESPOND_STATUS_H
#define DCC_SUGAR_REPLY_RESPOND_STATUS_H

#include <dcc/sugar/reply/respond_embeds.h>

#define DCC_RESPOND_ERROR(ctx_, title_, description_) \
    DCC_REPLY_ERROR((ctx_), (title_), (description_))
#define DCC_RESPOND_ERROR_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_REPLY_ERROR_CB((ctx_), (title_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_ERROR(ctx_, title_, description_) \
    DCC_RESPOND_ERROR((ctx_), (title_), (description_))
#define DCC_RESPOND_OR_EDIT_ERROR_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_RESPOND_ERROR_CB((ctx_), (title_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_SUCCESS(ctx_, title_, description_) \
    DCC_REPLY_SUCCESS((ctx_), (title_), (description_))
#define DCC_RESPOND_SUCCESS_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_REPLY_SUCCESS_CB((ctx_), (title_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_SUCCESS(ctx_, title_, description_) \
    DCC_RESPOND_SUCCESS((ctx_), (title_), (description_))
#define DCC_RESPOND_OR_EDIT_SUCCESS_CB(ctx_, title_, description_, cb_, user_data_) \
    DCC_RESPOND_SUCCESS_CB((ctx_), (title_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_DONE(ctx_, description_) DCC_REPLY_DONE((ctx_), (description_))
#define DCC_RESPOND_DONE_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_DONE_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_DONE(ctx_, description_) DCC_RESPOND_DONE((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_DONE_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_DONE_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_ACK(ctx_, description_) DCC_REPLY_ACK((ctx_), (description_))
#define DCC_RESPOND_ACK_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_ACK_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_ACK(ctx_, description_) DCC_RESPOND_ACK((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_ACK_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_ACK_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_FAIL(ctx_, description_) DCC_REPLY_FAIL((ctx_), (description_))
#define DCC_RESPOND_FAIL_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_FAIL_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_FAIL(ctx_, description_) DCC_RESPOND_FAIL((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_FAIL_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_FAIL_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_DENY(ctx_, description_) DCC_REPLY_DENY((ctx_), (description_))
#define DCC_RESPOND_DENY_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_DENY_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_DENY(ctx_, description_) DCC_RESPOND_DENY((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_DENY_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_DENY_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_BAD_INPUT(ctx_, description_) DCC_REPLY_BAD_INPUT((ctx_), (description_))
#define DCC_RESPOND_BAD_INPUT_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_BAD_INPUT_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_BAD_INPUT(ctx_, description_) \
    DCC_RESPOND_BAD_INPUT((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_BAD_INPUT_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_BAD_INPUT_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_NOT_FOUND(ctx_, description_) DCC_REPLY_NOT_FOUND((ctx_), (description_))
#define DCC_RESPOND_NOT_FOUND_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_NOT_FOUND_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_NOT_FOUND(ctx_, description_) \
    DCC_RESPOND_NOT_FOUND((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_NOT_FOUND_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_NOT_FOUND_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_BUSY(ctx_, description_) DCC_REPLY_BUSY((ctx_), (description_))
#define DCC_RESPOND_BUSY_CB(ctx_, description_, cb_, user_data_) \
    DCC_REPLY_BUSY_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_BUSY(ctx_, description_) DCC_RESPOND_BUSY((ctx_), (description_))
#define DCC_RESPOND_OR_EDIT_BUSY_CB(ctx_, description_, cb_, user_data_) \
    DCC_RESPOND_BUSY_CB((ctx_), (description_), (cb_), (user_data_))
#define DCC_RESPOND_INTERNAL_ERROR(ctx_) DCC_REPLY_INTERNAL_ERROR((ctx_))
#define DCC_RESPOND_INTERNAL_ERROR_CB(ctx_, cb_, user_data_) \
    DCC_REPLY_INTERNAL_ERROR_CB((ctx_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_INTERNAL_ERROR(ctx_) DCC_RESPOND_INTERNAL_ERROR((ctx_))
#define DCC_RESPOND_OR_EDIT_INTERNAL_ERROR_CB(ctx_, cb_, user_data_) \
    DCC_RESPOND_INTERNAL_ERROR_CB((ctx_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_ERROR(ctx_, title_, description_) \
    DCC_RESPOND_OR_EDIT_ERROR((ctx_), (title_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_SUCCESS(ctx_, title_, description_) \
    DCC_RESPOND_OR_EDIT_SUCCESS((ctx_), (title_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_DONE(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_DONE((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_ACK(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_ACK((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_FAIL(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_FAIL((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_DENY(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_DENY((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_BAD_INPUT(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_BAD_INPUT((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_NOT_FOUND(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_NOT_FOUND((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_BUSY(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_BUSY((ctx_), (description_))
#define DCC_RESPOND_OR_FOLLOWUP_INTERNAL_ERROR(ctx_) \
    DCC_RESPOND_OR_EDIT_INTERNAL_ERROR((ctx_))

#endif /* DCC_SUGAR_REPLY_RESPOND_STATUS_H */
