#ifndef DCC_SUGAR_STATUS_H
#define DCC_SUGAR_STATUS_H

#include <dcc/app.h>
#include <dcc/error.h>
#include <dcc/sugar/colors.h>
#include <dcc/sugar/embeds.h>
#include <dcc/sugar/flags.h>
#include <dcc/sugar/handlers.h>
#include <dcc/sugar/mentions.h>

#define DCC_STATUS_EMBED(title_, description_, color_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U \
    })

#define DCC_SUCCESS_EMBED(title_, description_) \
    DCC_STATUS_EMBED((title_), (description_), DCC_COLOR_SUCCESS)

#define DCC_ERROR_EMBED(title_, description_) \
    DCC_STATUS_EMBED((title_), (description_), DCC_COLOR_ERROR)

#define DCC_WARNING_EMBED(title_, description_) \
    DCC_STATUS_EMBED((title_), (description_), DCC_COLOR_WARNING)

#define DCC_INFO_EMBED(title_, description_) \
    DCC_STATUS_EMBED((title_), (description_), DCC_COLOR_INFO)

#define DCC_MESSAGE_STATUS_FLAGS(title_, description_, color_, flags_) \
    ((dcc_message_builder_t){ \
        .embeds = (dcc_embed_builder_t[]){ DCC_STATUS_EMBED((title_), (description_), (color_)) }, \
        .embeds_count = 1U, \
        .allowed_mentions = &((dcc_allowed_mentions_builder_t[]){ DCC_ALLOWED_MENTIONS_NONE() })[0], \
        .flags = (flags_), \
        .has_flags = ((flags_) != 0U ? 1U : 0U) \
    })

#define DCC_MESSAGE_STATUS(title_, description_, color_) \
    DCC_MESSAGE_STATUS_FLAGS((title_), (description_), (color_), 0U)

#define DCC_MESSAGE_EPHEMERAL_STATUS(title_, description_, color_) \
    DCC_MESSAGE_STATUS_FLAGS((title_), (description_), (color_), DCC_MESSAGE_FLAG_EPHEMERAL)

#define DCC_MESSAGE_PRIVATE_STATUS(title_, description_, color_) \
    DCC_MESSAGE_EPHEMERAL_STATUS((title_), (description_), (color_))

#define DCC_MESSAGE_SILENT_STATUS(title_, description_, color_) \
    DCC_MESSAGE_STATUS_FLAGS((title_), (description_), (color_), DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)

#define DCC_MESSAGE_SUCCESS(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Done", (description_), DCC_COLOR_SUCCESS)

#define DCC_MESSAGE_OK(description_) \
    DCC_MESSAGE_SUCCESS((description_))

#define DCC_MESSAGE_PUBLIC_SUCCESS(description_) \
    DCC_MESSAGE_STATUS("Done", (description_), DCC_COLOR_SUCCESS)

#define DCC_MESSAGE_PUBLIC_OK(description_) \
    DCC_MESSAGE_PUBLIC_SUCCESS((description_))

#define DCC_MESSAGE_WARNING(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Warning", (description_), DCC_COLOR_WARNING)

#define DCC_MESSAGE_PUBLIC_WARNING(description_) \
    DCC_MESSAGE_STATUS("Warning", (description_), DCC_COLOR_WARNING)

#define DCC_MESSAGE_INFO(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Info", (description_), DCC_COLOR_INFO)

#define DCC_MESSAGE_PUBLIC_INFO(description_) \
    DCC_MESSAGE_STATUS("Info", (description_), DCC_COLOR_INFO)

#define DCC_MESSAGE_ERROR(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Error", (description_), DCC_COLOR_ERROR)

#define DCC_MESSAGE_PUBLIC_ERROR(description_) \
    DCC_MESSAGE_STATUS("Error", (description_), DCC_COLOR_ERROR)

#define DCC_MESSAGE_DENIED(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Permission denied", (description_), DCC_COLOR_ERROR)

#define DCC_MESSAGE_INVALID_INPUT(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Invalid input", (description_), DCC_COLOR_ERROR)

#define DCC_MESSAGE_NOT_FOUND_STATUS(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Not found", (description_), DCC_COLOR_WARNING)

#define DCC_MESSAGE_BUSY_STATUS(description_) \
    DCC_MESSAGE_EPHEMERAL_STATUS("Please wait", (description_), DCC_COLOR_WARNING)

