#ifndef DCC_SUGAR_HANDLERS_H
#define DCC_SUGAR_HANDLERS_H

#include <dcc/app.h>
#include <dcc/sugar/app_extension.h>

#define DCC_HANDLER(name_) static void name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_PUBLIC_HANDLER(name_) void name_(dcc_ctx_t *ctx, void *user_data)

#define DCC_SLASH_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_SLASH_FN(name_) DCC_PUBLIC_HANDLER(name_)
#define DCC_COMMAND_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_COMMAND_FN(name_) DCC_PUBLIC_HANDLER(name_)
#define DCC_COMPONENT_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_COMPONENT_FN(name_) DCC_PUBLIC_HANDLER(name_)
#define DCC_BUTTON_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_BUTTON_FN(name_) DCC_PUBLIC_HANDLER(name_)
#define DCC_SELECT_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_SELECT_FN(name_) DCC_PUBLIC_HANDLER(name_)
#define DCC_MODAL_FN(name_) DCC_HANDLER(name_)
#define DCC_PUBLIC_MODAL_FN(name_) DCC_PUBLIC_HANDLER(name_)

#define DCC_TYPED_HANDLER(name_) static void name_(dcc_ctx_t *ctx, void *args, void *user_data)
#define DCC_PUBLIC_TYPED_HANDLER(name_) void name_(dcc_ctx_t *ctx, void *args, void *user_data)
#define DCC_TYPED_ARGS(type_, name_) const type_ *name_ = (const type_ *)args
#define DCC_TYPED_ARGS_MUT(type_, name_) type_ *name_ = (type_ *)args
#define DCC_TYPED_ARGS_REQUIRED(type_, name_) \
    const type_ *name_ = (const type_ *)args; \
    do { \
        if (name_ == NULL) { \
            return; \
        } \
    } while (0)
#define DCC_TYPED_ARGS_MUT_REQUIRED(type_, name_) \
    type_ *name_ = (type_ *)args; \
    do { \
        if (name_ == NULL) { \
            return; \
        } \
    } while (0)

#define DCC_NO_ARGS_TYPED_HANDLER_IMPL(name_) \
    static void name_##_no_args(dcc_ctx_t *ctx, void *user_data); \
    static void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        (void)args; \
        name_##_no_args(ctx, user_data); \
    } \
    static void name_##_no_args(dcc_ctx_t *ctx, void *user_data)
#define DCC_PUBLIC_NO_ARGS_TYPED_HANDLER_IMPL(name_) \
    void name_##_no_args(dcc_ctx_t *ctx, void *user_data); \
    void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        (void)args; \
        name_##_no_args(ctx, user_data); \
    } \
    void name_##_no_args(dcc_ctx_t *ctx, void *user_data)

#define DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_) \
    static void name_##_typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data); \
    static void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        name_##_typed(ctx, (const args_type_ *)args, user_data); \
    } \
    static void name_##_typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data)
#define DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_) \
    void name_##_typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data); \
    void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        name_##_typed(ctx, (const args_type_ *)args, user_data); \
    } \
    void name_##_typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data)
#define DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_) \
    static void name_##_typed(dcc_ctx_t *ctx, args_type_ *arg_, void *user_data); \
    static void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        name_##_typed(ctx, (args_type_ *)args, user_data); \
    } \
    static void name_##_typed(dcc_ctx_t *ctx, args_type_ *arg_, void *user_data)
#define DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_) \
    void name_##_typed(dcc_ctx_t *ctx, args_type_ *arg_, void *user_data); \
    void name_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        name_##_typed(ctx, (args_type_ *)args, user_data); \
    } \
    void name_##_typed(dcc_ctx_t *ctx, args_type_ *arg_, void *user_data)

#define DCC_TYPED_SLASH_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_SLASH_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_COMMAND_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_COMMAND_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_AUTOCOMPLETE_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_AUTOCOMPLETE_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_COMPONENT_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_COMPONENT_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_BUTTON_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_BUTTON_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_SELECT_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_SELECT_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)
#define DCC_TYPED_MODAL_FN(name_) DCC_TYPED_HANDLER(name_)
#define DCC_PUBLIC_TYPED_MODAL_FN(name_) DCC_PUBLIC_TYPED_HANDLER(name_)

