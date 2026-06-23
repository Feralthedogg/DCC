#ifndef DCC_SUGAR_REPLY_CTX_ALIASES_H
#define DCC_SUGAR_REPLY_CTX_ALIASES_H

#include <dcc/sugar/reply/respond_aliases.h>
#include <dcc/sugar/reply/send.h>
#include <dcc/sugar/reply/modal_autocomplete.h>

#define DCC_CTX_REPLY(ctx_, message_) DCC_REPLY((ctx_), (message_))
#define DCC_CTX_REPLY_CB(ctx_, message_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_CTX_REPLY_TEXT(ctx_, content_) DCC_REPLY_TEXT((ctx_), (content_))
#define DCC_CTX_RESPOND(ctx_, message_) DCC_RESPOND((ctx_), (message_))
#define DCC_CTX_RESPOND_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_CTX_RESPOND_TEXT(ctx_, content_) DCC_RESPOND_TEXT((ctx_), (content_))
#define DCC_CTX_RESPOND_OR_EDIT(ctx_, message_) DCC_RESPOND_OR_EDIT((ctx_), (message_))
#define DCC_CTX_RESPOND_OR_EDIT_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_CTX_RESPOND_OR_EDIT_TEXT(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_TEXT((ctx_), (content_))
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_PRIVATE((ctx_), (content_))
#define DCC_CTX_RESPOND_OR_FOLLOWUP(ctx_, message_) DCC_RESPOND_OR_FOLLOWUP((ctx_), (message_))
#define DCC_CTX_RESPOND_OR_FOLLOWUP_TEXT(ctx_, content_) \
    DCC_RESPOND_OR_FOLLOWUP_TEXT((ctx_), (content_))
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE(ctx_, content_) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE((ctx_), (content_))
#define DCC_CTX_PUBLIC(ctx_, content_) DCC_PUBLIC((ctx_), (content_))
#define DCC_CTX_EPHEMERAL(ctx_, content_) DCC_EPHEMERAL((ctx_), (content_))
#define DCC_CTX_PRIVATE(ctx_, content_) DCC_PRIVATE((ctx_), (content_))
#define DCC_CTX_RESPOND_PRIVATE(ctx_, content_) DCC_RESPOND_PRIVATE((ctx_), (content_))
#define DCC_CTX_SILENT(ctx_, content_) DCC_REPLY_SILENT((ctx_), (content_))
#define DCC_CTX_RESPOND_SILENT(ctx_, content_) DCC_RESPOND_SILENT((ctx_), (content_))
#define DCC_CTX_NO_EMBEDS(ctx_, content_) DCC_REPLY_NO_EMBEDS((ctx_), (content_))
#define DCC_CTX_RESPOND_NO_EMBEDS(ctx_, content_) DCC_RESPOND_NO_EMBEDS((ctx_), (content_))
#define DCC_CTX_OK(ctx_, description_) DCC_DONE((ctx_), (description_))
#define DCC_CTX_DONE(ctx_, description_) DCC_DONE((ctx_), (description_))
#define DCC_CTX_RESPOND_DONE(ctx_, description_) DCC_RESPOND_DONE((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_DONE(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_DONE((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_FOLLOWUP_DONE(ctx_, description_) \
    DCC_RESPOND_OR_FOLLOWUP_DONE((ctx_), (description_))
#define DCC_CTX_FAIL(ctx_, description_) DCC_FAIL((ctx_), (description_))
#define DCC_CTX_RESPOND_FAIL(ctx_, description_) DCC_RESPOND_FAIL((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_FAIL(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_FAIL((ctx_), (description_))
#define DCC_CTX_DENY(ctx_, description_) DCC_DENY((ctx_), (description_))
#define DCC_CTX_RESPOND_DENY(ctx_, description_) DCC_RESPOND_DENY((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_DENY(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_DENY((ctx_), (description_))
#define DCC_CTX_BAD_INPUT(ctx_, description_) DCC_BAD_INPUT((ctx_), (description_))
#define DCC_CTX_RESPOND_BAD_INPUT(ctx_, description_) DCC_RESPOND_BAD_INPUT((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_BAD_INPUT(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_BAD_INPUT((ctx_), (description_))
#define DCC_CTX_NOT_FOUND(ctx_, description_) DCC_NOT_FOUND((ctx_), (description_))
#define DCC_CTX_RESPOND_NOT_FOUND(ctx_, description_) DCC_RESPOND_NOT_FOUND((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_NOT_FOUND(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_NOT_FOUND((ctx_), (description_))
#define DCC_CTX_BUSY(ctx_, description_) DCC_BUSY((ctx_), (description_))
#define DCC_CTX_RESPOND_BUSY(ctx_, description_) DCC_RESPOND_BUSY((ctx_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_BUSY(ctx_, description_) \
    DCC_RESPOND_OR_EDIT_BUSY((ctx_), (description_))
