#ifndef DCC_SUGAR_REPLY_H
#define DCC_SUGAR_REPLY_H

#include <dcc/app.h>
#include <dcc/sugar/flags.h>
#include <dcc/sugar/messages.h>
#include <dcc/sugar/modal.h>

#define DCC_REPLY(ctx_, message_) \
    dcc_ctx_reply((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_REPLY_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_reply((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_REPLY_TEXT(ctx_, content_) \
    dcc_ctx_reply_text((ctx_), (content_), NULL, NULL)

#define DCC_REPLY_TEXT_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_reply_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_PUBLIC(ctx_, content_) \
    DCC_REPLY_TEXT((ctx_), (content_))

#define DCC_REPLY_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_PUBLIC(ctx_, content_) \
    DCC_REPLY_TEXT((ctx_), (content_))

#define DCC_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_EPHEMERAL(ctx_, content_) \
    dcc_ctx_reply_ephemeral_text((ctx_), (content_), NULL, NULL)

#define DCC_REPLY_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_reply_ephemeral_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_PRIVATE(ctx_, content_) \
    DCC_REPLY_EPHEMERAL((ctx_), (content_))

#define DCC_REPLY_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_SILENT(ctx_, content_) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)))

#define DCC_REPLY_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#define DCC_REPLY_NO_EMBEDS(ctx_, content_) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_NO_EMBEDS((content_)))

#define DCC_REPLY_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_TEXT_NO_EMBEDS((content_)), (cb_), (user_data_))

#define DCC_EPHEMERAL(ctx_, content_) \
    DCC_REPLY_EPHEMERAL((ctx_), (content_))

#define DCC_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_PRIVATE(ctx_, content_) \
    DCC_REPLY_PRIVATE((ctx_), (content_))

#define DCC_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))

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

#define DCC_REPLY_DEFER(ctx_) \
    dcc_ctx_defer((ctx_), NULL, NULL)

#define DCC_DEFER(ctx_) \
    DCC_REPLY_DEFER((ctx_))

#define DCC_REPLY_DEFER_EPHEMERAL(ctx_) \
    dcc_ctx_defer_ephemeral((ctx_), NULL, NULL)

#define DCC_DEFER_EPHEMERAL(ctx_) \
    DCC_REPLY_DEFER_EPHEMERAL((ctx_))

#define DCC_REPLY_DEFER_PRIVATE(ctx_) \
    DCC_REPLY_DEFER_EPHEMERAL((ctx_))

#define DCC_DEFER_PRIVATE(ctx_) \
    DCC_REPLY_DEFER_PRIVATE((ctx_))

#define DCC_UPDATE_MESSAGE(ctx_, message_) \
    dcc_ctx_update_message((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_UPDATE_MESSAGE_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_update_message((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_UPDATE(ctx_, message_) \
    DCC_UPDATE_MESSAGE((ctx_), (message_))

#define DCC_UPDATE_CB(ctx_, message_, cb_, user_data_) \
    DCC_UPDATE_MESSAGE_CB((ctx_), (message_), (cb_), (user_data_))

#define DCC_EDIT_ORIGINAL(ctx_, message_) \
    dcc_ctx_edit_original((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_EDIT_ORIGINAL_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_edit_original((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_EDIT(ctx_, message_) \
    DCC_EDIT_ORIGINAL((ctx_), (message_))

#define DCC_EDIT_CB(ctx_, message_, cb_, user_data_) \
    DCC_EDIT_ORIGINAL_CB((ctx_), (message_), (cb_), (user_data_))

#define DCC_DELETE_ORIGINAL(ctx_) \
    dcc_ctx_delete_original((ctx_), NULL, NULL)

#define DCC_DELETE_ORIGINAL_CB(ctx_, cb_, user_data_) \
    dcc_ctx_delete_original((ctx_), (cb_), (user_data_))

#define DCC_FOLLOWUP(ctx_, message_) \
    dcc_ctx_followup((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_FOLLOWUP_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_followup((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_FOLLOWUP_TEXT(ctx_, content_) \
    dcc_ctx_followup_text((ctx_), (content_), NULL, NULL)

#define DCC_FOLLOWUP_PUBLIC(ctx_, content_) \
    DCC_FOLLOWUP_TEXT((ctx_), (content_))

#define DCC_FOLLOWUP_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_followup_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_EPHEMERAL(ctx_, content_) \
    dcc_ctx_followup_ephemeral_text((ctx_), (content_), NULL, NULL)

#define DCC_FOLLOWUP_EPHEMERAL_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_EPHEMERAL((ctx_), (content_))

#define DCC_FOLLOWUP_EPHEMERAL_TEXT_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_followup_ephemeral_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_PRIVATE(ctx_, content_) \
    DCC_FOLLOWUP_EPHEMERAL((ctx_), (content_))

#define DCC_FOLLOWUP_PRIVATE_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_PRIVATE((ctx_), (content_))

#define DCC_FOLLOWUP_PRIVATE_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_FOLLOWUP_EPHEMERAL_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_SILENT(ctx_, content_) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)))

#define DCC_FOLLOWUP_SILENT_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_SILENT((ctx_), (content_))

#define DCC_FOLLOWUP_SILENT_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_FOLLOWUP_CB((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#define DCC_SEND(ctx_, message_) \
    dcc_ctx_send((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_SEND_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_send((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_SEND_WITH_ID(ctx_, message_, cb_, user_data_) \
    dcc_ctx_send_with_id((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_SEND_ID(ctx_, message_, cb_, user_data_) \
    DCC_SEND_WITH_ID((ctx_), (message_), (cb_), (user_data_))

#define DCC_SEND_THREAD_PARAMS(ctx_, message_, thread_, cb_, user_data_) \
    dcc_ctx_send_with_thread((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (thread_), (cb_), (user_data_))

#define DCC_SEND_THREAD(ctx_, message_, thread_name_) \
    dcc_ctx_send_with_thread_name( \
        (ctx_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        NULL, \
        NULL \
    )

#define DCC_SEND_THREAD_CB(ctx_, message_, thread_name_, cb_, user_data_) \
    dcc_ctx_send_with_thread_name( \
        (ctx_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )

#define DCC_SEND_TEXT(ctx_, content_) \
    dcc_ctx_send_text((ctx_), (content_), NULL, NULL)

#define DCC_SEND_TEXT_WITH_ID(ctx_, content_, cb_, user_data_) \
    dcc_ctx_send_text_with_id((ctx_), (content_), (cb_), (user_data_))

#define DCC_SEND_TEXT_ID(ctx_, content_, cb_, user_data_) \
    DCC_SEND_TEXT_WITH_ID((ctx_), (content_), (cb_), (user_data_))

#define DCC_SEND_TEXT_THREAD(ctx_, content_, thread_name_) \
    dcc_ctx_send_text_with_thread((ctx_), (content_), (thread_name_), NULL, NULL)

#define DCC_SEND_TEXT_THREAD_CB(ctx_, content_, thread_name_, cb_, user_data_) \
    dcc_ctx_send_text_with_thread((ctx_), (content_), (thread_name_), (cb_), (user_data_))

#define DCC_SEND_V2_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_THREAD((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_))

#define DCC_SEND_V2_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_THREAD_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_), (cb_), (user_data_))

#define DCC_SEND_UI_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_V2_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_SEND_UI_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_V2_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_CTX_SEND_THREAD(ctx_, message_, thread_name_) \
    DCC_SEND_THREAD((ctx_), (message_), (thread_name_))

#define DCC_CTX_SEND_THREAD_CB(ctx_, message_, thread_name_, cb_, user_data_) \
    DCC_SEND_THREAD_CB((ctx_), (message_), (thread_name_), (cb_), (user_data_))

#define DCC_CTX_SEND_THREAD_PARAMS(ctx_, message_, thread_, cb_, user_data_) \
    DCC_SEND_THREAD_PARAMS((ctx_), (message_), (thread_), (cb_), (user_data_))

#define DCC_CTX_SEND_TEXT_THREAD(ctx_, content_, thread_name_) \
    DCC_SEND_TEXT_THREAD((ctx_), (content_), (thread_name_))

#define DCC_CTX_SEND_TEXT_THREAD_CB(ctx_, content_, thread_name_, cb_, user_data_) \
    DCC_SEND_TEXT_THREAD_CB((ctx_), (content_), (thread_name_), (cb_), (user_data_))

#define DCC_CTX_SEND_V2_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_V2_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_CTX_SEND_V2_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_V2_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_CTX_SEND_UI_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_UI_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_CTX_SEND_UI_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_UI_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_SHOW_MODAL(ctx_, modal_) \
    dcc_ctx_show_modal((ctx_), &((dcc_modal_builder_t[]){ (modal_) })[0], NULL, NULL)

#define DCC_SHOW_MODAL_CB(ctx_, modal_, cb_, user_data_) \
    dcc_ctx_show_modal((ctx_), &((dcc_modal_builder_t[]){ (modal_) })[0], (cb_), (user_data_))

#define DCC_SHOW_MODAL_FLOW(ctx_, flow_) \
    dcc_ctx_show_modal_flow((ctx_), &((dcc_app_modal_flow_t[]){ (flow_) })[0], NULL, NULL)

#define DCC_SHOW_MODAL_FLOW_CB(ctx_, flow_, cb_, user_data_) \
    dcc_ctx_show_modal_flow((ctx_), &((dcc_app_modal_flow_t[]){ (flow_) })[0], (cb_), (user_data_))

#define DCC_SHOW_MODAL_BUILDER(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_BUILDER_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_SHOW_MODAL_V1(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_BUILDER((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V1_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_BUILDER_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_CTX_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_V2((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_CTX_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_V2_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_REPLY_AUTOCOMPLETE(ctx_, autocomplete_) \
    dcc_ctx_reply_autocomplete((ctx_), &((dcc_autocomplete_builder_t[]){ (autocomplete_) })[0], NULL, NULL)

#define DCC_REPLY_AUTOCOMPLETE_CB(ctx_, autocomplete_, cb_, user_data_) \
    dcc_ctx_reply_autocomplete((ctx_), &((dcc_autocomplete_builder_t[]){ (autocomplete_) })[0], (cb_), (user_data_))

#define DCC_REPLY_AUTOCOMPLETE_MATCHING(ctx_, ...) \
    dcc_ctx_reply_autocomplete_matching( \
        (ctx_), \
        (dcc_autocomplete_choice_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_autocomplete_choice_t[]){ __VA_ARGS__ }) / sizeof(dcc_autocomplete_choice_t), \
        DCC_AUTOCOMPLETE_MAX_CHOICES, \
        NULL, \
        NULL \
    )

#define DCC_REPLY_AUTOCOMPLETE_MATCHING_CB(ctx_, cb_, user_data_, ...) \
    dcc_ctx_reply_autocomplete_matching( \
        (ctx_), \
        (dcc_autocomplete_choice_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_autocomplete_choice_t[]){ __VA_ARGS__ }) / sizeof(dcc_autocomplete_choice_t), \
        DCC_AUTOCOMPLETE_MAX_CHOICES, \
        (cb_), \
        (user_data_) \
    )

#define DCC_REPLY_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_V2(ctx_, ...) DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_V2(ctx_, ...) DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_REPLY_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_REPLY_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2(ctx_, ...) DCC_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2(ctx_, ...) \
    DCC_UPDATE((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_UPDATE_V2(ctx_, ...) DCC_UPDATE_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2(ctx_, ...) DCC_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_COMPONENTS_V2(ctx_, ...) \
    DCC_SEND((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_SEND_V2(ctx_, ...) DCC_SEND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_WITH_ID((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2(ctx_, ...) DCC_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_REPLY_UI(ctx_, ...) DCC_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_UI(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_UI(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_UI(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_UI(ctx_, ...) DCC_CTX_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI(ctx_, ...) DCC_CTX_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_CTX_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_UI(ctx_, ...) DCC_CTX_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_UI(ctx_, ...) DCC_CTX_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_UI(ctx_, ...) DCC_CTX_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_UI(ctx_, ...) DCC_CTX_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_UI(ctx_, ...) DCC_CTX_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_RESPOND(ctx_, message_) DCC_REPLY((ctx_), (message_))
#define DCC_RESPOND_CB(ctx_, message_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_TEXT(ctx_, content_) DCC_REPLY_TEXT((ctx_), (content_))
#define DCC_RESPOND_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_PUBLIC(ctx_, content_) DCC_REPLY_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_EPHEMERAL(ctx_, content_) DCC_REPLY_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_PRIVATE(ctx_, content_) DCC_REPLY_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT(ctx_, message_) DCC_RESPOND((ctx_), (message_))
#define DCC_RESPOND_OR_EDIT_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_TEXT(ctx_, content_) DCC_RESPOND_TEXT((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PUBLIC(ctx_, content_) DCC_RESPOND_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL(ctx_, content_) DCC_RESPOND_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PRIVATE(ctx_, content_) DCC_RESPOND_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP(ctx_, message_) DCC_RESPOND_OR_EDIT((ctx_), (message_))
#define DCC_RESPOND_OR_FOLLOWUP_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_TEXT(ctx_, content_) DCC_RESPOND_OR_EDIT_TEXT((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC(ctx_, content_) DCC_RESPOND_OR_EDIT_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_SILENT(ctx_, content_) DCC_REPLY_SILENT((ctx_), (content_))
#define DCC_RESPOND_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_NO_EMBEDS(ctx_, content_) DCC_REPLY_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_SILENT(ctx_, content_) DCC_RESPOND_SILENT((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_NO_EMBEDS(ctx_, content_) DCC_RESPOND_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_SILENT(ctx_, content_) DCC_RESPOND_OR_EDIT_SILENT((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_NO_EMBEDS(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_REPLY_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_REPLY_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_RESPOND_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_OR_EDIT_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_RESPOND_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_RESPOND_OR_EDIT_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_OR_FOLLOWUP_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
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
#define DCC_RESPOND_DEFER(ctx_) DCC_REPLY_DEFER((ctx_))
#define DCC_RESPOND_DEFER_EPHEMERAL(ctx_) DCC_REPLY_DEFER_EPHEMERAL((ctx_))
#define DCC_RESPOND_DEFER_PRIVATE(ctx_) DCC_REPLY_DEFER_PRIVATE((ctx_))
#define DCC_RESPOND_AUTOCOMPLETE(ctx_, autocomplete_) \
    DCC_REPLY_AUTOCOMPLETE((ctx_), (autocomplete_))
#define DCC_RESPOND_AUTOCOMPLETE_CB(ctx_, autocomplete_, cb_, user_data_) \
    DCC_REPLY_AUTOCOMPLETE_CB((ctx_), (autocomplete_), (cb_), (user_data_))
#define DCC_RESPOND_AUTOCOMPLETE_MATCHING(ctx_, ...) \
    DCC_REPLY_AUTOCOMPLETE_MATCHING((ctx_), __VA_ARGS__)
#define DCC_RESPOND_AUTOCOMPLETE_MATCHING_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_AUTOCOMPLETE_MATCHING_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)

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

#endif