#define DCC_TYPED_SLASH_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_SLASH_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_SLASH_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_SLASH_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SLASH_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_SLASH_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_COMMAND_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_COMMAND_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_COMMAND_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMMAND_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_COMMAND_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMMAND_IMPL(name_, args_type_, arg_)
#define DCC_SUBCOMMAND_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMMAND_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SUBCOMMAND_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMMAND_IMPL(name_, args_type_, arg_)
#define DCC_COMMAND_IMPL(name_) \
    DCC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_PUBLIC_COMMAND_IMPL(name_) \
    DCC_PUBLIC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_COMMAND_NO_ARGS_IMPL(name_) \
    DCC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_PUBLIC_COMMAND_NO_ARGS_IMPL(name_) \
    DCC_PUBLIC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_SLASH_IMPL(name_) \
    DCC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_PUBLIC_SLASH_IMPL(name_) \
    DCC_PUBLIC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_SLASH_NO_ARGS_IMPL(name_) \
    DCC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_PUBLIC_SLASH_NO_ARGS_IMPL(name_) \
    DCC_PUBLIC_NO_ARGS_TYPED_HANDLER_IMPL(name_)
#define DCC_TYPED_AUTOCOMPLETE_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_AUTOCOMPLETE_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_AUTOCOMPLETE_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_AUTOCOMPLETE_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_AUTOCOMPLETE_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_AUTOCOMPLETE_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_COMPONENT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_COMPONENT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_COMPONENT_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMPONENT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_COMPONENT_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMPONENT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_BUTTON_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_BUTTON_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_BUTTON_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_BUTTON_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_BUTTON_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_BUTTON_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_SELECT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_SELECT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_SELECT_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_SELECT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SELECT_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_SELECT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_MODAL_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_MODAL_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_IMPL(name_, args_type_, arg_)
#define DCC_MODAL_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_MODAL_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_MODAL_ARGS_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_MODAL_IMPL(name_, args_type_, arg_)

#define DCC_TYPED_SLASH_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_SLASH_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_SLASH_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_SLASH_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SLASH_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_SLASH_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_COMMAND_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_COMMAND_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_)
#define DCC_SUBCOMMAND_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SUBCOMMAND_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMMAND_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_AUTOCOMPLETE_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_AUTOCOMPLETE_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_AUTOCOMPLETE_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_AUTOCOMPLETE_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_AUTOCOMPLETE_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_AUTOCOMPLETE_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_COMPONENT_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_COMPONENT_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_COMPONENT_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_COMPONENT_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_COMPONENT_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_COMPONENT_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_BUTTON_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_BUTTON_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_BUTTON_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_BUTTON_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_BUTTON_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_BUTTON_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_SELECT_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_SELECT_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_SELECT_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_SELECT_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_SELECT_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_SELECT_MUT_IMPL(name_, args_type_, arg_)
#define DCC_TYPED_MODAL_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_TYPED_MODAL_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_HANDLER_MUT_IMPL(name_, args_type_, arg_)
#define DCC_MODAL_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_TYPED_MODAL_MUT_IMPL(name_, args_type_, arg_)
#define DCC_PUBLIC_MODAL_ARGS_MUT_IMPL(name_, args_type_, arg_) \
    DCC_PUBLIC_TYPED_MODAL_MUT_IMPL(name_, args_type_, arg_)

#define DCC_MIDDLEWARE_FN(name_) static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_PUBLIC_MIDDLEWARE_FN(name_) dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_CHECK_FN(name_) static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_PUBLIC_CHECK_FN(name_) dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_ERROR_FN(name_) \
    static void name_(dcc_ctx_t *ctx, dcc_status_t status, const char *message, void *user_data)
#define DCC_PUBLIC_ERROR_FN(name_) \
    void name_(dcc_ctx_t *ctx, dcc_status_t status, const char *message, void *user_data)

#define DCC_EVENT_FN(name_) \
    static void name_(dcc_app_t *app, const dcc_event_t *event, void *user_data)
#define DCC_PUBLIC_EVENT_FN(name_) \
    void name_(dcc_app_t *app, const dcc_event_t *event, void *user_data)
#define DCC_READY_FN(name_) \
    static void name_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)
#define DCC_PUBLIC_READY_FN(name_) \
    void name_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)
#define DCC_MESSAGE_FN(name_) \
    static void name_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const dcc_event_t *event, \
        void *user_data \
    )
#define DCC_PUBLIC_MESSAGE_FN(name_) \
    void name_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const dcc_event_t *event, \
        void *user_data \
    )
#define DCC_MESSAGE_COMMAND_FN(name_) \
    static void name_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )
#define DCC_PUBLIC_MESSAGE_COMMAND_FN(name_) \
    void name_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )
#define DCC_PREFIX_COMMAND_FN(name_) DCC_MESSAGE_COMMAND_FN(name_)
#define DCC_PUBLIC_PREFIX_COMMAND_FN(name_) DCC_PUBLIC_MESSAGE_COMMAND_FN(name_)
#define DCC_TEXT_COMMAND_FN(name_) DCC_MESSAGE_COMMAND_FN(name_)
#define DCC_PUBLIC_TEXT_COMMAND_FN(name_) DCC_PUBLIC_MESSAGE_COMMAND_FN(name_)