static inline const char *dcc_sugar_error_description(dcc_status_t status, const char *message) {
    if (message != NULL && message[0] != '\0') {
        return message;
    }
    return dcc_status_string(status);
}

static inline const char *dcc_sugar_friendly_error_description(dcc_status_t status, const char *message) {
    if (message != NULL && message[0] != '\0') {
        return message;
    }
    if (status == DCC_ERR_INVALID_ARG) {
        return "The request was invalid. Check the input and try again.";
    }
    if (status == DCC_ERR_NOT_FOUND) {
        return "Required input was not found. Check the command or form fields and try again.";
    }
    if (status == DCC_ERR_STATE) {
        return "This action cannot be used right now.";
    }
    return dcc_status_string(status);
}

#define DCC_ERROR_DESCRIPTION(status_, message_) \
    dcc_sugar_error_description((status_), (message_))

#define DCC_FRIENDLY_ERROR_DESCRIPTION(status_, message_) \
    dcc_sugar_friendly_error_description((status_), (message_))

#define DCC_SIMPLE_ERROR_DESCRIPTION(status_, message_) \
    ((void)(status_), (void)(message_), "Please try again in a moment.")

#define DCC_CTX_REPLY_APP_ERROR(ctx_, status_, message_) \
    dcc_ctx_reply_error( \
        (ctx_), \
        "Action failed", \
        DCC_FRIENDLY_ERROR_DESCRIPTION((status_), (message_)), \
        NULL, \
        NULL \
    )

#define DCC_CTX_REPLY_VERBOSE_APP_ERROR(ctx_, status_, message_) \
    dcc_ctx_reply_error((ctx_), "Action failed", DCC_ERROR_DESCRIPTION((status_), (message_)), NULL, NULL)

#define DCC_CTX_REPLY_SIMPLE_APP_ERROR(ctx_, status_, message_) \
    dcc_ctx_reply_error((ctx_), "Action failed", DCC_SIMPLE_ERROR_DESCRIPTION((status_), (message_)), NULL, NULL)

#define DCC_FRIENDLY_ERROR_FN(name_) \
    DCC_ERROR_FN(name_) { \
        (void)user_data; \
        (void)DCC_CTX_REPLY_APP_ERROR(ctx, status, message); \
    }

#define DCC_PUBLIC_FRIENDLY_ERROR_FN(name_) \
    DCC_PUBLIC_ERROR_FN(name_) { \
        (void)user_data; \
        (void)DCC_CTX_REPLY_APP_ERROR(ctx, status, message); \
    }

#define DCC_SIMPLE_ERROR_FN(name_) \
    DCC_ERROR_FN(name_) { \
        (void)DCC_CTX_REPLY_SIMPLE_APP_ERROR(ctx, status, message); \
        (void)user_data; \
    }

#define DCC_PUBLIC_SIMPLE_ERROR_FN(name_) \
    DCC_PUBLIC_ERROR_FN(name_) { \
        (void)DCC_CTX_REPLY_SIMPLE_APP_ERROR(ctx, status, message); \
        (void)user_data; \
    }

#define DCC_VERBOSE_ERROR_FN(name_) \
    DCC_ERROR_FN(name_) { \
        (void)user_data; \
        (void)DCC_CTX_REPLY_VERBOSE_APP_ERROR(ctx, status, message); \
    }

#define DCC_PUBLIC_VERBOSE_ERROR_FN(name_) \
    DCC_PUBLIC_ERROR_FN(name_) { \
        (void)user_data; \
        (void)DCC_CTX_REPLY_VERBOSE_APP_ERROR(ctx, status, message); \
    }

#define DCC_REPLY_SAFE(ctx_, content_) \
    dcc_ctx_reply((ctx_), &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], NULL, NULL)

#define DCC_REPLY_SAFE_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_reply( \
        (ctx_), \
        &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], \
        (cb_), \
        (user_data_) \
    )

#define DCC_FOLLOWUP_SAFE(ctx_, content_) \
    dcc_ctx_followup((ctx_), &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], NULL, NULL)

#define DCC_SEND_SAFE(ctx_, content_) \
    dcc_ctx_send((ctx_), &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], NULL, NULL)

#define DCC_APP_SEND_SAFE(app_, channel_id_, content_) \
    dcc_app_send( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], \
        NULL, \
        NULL \
    )