#define DCC_CTX_SUCCESS(ctx_, title_, description_) DCC_SUCCESS((ctx_), (title_), (description_))
#define DCC_CTX_RESPOND_SUCCESS(ctx_, title_, description_) \
    DCC_RESPOND_SUCCESS((ctx_), (title_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_SUCCESS(ctx_, title_, description_) \
    DCC_RESPOND_OR_EDIT_SUCCESS((ctx_), (title_), (description_))
#define DCC_CTX_ERROR(ctx_, title_, description_) DCC_ERROR((ctx_), (title_), (description_))
#define DCC_CTX_RESPOND_ERROR(ctx_, title_, description_) \
    DCC_RESPOND_ERROR((ctx_), (title_), (description_))
#define DCC_CTX_RESPOND_OR_EDIT_ERROR(ctx_, title_, description_) \
    DCC_RESPOND_OR_EDIT_ERROR((ctx_), (title_), (description_))
#define DCC_CTX_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_REPLY_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_RESPOND_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_REPLY_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_RESPOND_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_REPLY_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_RESPOND_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_CTX_INTERNAL_ERROR(ctx_) DCC_INTERNAL_ERROR((ctx_))
#define DCC_CTX_RESPOND_INTERNAL_ERROR(ctx_) DCC_RESPOND_INTERNAL_ERROR((ctx_))
#define DCC_CTX_RESPOND_OR_EDIT_INTERNAL_ERROR(ctx_) DCC_RESPOND_OR_EDIT_INTERNAL_ERROR((ctx_))
#define DCC_CTX_DEFER(ctx_) DCC_DEFER((ctx_))
#define DCC_CTX_DEFER_EPHEMERAL(ctx_) DCC_DEFER_EPHEMERAL((ctx_))
#define DCC_CTX_DEFER_PRIVATE(ctx_) DCC_DEFER_PRIVATE((ctx_))
#define DCC_CTX_RESPOND_DEFER(ctx_) DCC_RESPOND_DEFER((ctx_))
#define DCC_CTX_RESPOND_DEFER_EPHEMERAL(ctx_) DCC_RESPOND_DEFER_EPHEMERAL((ctx_))
#define DCC_CTX_RESPOND_DEFER_PRIVATE(ctx_) DCC_RESPOND_DEFER_PRIVATE((ctx_))
#define DCC_CTX_UPDATE(ctx_, message_) DCC_UPDATE((ctx_), (message_))
#define DCC_CTX_EDIT(ctx_, message_) DCC_EDIT((ctx_), (message_))
#define DCC_CTX_EDIT_ORIGINAL(ctx_, message_) DCC_EDIT_ORIGINAL((ctx_), (message_))
#define DCC_CTX_DELETE_ORIGINAL(ctx_) DCC_DELETE_ORIGINAL((ctx_))
#define DCC_CTX_FOLLOWUP(ctx_, message_) DCC_FOLLOWUP((ctx_), (message_))
#define DCC_CTX_FOLLOWUP_TEXT(ctx_, content_) DCC_FOLLOWUP_TEXT((ctx_), (content_))
#define DCC_CTX_FOLLOWUP_EPHEMERAL(ctx_, content_) DCC_FOLLOWUP_EPHEMERAL((ctx_), (content_))
#define DCC_CTX_FOLLOWUP_PRIVATE(ctx_, content_) DCC_FOLLOWUP_PRIVATE((ctx_), (content_))
#define DCC_CTX_FOLLOWUP_SILENT(ctx_, content_) DCC_FOLLOWUP_SILENT((ctx_), (content_))
#define DCC_CTX_SEND(ctx_, message_) DCC_SEND((ctx_), (message_))
#define DCC_CTX_SEND_WITH_ID(ctx_, message_, cb_, user_data_) \
    DCC_SEND_WITH_ID((ctx_), (message_), (cb_), (user_data_))
#define DCC_CTX_SEND_ID(ctx_, message_, cb_, user_data_) \
    DCC_CTX_SEND_WITH_ID((ctx_), (message_), (cb_), (user_data_))
#define DCC_CTX_SEND_TEXT(ctx_, content_) DCC_SEND_TEXT((ctx_), (content_))
#define DCC_CTX_SEND_TEXT_WITH_ID(ctx_, content_, cb_, user_data_) \
    DCC_SEND_TEXT_WITH_ID((ctx_), (content_), (cb_), (user_data_))
#define DCC_CTX_SEND_TEXT_ID(ctx_, content_, cb_, user_data_) \
    DCC_CTX_SEND_TEXT_WITH_ID((ctx_), (content_), (cb_), (user_data_))
#define DCC_CTX_SHOW_MODAL(ctx_, modal_) DCC_SHOW_MODAL((ctx_), (modal_))
#define DCC_CTX_SHOW_MODAL_FLOW(ctx_, flow_) DCC_SHOW_MODAL_FLOW((ctx_), (flow_))
#define DCC_CTX_REPLY_AUTOCOMPLETE(ctx_, autocomplete_) DCC_REPLY_AUTOCOMPLETE((ctx_), (autocomplete_))
#define DCC_CTX_AUTOCOMPLETE(ctx_, autocomplete_) DCC_REPLY_AUTOCOMPLETE((ctx_), (autocomplete_))
#define DCC_CTX_AUTOCOMPLETE_MATCHING(ctx_, ...) \
    DCC_REPLY_AUTOCOMPLETE_MATCHING((ctx_), __VA_ARGS__)

#endif /* DCC_SUGAR_REPLY_CTX_ALIASES_H */