#define DCC_TASK_FN(name_) static void name_(dcc_app_t *app, void *user_data)
#define DCC_PUBLIC_TASK_FN(name_) void name_(dcc_app_t *app, void *user_data)
#define DCC_CLEANUP_FN(name_) static void name_(void *user_data)
#define DCC_PUBLIC_CLEANUP_FN(name_) void name_(void *user_data)
#define DCC_MODULE_SETUP_FN(name_) static dcc_status_t name_(dcc_app_t *app, void *user_data)
#define DCC_PUBLIC_MODULE_SETUP_FN(name_) dcc_status_t name_(dcc_app_t *app, void *user_data)

#define DCC_ON_SLASH_FN(name_, description_) \
    DCC_ON_SLASH(#name_, (description_), (name_))
#define DCC_ON_SLASH_FN_DATA(name_, description_, user_data_) \
    DCC_ON_SLASH_DATA(#name_, (description_), (name_), (user_data_))
#define DCC_ON_COMMAND_FN(name_, description_) \
    DCC_ON_COMMAND(#name_, (description_), (name_))
#define DCC_ON_COMMAND_FN_DATA(name_, description_, user_data_) \
    DCC_ON_COMMAND_DATA(#name_, (description_), (name_), (user_data_))
#define DCC_ON_SLASH_FN_GUARDED(name_, description_, ...) \
    DCC_ON_SLASH_GUARDED(#name_, (description_), (name_), __VA_ARGS__)
#define DCC_ON_SLASH_FN_DATA_GUARDED(name_, description_, user_data_, ...) \
    DCC_ON_SLASH_DATA_GUARDED(#name_, (description_), (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_COMMAND_FN_GUARDED(name_, description_, ...) \
    DCC_ON_COMMAND_GUARDED(#name_, (description_), (name_), __VA_ARGS__)
#define DCC_ON_COMMAND_FN_DATA_GUARDED(name_, description_, user_data_, ...) \
    DCC_ON_COMMAND_DATA_GUARDED(#name_, (description_), (name_), (user_data_), __VA_ARGS__)

#define DCC_ON_AUTOCOMPLETE_FN(name_) \
    DCC_ON_AUTOCOMPLETE(#name_, (name_))
#define DCC_ON_AUTOCOMPLETE_FN_DATA(name_, user_data_) \
    DCC_ON_AUTOCOMPLETE_DATA(#name_, (name_), (user_data_))
#define DCC_ON_AUTOCOMPLETE_FN_GUARDED(name_, ...) \
    DCC_ON_AUTOCOMPLETE_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_AUTOCOMPLETE_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_AUTOCOMPLETE_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)

#define DCC_ON_USER_CONTEXT_MENU_FN(name_) \
    DCC_ON_USER_CONTEXT_MENU(#name_, (name_))
#define DCC_ON_USER_CONTEXT_MENU_FN_DATA(name_, user_data_) \
    DCC_ON_USER_CONTEXT_MENU_DATA(#name_, (name_), (user_data_))
#define DCC_ON_USER_CONTEXT_MENU_FN_GUARDED(name_, ...) \
    DCC_ON_USER_CONTEXT_MENU_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_USER_CONTEXT_MENU_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_USER_CONTEXT_MENU_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_MESSAGE_CONTEXT_MENU_FN(name_) \
    DCC_ON_MESSAGE_CONTEXT_MENU(#name_, (name_))
#define DCC_ON_MESSAGE_CONTEXT_MENU_FN_DATA(name_, user_data_) \
    DCC_ON_MESSAGE_CONTEXT_MENU_DATA(#name_, (name_), (user_data_))
#define DCC_ON_MESSAGE_CONTEXT_MENU_FN_GUARDED(name_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_MESSAGE_CONTEXT_MENU_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)

#define DCC_ON_BUTTON_FN(name_) \
    DCC_ON_BUTTON(#name_, (name_))
#define DCC_ON_BUTTON_FN_DATA(name_, user_data_) \
    DCC_ON_BUTTON_DATA(#name_, (name_), (user_data_))
#define DCC_ON_BUTTON_FN_GUARDED(name_, ...) \
    DCC_ON_BUTTON_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_BUTTON_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_BUTTON_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_BUTTON_PREFIX_FN(name_) \
    DCC_ON_BUTTON_PREFIX(#name_, (name_))
#define DCC_ON_BUTTON_PREFIX_FN_DATA(name_, user_data_) \
    DCC_ON_BUTTON_PREFIX_DATA(#name_, (name_), (user_data_))
#define DCC_ON_BUTTON_PREFIX_FN_GUARDED(name_, ...) \
    DCC_ON_BUTTON_PREFIX_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_BUTTON_PREFIX_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_BUTTON_PREFIX_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_BUTTON_NS_FN(namespace_, name_) \
    DCC_ON_BUTTON_NS(namespace_, #name_, (name_))
#define DCC_ON_BUTTON_NS_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_BUTTON_NS_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_BUTTON_NS_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_BUTTON_NS_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_BUTTON_NS_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_BUTTON_NS_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_BUTTON_NS_PREFIX_FN(namespace_, name_) \
    DCC_ON_BUTTON_NS_PREFIX(namespace_, #name_, (name_))
#define DCC_ON_BUTTON_NS_PREFIX_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_BUTTON_NS_PREFIX_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_BUTTON_NS_PREFIX_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_BUTTON_NS_PREFIX_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_BUTTON_NS_PREFIX_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_BUTTON_NS_PREFIX_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)

#define DCC_ON_SELECT_FN(name_) \
    DCC_ON_SELECT(#name_, (name_))
#define DCC_ON_SELECT_FN_DATA(name_, user_data_) \
    DCC_ON_SELECT_DATA(#name_, (name_), (user_data_))
#define DCC_ON_SELECT_FN_GUARDED(name_, ...) \
    DCC_ON_SELECT_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_SELECT_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_SELECT_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_SELECT_PREFIX_FN(name_) \
    DCC_ON_SELECT_PREFIX(#name_, (name_))
#define DCC_ON_SELECT_PREFIX_FN_DATA(name_, user_data_) \
    DCC_ON_SELECT_PREFIX_DATA(#name_, (name_), (user_data_))
#define DCC_ON_SELECT_PREFIX_FN_GUARDED(name_, ...) \
    DCC_ON_SELECT_PREFIX_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_SELECT_PREFIX_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_SELECT_PREFIX_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_SELECT_NS_FN(namespace_, name_) \
    DCC_ON_SELECT_NS(namespace_, #name_, (name_))
#define DCC_ON_SELECT_NS_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_SELECT_NS_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_SELECT_NS_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_SELECT_NS_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_SELECT_NS_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_SELECT_NS_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_SELECT_NS_PREFIX_FN(namespace_, name_) \
    DCC_ON_SELECT_NS_PREFIX(namespace_, #name_, (name_))
#define DCC_ON_SELECT_NS_PREFIX_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_SELECT_NS_PREFIX_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_SELECT_NS_PREFIX_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_SELECT_NS_PREFIX_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_SELECT_NS_PREFIX_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_SELECT_NS_PREFIX_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)

#define DCC_ON_MODAL_FN(name_) \
    DCC_ON_MODAL(#name_, (name_))
#define DCC_ON_MODAL_FN_DATA(name_, user_data_) \
    DCC_ON_MODAL_DATA(#name_, (name_), (user_data_))
#define DCC_ON_MODAL_FN_GUARDED(name_, ...) \
    DCC_ON_MODAL_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_MODAL_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_MODAL_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_MODAL_PREFIX_FN(name_) \
    DCC_ON_MODAL_PREFIX(#name_, (name_))
#define DCC_ON_MODAL_PREFIX_FN_DATA(name_, user_data_) \
    DCC_ON_MODAL_PREFIX_DATA(#name_, (name_), (user_data_))
#define DCC_ON_MODAL_PREFIX_FN_GUARDED(name_, ...) \
    DCC_ON_MODAL_PREFIX_GUARDED(#name_, (name_), __VA_ARGS__)
#define DCC_ON_MODAL_PREFIX_FN_DATA_GUARDED(name_, user_data_, ...) \
    DCC_ON_MODAL_PREFIX_DATA_GUARDED(#name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_MODAL_NS_FN(namespace_, name_) \
    DCC_ON_MODAL_NS(namespace_, #name_, (name_))
#define DCC_ON_MODAL_NS_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_MODAL_NS_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_MODAL_NS_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_MODAL_NS_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_MODAL_NS_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_MODAL_NS_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)
#define DCC_ON_MODAL_NS_PREFIX_FN(namespace_, name_) \
    DCC_ON_MODAL_NS_PREFIX(namespace_, #name_, (name_))
#define DCC_ON_MODAL_NS_PREFIX_FN_DATA(namespace_, name_, user_data_) \
    DCC_ON_MODAL_NS_PREFIX_DATA(namespace_, #name_, (name_), (user_data_))
#define DCC_ON_MODAL_NS_PREFIX_FN_GUARDED(namespace_, name_, ...) \
    DCC_ON_MODAL_NS_PREFIX_GUARDED(namespace_, #name_, (name_), __VA_ARGS__)
#define DCC_ON_MODAL_NS_PREFIX_FN_DATA_GUARDED(namespace_, name_, user_data_, ...) \
    DCC_ON_MODAL_NS_PREFIX_DATA_GUARDED(namespace_, #name_, (name_), (user_data_), __VA_ARGS__)

#endif