#define DCC_APP_SEND_SAFE_CB(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ DCC_MESSAGE_NO_MENTIONS((content_)) })[0], \
        (cb_), \
        (user_data_) \
    )

#define DCC_REPLY_STATUS(ctx_, title_, description_, color_) \
    dcc_ctx_reply( \
        (ctx_), \
        &((dcc_message_builder_t[]){ DCC_MESSAGE_EPHEMERAL_STATUS((title_), (description_), (color_)) })[0], \
        NULL, \
        NULL \
    )

#define DCC_REPLY_OK(ctx_, description_) \
    DCC_REPLY_STATUS((ctx_), "Done", (description_), DCC_COLOR_SUCCESS)

#define DCC_REPLY_WARNING(ctx_, description_) \
    DCC_REPLY_STATUS((ctx_), "Warning", (description_), DCC_COLOR_WARNING)

#define DCC_REPLY_WARN(ctx_, description_) \
    DCC_REPLY_WARNING((ctx_), (description_))

#define DCC_REPLY_INFO(ctx_, description_) \
    DCC_REPLY_STATUS((ctx_), "Info", (description_), DCC_COLOR_INFO)

#define DCC_REPLY_PUBLIC_STATUS(ctx_, title_, description_, color_) \
    dcc_ctx_reply( \
        (ctx_), \
        &((dcc_message_builder_t[]){ DCC_MESSAGE_STATUS((title_), (description_), (color_)) })[0], \
        NULL, \
        NULL \
    )

#define DCC_REPLY_PUBLIC_OK(ctx_, description_) \
    DCC_REPLY_PUBLIC_STATUS((ctx_), "Done", (description_), DCC_COLOR_SUCCESS)

#define DCC_REPLY_PUBLIC_WARNING(ctx_, description_) \
    DCC_REPLY_PUBLIC_STATUS((ctx_), "Warning", (description_), DCC_COLOR_WARNING)

#define DCC_REPLY_PUBLIC_WARN(ctx_, description_) \
    DCC_REPLY_PUBLIC_WARNING((ctx_), (description_))

#define DCC_REPLY_PUBLIC_INFO(ctx_, description_) \
    DCC_REPLY_PUBLIC_STATUS((ctx_), "Info", (description_), DCC_COLOR_INFO)

#define DCC_CTX_REPLY_SAFE(ctx_, content_) DCC_REPLY_SAFE((ctx_), (content_))
#define DCC_CTX_FOLLOWUP_SAFE(ctx_, content_) DCC_FOLLOWUP_SAFE((ctx_), (content_))
#define DCC_CTX_SEND_SAFE(ctx_, content_) DCC_SEND_SAFE((ctx_), (content_))
#define DCC_CTX_REPLY_STATUS(ctx_, title_, description_, color_) \
    DCC_REPLY_STATUS((ctx_), (title_), (description_), (color_))
#define DCC_CTX_REPLY_PUBLIC_STATUS(ctx_, title_, description_, color_) \
    DCC_REPLY_PUBLIC_STATUS((ctx_), (title_), (description_), (color_))
#define DCC_CTX_REPLY_OK(ctx_, description_) DCC_REPLY_OK((ctx_), (description_))
#define DCC_CTX_REPLY_WARNING(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_CTX_REPLY_WARN(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_CTX_REPLY_INFO(ctx_, description_) DCC_REPLY_INFO((ctx_), (description_))
#define DCC_CTX_REPLY_PUBLIC_OK(ctx_, description_) \
    DCC_REPLY_PUBLIC_OK((ctx_), (description_))
#define DCC_CTX_REPLY_PUBLIC_WARNING(ctx_, description_) \
    DCC_REPLY_PUBLIC_WARNING((ctx_), (description_))
#define DCC_CTX_REPLY_PUBLIC_WARN(ctx_, description_) \
    DCC_REPLY_PUBLIC_WARNING((ctx_), (description_))
#define DCC_CTX_REPLY_PUBLIC_INFO(ctx_, description_) \
    DCC_REPLY_PUBLIC_INFO((ctx_), (description_))
#define DCC_WARNING(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_WARN(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_INFO(ctx_, description_) DCC_REPLY_INFO((ctx_), (description_))
#define DCC_CTX_WARNING(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_CTX_WARN(ctx_, description_) DCC_REPLY_WARNING((ctx_), (description_))
#define DCC_CTX_INFO(ctx_, description_) DCC_REPLY_INFO((ctx_), (description_))

#endif
