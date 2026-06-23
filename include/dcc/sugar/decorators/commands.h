#ifndef DCC_SUGAR_DECORATORS_COMMANDS_H
#define DCC_SUGAR_DECORATORS_COMMANDS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_SLASH_AS(handler_, command_name_, description_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_slash_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (command_name_), \
        .description = (description_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SLASH_AS_DATA_POLICY(handler_, command_name_, description_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_slash_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (command_name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SLASH_AS_DATA(handler_, command_name_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SLASH_AS_POLICY(handler_, command_name_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY(handler_, (command_name_), (description_), NULL, (policy_))

#define DCC_DECORATE_SLASH_AS_GUARDED(handler_, command_name_, description_, ...) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, (command_name_), (description_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_SLASH_AS_DATA_GUARDED(handler_, command_name_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SLASH(handler_, description_) \
    DCC_DECORATE_SLASH_AS(handler_, #handler_, (description_))

#define DCC_DECORATE_SLASH_DATA(handler_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA(handler_, #handler_, (description_), (user_data_))

#define DCC_DECORATE_SLASH_POLICY(handler_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, #handler_, (description_), (policy_))

#define DCC_DECORATE_SLASH_GUARDED(handler_, description_, ...) \
    DCC_DECORATE_SLASH_AS_GUARDED(handler_, #handler_, (description_), __VA_ARGS__)

#define DCC_DECORATE_SLASH_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_GUARDED(handler_, #handler_, (description_), (user_data_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_AS(handler_, command_name_, description_) \
    DCC_DECORATE_SLASH_AS(handler_, (command_name_), (description_))

#define DCC_DECORATE_COMMAND(handler_, description_) \
    DCC_DECORATE_SLASH(handler_, (description_))

#define DCC_DECORATE_COMMAND_AS_DATA(handler_, command_name_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA(handler_, (command_name_), (description_), (user_data_))

#define DCC_DECORATE_COMMAND_AS_POLICY(handler_, command_name_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, (command_name_), (description_), (policy_))

#define DCC_DECORATE_COMMAND_AS_GUARDED(handler_, command_name_, description_, ...) \
    DCC_DECORATE_SLASH_AS_GUARDED(handler_, (command_name_), (description_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_AS_DATA_GUARDED(handler_, command_name_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_GUARDED( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_COMMAND_DATA(handler_, description_, user_data_) \
    DCC_DECORATE_SLASH_DATA(handler_, (description_), (user_data_))

#define DCC_DECORATE_COMMAND_POLICY(handler_, description_, policy_) \
    DCC_DECORATE_SLASH_POLICY(handler_, (description_), (policy_))

#define DCC_DECORATE_COMMAND_GUARDED(handler_, description_, ...) \
    DCC_DECORATE_SLASH_GUARDED(handler_, (description_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_DATA_GUARDED(handler_, (description_), (user_data_), __VA_ARGS__)

#define DCC_DECORATE_SUBCOMMAND_AS(handler_, command_name_, subcommand_path_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_subcommand_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SUBCOMMAND_DATA_POLICY(handler_, command_name_, subcommand_path_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_subcommand_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SUBCOMMAND(handler_, command_name_, subcommand_path_) \
    DCC_DECORATE_SUBCOMMAND_AS(handler_, (command_name_), (subcommand_path_))

#define DCC_DECORATE_SUBCOMMAND_DATA(handler_, command_name_, subcommand_path_, user_data_) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (subcommand_path_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SUBCOMMAND_POLICY(handler_, command_name_, subcommand_path_, policy_) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY(handler_, (command_name_), (subcommand_path_), NULL, (policy_))

#define DCC_DECORATE_SUBCOMMAND_GUARDED(handler_, command_name_, subcommand_path_, ...) \
    DCC_DECORATE_SUBCOMMAND_POLICY(handler_, (command_name_), (subcommand_path_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_SUBCOMMAND_DATA_GUARDED(handler_, command_name_, subcommand_path_, user_data_, ...) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (subcommand_path_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_AUTOCOMPLETE_AS(handler_, command_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_autocomplete_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (command_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, command_name_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_autocomplete_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (command_name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_AUTOCOMPLETE(handler_) \
    DCC_DECORATE_AUTOCOMPLETE_AS(handler_, #handler_)

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA(handler_, command_name_, user_data_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, (command_name_), (user_data_), DCC_ROUTE_NO_POLICY())

#define DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, command_name_, policy_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, (command_name_), NULL, (policy_))

#define DCC_DECORATE_AUTOCOMPLETE_AS_GUARDED(handler_, command_name_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, (command_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA_GUARDED(handler_, command_name_, user_data_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_AUTOCOMPLETE_DATA(handler_, user_data_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_AUTOCOMPLETE_POLICY(handler_, policy_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, #handler_, (policy_))

#define DCC_DECORATE_AUTOCOMPLETE_GUARDED(handler_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_GUARDED(handler_, #handler_, __VA_ARGS__)

#define DCC_DECORATE_AUTOCOMPLETE_DATA_GUARDED(handler_, user_data_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_GUARDED(handler_, #handler_, (user_data_), __VA_ARGS__)

#define DCC_DECORATE_USER_CONTEXT_MENU_AS(handler_, menu_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        .name = (menu_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_CONTEXT_MENU_DATA_POLICY(handler_, type_, menu_name_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (menu_name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_USER_CONTEXT_MENU(handler_) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS(handler_, #handler_)

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_DATA(handler_, menu_name_, user_data_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        (menu_name_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_POLICY(handler_, menu_name_, policy_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY(handler_, DCC_APP_EXTENSION_USER_CONTEXT_MENU, (menu_name_), NULL, (policy_))

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_GUARDED(handler_, menu_name_, ...) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_POLICY(handler_, (menu_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_USER_CONTEXT_MENU_DATA(handler_, user_data_) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_USER_CONTEXT_MENU_GUARDED(handler_, ...) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_GUARDED(handler_, #handler_, __VA_ARGS__)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS(handler_, menu_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        .name = (menu_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU(handler_) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS(handler_, #handler_)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_DATA(handler_, menu_name_, user_data_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (menu_name_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_POLICY(handler_, menu_name_, policy_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (menu_name_), \
        NULL, \
        (policy_) \
    )

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_GUARDED(handler_, menu_name_, ...) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_POLICY(handler_, (menu_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_DATA(handler_, user_data_) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_GUARDED(handler_, ...) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_GUARDED(handler_, #handler_, __VA_ARGS__)


#endif /* DCC_SUGAR_DECORATORS_COMMANDS_H */
