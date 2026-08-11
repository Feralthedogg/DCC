#ifndef DCC_APP_LEGACY_H
#define DCC_APP_LEGACY_H

#include <dcc/app/base.h>
#include <dcc/app/context.h>
#include <dcc/app/options.h>
#include <dcc/app/modules.h>
#include <dcc/app/env.h>
#include <dcc/application_command.h>
#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/component_session.h>
#include <dcc/command_registry.h>
#include <dcc/embed.h>
#include <dcc/events.h>
#include <dcc/interaction_flow.h>
#include <dcc/interaction_helpers.h>
#include <dcc/managed_message.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/permissions.h>
#include <dcc/rest/base.h>
#include <dcc/rest/types/auto_moderation.h>
#include <dcc/rest/types/channels.h>
#include <dcc/rest/types/emojis_stickers.h>
#include <dcc/rest/types/entitlements.h>
#include <dcc/rest/types/guilds.h>
#include <dcc/rest/types/guild_members.h>
#include <dcc/rest/types/invites.h>
#include <dcc/rest/types/messages.h>
#include <dcc/rest/types/onboarding.h>
#include <dcc/rest/types/roles.h>
#include <dcc/rest/types/scheduled_events.h>
#include <dcc/rest/types/stage.h>
#include <dcc/rest/types/templates.h>
#include <dcc/rest/types/threads.h>
#include <dcc/rest/types/users.h>
#include <dcc/rest/types/voice_states.h>
#include <dcc/rest/types/webhooks.h>
#include <dcc/store.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Transition-only DCC 1 callback surface. */
typedef uint64_t dcc_app_route_id_t;
#define DCC_APP_ROUTE_INVALID UINT64_C(0)
typedef void (*dcc_app_legacy_handler_fn)(dcc_ctx_t *ctx, void *user_data);
typedef void (*dcc_app_legacy_typed_handler_fn)(dcc_ctx_t *ctx, void *args, void *user_data);
typedef void (*dcc_app_legacy_event_fn)(dcc_app_t *app, const dcc_event_t *event, void *user_data);
typedef void (*dcc_app_legacy_ready_fn)(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data);
typedef void (*dcc_app_legacy_message_fn)(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data);
typedef void (*dcc_app_legacy_message_command_fn)(dcc_app_t *app, const dcc_message_t *message, const char *args, const dcc_event_t *event, void *user_data);
typedef void (*dcc_app_legacy_task_fn)(dcc_app_t *app, void *user_data);
typedef void (*dcc_app_message_id_cb)(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_status_t status,
    void *user_data
);
typedef void (*dcc_app_message_thread_cb)(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_status_t status,
    void *user_data
);
typedef void (*dcc_app_infer_guild_cb)(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t guild_id,
    dcc_status_t status,
    void *user_data
);
typedef enum dcc_app_view_item_type {
    DCC_APP_VIEW_BUTTON = 1,
    DCC_APP_VIEW_BUTTON_PREFIX,
    DCC_APP_VIEW_SELECT,
    DCC_APP_VIEW_SELECT_PREFIX,
    DCC_APP_VIEW_MODAL,
    DCC_APP_VIEW_MODAL_PREFIX
} dcc_app_view_item_type_t;

typedef struct dcc_app_view_item {
    size_t size;
    dcc_app_view_item_type_t type;
    const char *custom_id;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
} dcc_app_view_item_t;

typedef struct dcc_app_view {
    size_t size;
    const dcc_app_view_item_t *items;
    size_t item_count;
    void *user_data;
} dcc_app_view_t;

typedef struct dcc_app_component_session_route {
    size_t size;
    const dcc_component_session_t *session;
    const dcc_component_session_store_t *store;
    const dcc_component_session_listener_options_t *options;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
} dcc_app_component_session_route_t;

typedef struct dcc_app_route_group {
    size_t size;
    dcc_app_t *app;
    const char *custom_id_prefix;
    void *user_data;
} dcc_app_route_group_t;

typedef enum dcc_app_extension_component_route_type {
    DCC_APP_EXTENSION_BUTTON = 1,
    DCC_APP_EXTENSION_BUTTON_PREFIX,
    DCC_APP_EXTENSION_SELECT,
    DCC_APP_EXTENSION_SELECT_PREFIX,
    DCC_APP_EXTENSION_MODAL,
    DCC_APP_EXTENSION_MODAL_PREFIX
} dcc_app_extension_component_route_type_t;

typedef enum dcc_app_extension_event_type {
    DCC_APP_EXTENSION_EVENT = 1,
    DCC_APP_EXTENSION_READY,
    DCC_APP_EXTENSION_MESSAGE_CREATE,
    DCC_APP_EXTENSION_MESSAGE_UPDATE,
    DCC_APP_EXTENSION_MESSAGE_DELETE
} dcc_app_extension_event_type_t;

typedef enum dcc_app_extension_task_type {
    DCC_APP_EXTENSION_TASK_EVERY_MS = 1,
    DCC_APP_EXTENSION_TASK_EVERY_SECONDS,
    DCC_APP_EXTENSION_TASK_EVERY_MINUTES,
    DCC_APP_EXTENSION_TASK_EVERY_HOURS,
    DCC_APP_EXTENSION_TASK_EVERY_KST,
    DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST
} dcc_app_extension_task_type_t;

typedef struct dcc_app_extension_middleware {
    size_t size;
    dcc_app_middleware_fn middleware;
    void *user_data;
} dcc_app_extension_middleware_t;

typedef struct dcc_app_check {
    size_t size;
    dcc_app_check_fn check;
    void *user_data;
    const char *title;
    const char *description;
} dcc_app_check_t;

typedef enum dcc_app_cooldown_bucket {
    DCC_APP_COOLDOWN_GLOBAL = 1,
    DCC_APP_COOLDOWN_USER,
    DCC_APP_COOLDOWN_CHANNEL,
    DCC_APP_COOLDOWN_GUILD
} dcc_app_cooldown_bucket_t;

typedef struct dcc_app_cooldown_options {
    size_t size;
    dcc_app_cooldown_bucket_t bucket;
    uint32_t limit;
    uint64_t window_ms;
} dcc_app_cooldown_options_t;

typedef struct dcc_app_route_policy {
    size_t size;
    const dcc_app_extension_middleware_t *middlewares;
    size_t middleware_count;
    const dcc_snowflake_t *owner_user_ids;
    size_t owner_user_id_count;
    dcc_permission_t required_permissions;
    uint8_t guild_only;
    dcc_app_cooldown_options_t cooldown;
    const dcc_app_check_t *checks;
    size_t check_count;
    uint8_t dm_only;
    uint8_t nsfw_only;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_snowflake_t *required_role_ids;
    size_t required_role_id_count;
    const dcc_snowflake_t *any_role_ids;
    size_t any_role_id_count;
} dcc_app_route_policy_t;

typedef struct dcc_app_extension_slash_command {
    size_t size;
    const char *name;
    const char *description;
    const dcc_application_command_builder_t *builder;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_slash_command_t;

typedef struct dcc_app_extension_subcommand {
    size_t size;
    const char *command_name;
    const char *subcommand_path;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_subcommand_t;

typedef struct dcc_app_extension_autocomplete {
    size_t size;
    const char *name;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_autocomplete_t;

typedef enum dcc_app_extension_context_menu_type {
    DCC_APP_EXTENSION_USER_CONTEXT_MENU = 1,
    DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU
} dcc_app_extension_context_menu_type_t;

typedef struct dcc_app_extension_context_menu {
    size_t size;
    dcc_app_extension_context_menu_type_t type;
    const char *name;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_context_menu_t;

typedef struct dcc_app_extension_component_route {
    size_t size;
    dcc_app_extension_component_route_type_t type;
    const char *custom_id;
    dcc_app_legacy_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_component_route_t;

typedef struct dcc_app_extension_event {
    size_t size;
    dcc_app_extension_event_type_t type;
    dcc_event_type_t event_type;
    dcc_app_legacy_event_fn event_handler;
    dcc_app_legacy_ready_fn ready_handler;
    dcc_app_legacy_message_fn message_handler;
    void *user_data;
    uint8_t once;
} dcc_app_extension_event_t;

typedef struct dcc_app_extension_message_command {
    size_t size;
    const char *prefix;
    const char *name;
    dcc_app_legacy_message_command_fn handler;
    void *user_data;
} dcc_app_extension_message_command_t;

typedef struct dcc_app_extension_task {
    size_t size;
    dcc_app_extension_task_type_t type;
    uint64_t interval;
    const char *hhmm;
    uint8_t hour;
    uint8_t minute;
    dcc_app_legacy_task_fn handler;
    void *user_data;
} dcc_app_extension_task_t;

typedef struct dcc_app_typed_slash_command dcc_app_typed_slash_command_t;
typedef struct dcc_app_typed_subcommand dcc_app_typed_subcommand_t;
typedef struct dcc_app_typed_autocomplete dcc_app_typed_autocomplete_t;
typedef struct dcc_app_typed_modal dcc_app_typed_modal_t;
typedef struct dcc_app_typed_component dcc_app_typed_component_t;

typedef struct dcc_app_extension {
    size_t size;
    const char *name;
    const dcc_app_extension_middleware_t *middlewares;
    size_t middleware_count;
    const dcc_app_extension_slash_command_t *slash_commands;
    size_t slash_command_count;
    const dcc_app_extension_subcommand_t *subcommands;
    size_t subcommand_count;
    const dcc_app_extension_autocomplete_t *autocompletes;
    size_t autocomplete_count;
    const dcc_app_extension_component_route_t *component_routes;
    size_t component_route_count;
    const dcc_app_view_t *views;
    size_t view_count;
    const dcc_app_extension_event_t *events;
    size_t event_count;
    const dcc_app_extension_task_t *tasks;
    size_t task_count;
    const dcc_app_typed_slash_command_t *typed_slash_commands;
    size_t typed_slash_command_count;
    const dcc_app_typed_modal_t *typed_modals;
    size_t typed_modal_count;
    const dcc_app_typed_component_t *typed_buttons;
    size_t typed_button_count;
    const dcc_app_typed_component_t *typed_selects;
    size_t typed_select_count;
    const dcc_app_extension_message_command_t *message_commands;
    size_t message_command_count;
    const dcc_app_typed_subcommand_t *typed_subcommands;
    size_t typed_subcommand_count;
    const dcc_app_typed_autocomplete_t *typed_autocompletes;
    size_t typed_autocomplete_count;
    const dcc_app_extension_context_menu_t *context_menus;
    size_t context_menu_count;
} dcc_app_extension_t;

struct dcc_app_typed_slash_command {
    size_t size;
    const dcc_application_command_builder_t *command;
    size_t args_size;
    dcc_ctx_option_field_binding_list_t bindings;
    dcc_ctx_field_validator_list_t validators;
    dcc_app_legacy_typed_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    uint8_t suppress_validation_response;
    dcc_app_route_policy_t policy;
};

struct dcc_app_typed_subcommand {
    size_t size;
    const dcc_application_command_builder_t *command;
    const char *command_name;
    const char *subcommand_path;
    size_t args_size;
    dcc_ctx_option_field_binding_list_t bindings;
    dcc_ctx_field_validator_list_t validators;
    dcc_app_legacy_typed_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    uint8_t suppress_validation_response;
    dcc_app_route_policy_t policy;
};

struct dcc_app_typed_autocomplete {
    size_t size;
    const dcc_application_command_builder_t *command;
    const char *command_name;
    const char *subcommand_path;
    size_t args_size;
    dcc_ctx_option_field_binding_list_t bindings;
    dcc_ctx_field_validator_list_t validators;
    dcc_app_legacy_typed_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    uint8_t suppress_validation_response;
    dcc_app_route_policy_t policy;
};

struct dcc_app_typed_modal {
    size_t size;
    const char *custom_id;
    size_t args_size;
    dcc_ctx_form_field_binding_list_t bindings;
    dcc_ctx_field_validator_list_t validators;
    dcc_app_legacy_typed_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    uint8_t prefix;
    uint8_t suppress_validation_response;
    dcc_app_route_policy_t policy;
};

typedef struct dcc_app_modal_flow {
    size_t size;
    dcc_modal_builder_t modal;
    dcc_app_typed_modal_t route;
} dcc_app_modal_flow_t;

struct dcc_app_typed_component {
    size_t size;
    const char *custom_id;
    size_t args_size;
    dcc_ctx_component_field_binding_list_t bindings;
    dcc_ctx_field_validator_list_t validators;
    dcc_app_legacy_typed_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    uint8_t prefix;
    uint8_t suppress_validation_response;
    dcc_app_route_policy_t policy;
};

typedef enum dcc_app_listener_kind {
    DCC_APP_LISTENER_NONE = 0,
    DCC_APP_LISTENER_SLASH,
    DCC_APP_LISTENER_SUBCOMMAND,
    DCC_APP_LISTENER_AUTOCOMPLETE,
    DCC_APP_LISTENER_CONTEXT_MENU,
    DCC_APP_LISTENER_COMPONENT,
    DCC_APP_LISTENER_EVENT,
    DCC_APP_LISTENER_MESSAGE_COMMAND,
    DCC_APP_LISTENER_TASK,
    DCC_APP_LISTENER_TYPED_SLASH,
    DCC_APP_LISTENER_TYPED_SUBCOMMAND,
    DCC_APP_LISTENER_TYPED_AUTOCOMPLETE,
    DCC_APP_LISTENER_TYPED_MODAL,
    DCC_APP_LISTENER_TYPED_BUTTON,
    DCC_APP_LISTENER_TYPED_SELECT,
    DCC_APP_LISTENER_MODAL_FLOW,
    DCC_APP_LISTENER_COMMAND_SCHEMA
} dcc_app_listener_kind_t;

typedef struct dcc_app_listener {
    size_t size;
    dcc_app_listener_kind_t kind;
    dcc_app_extension_slash_command_t slash;
    dcc_app_extension_subcommand_t subcommand;
    dcc_app_extension_autocomplete_t autocomplete;
    dcc_app_extension_context_menu_t context_menu;
    dcc_app_extension_component_route_t component;
    dcc_app_extension_event_t event;
    dcc_app_extension_message_command_t message_command;
    dcc_app_extension_task_t task;
    dcc_app_typed_slash_command_t typed_slash;
    dcc_app_typed_subcommand_t typed_subcommand;
    dcc_app_typed_autocomplete_t typed_autocomplete;
    dcc_app_typed_modal_t typed_modal;
    dcc_app_typed_component_t typed_component;
    dcc_app_modal_flow_t modal_flow;
    dcc_application_command_builder_t command_schema;
} dcc_app_listener_t;

typedef struct dcc_app_listener_list {
    const dcc_app_listener_t *items;
    size_t count;
} dcc_app_listener_list_t;

typedef struct dcc_app_definition {
    size_t size;
    const char *name;
    const dcc_app_module_t *modules;
    size_t module_count;
    const dcc_app_extension_t *extensions;
    size_t extension_count;
    const dcc_app_typed_slash_command_t *typed_slash_commands;
    size_t typed_slash_command_count;
    const dcc_app_typed_modal_t *typed_modals;
    size_t typed_modal_count;
    const dcc_app_typed_component_t *typed_buttons;
    size_t typed_button_count;
    const dcc_app_typed_component_t *typed_selects;
    size_t typed_select_count;
    const dcc_app_command_sync_options_t *command_sync;
    uint64_t auto_defer_after_ms;
    uint8_t auto_defer_ephemeral;
    uint8_t use_default_error_responses;
    const dcc_app_extension_middleware_t *middlewares;
    size_t middleware_count;
    const dcc_snowflake_t *owner_user_ids;
    size_t owner_user_id_count;
    dcc_permission_t required_permissions;
    uint8_t guild_only;
    uint8_t dm_only;
    uint8_t nsfw_only;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_snowflake_t *required_role_ids;
    size_t required_role_id_count;
    const dcc_snowflake_t *any_role_ids;
    size_t any_role_id_count;
    const dcc_app_extension_message_command_t *message_commands;
    size_t message_command_count;
    const dcc_app_typed_subcommand_t *typed_subcommands;
    size_t typed_subcommand_count;
    const dcc_app_typed_autocomplete_t *typed_autocompletes;
    size_t typed_autocomplete_count;
    const dcc_app_check_t *checks;
    size_t check_count;
    dcc_app_cooldown_options_t cooldown;
    const dcc_app_extension_task_t *tasks;
    size_t task_count;
    const dcc_app_view_t *views;
    size_t view_count;
    const dcc_app_component_session_route_t *component_sessions;
    size_t component_session_count;
    const dcc_app_extension_event_t *events;
    size_t event_count;
    const dcc_app_env_requirement_t *required_env;
    size_t required_env_count;
    dcc_app_error_fn error_handler;
    void *error_user_data;
    const dcc_app_listener_t *listeners;
    size_t listener_count;
} dcc_app_definition_t;

DCC_API dcc_status_t dcc_app_validate_definition_env(const dcc_app_definition_t *definition);
DCC_API dcc_status_t dcc_app_options_from_env(dcc_app_options_t *options, const char *token_env);
DCC_API dcc_status_t dcc_app_create_from_env(const char *token_env, dcc_app_t **out);
DCC_API dcc_command_registry_t *dcc_app_command_registry(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_set_state(
    dcc_app_t *app,
    void *state,
    dcc_app_cleanup_fn cleanup
);
DCC_API void *dcc_app_state(const dcc_app_t *app);
DCC_API void dcc_app_clear_state(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_extension_register(
    dcc_app_t *app,
    const dcc_app_extension_t *extension
);
DCC_API dcc_status_t dcc_app_apply(
    dcc_app_t *app,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_create_defined(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition,
    dcc_app_t **out
);
DCC_API dcc_status_t dcc_app_create_from_env_defined(
    const char *token_env,
    const dcc_app_definition_t *definition,
    dcc_app_t **out
);
DCC_API dcc_status_t dcc_app_run_defined(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_defined_with_signals(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_from_env_defined(
    const char *token_env,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_from_env_defined_with_signals(
    const char *token_env,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_dotenv_defined(
    const char *token_env,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_dotenv_defined_with_signals(
    const char *token_env,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_slash_typed(
    dcc_app_t *app,
    const dcc_app_typed_slash_command_t *command
);
DCC_API dcc_status_t dcc_app_subcommand_typed(
    dcc_app_t *app,
    const dcc_app_typed_subcommand_t *subcommand
);
DCC_API dcc_status_t dcc_app_autocomplete_typed(
    dcc_app_t *app,
    const dcc_app_typed_autocomplete_t *autocomplete
);
DCC_API dcc_status_t dcc_app_modal_typed(
    dcc_app_t *app,
    const dcc_app_typed_modal_t *modal
);
DCC_API dcc_status_t dcc_app_modal_flow_register(
    dcc_app_t *app,
    const dcc_app_modal_flow_t *flow
);
DCC_API dcc_status_t dcc_app_button_typed(
    dcc_app_t *app,
    const dcc_app_typed_component_t *component
);
DCC_API dcc_status_t dcc_app_select_typed(
    dcc_app_t *app,
    const dcc_app_typed_component_t *component
);
DCC_API dcc_status_t dcc_app_run_with_signals(dcc_app_t *app);

DCC_API dcc_status_t dcc_app_auto_defer(dcc_app_t *app, uint64_t after_ms);
DCC_API dcc_status_t dcc_app_auto_defer_ephemeral(dcc_app_t *app, uint64_t after_ms);
DCC_API dcc_status_t dcc_app_disable_auto_defer(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_sync_commands_on_ready(
    dcc_app_t *app,
    const dcc_app_command_sync_options_t *options
);
DCC_API dcc_status_t dcc_app_disable_command_sync_on_ready(dcc_app_t *app);
DCC_API void dcc_app_cooldown_options_init(dcc_app_cooldown_options_t *options);
DCC_API dcc_status_t dcc_app_on(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_app_legacy_event_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_ready(
    dcc_app_t *app,
    dcc_app_legacy_ready_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_ready_once(
    dcc_app_t *app,
    dcc_app_legacy_ready_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_create(
    dcc_app_t *app,
    dcc_app_legacy_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_command(
    dcc_app_t *app,
    const char *prefix,
    const char *name,
    dcc_app_legacy_message_command_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_update(
    dcc_app_t *app,
    dcc_app_legacy_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_delete(
    dcc_app_t *app,
    dcc_app_legacy_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const dcc_thread_params_t *thread,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_thread_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_json(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_infer_guild_id_from_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_app_infer_guild_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_channels(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_channel(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_channel_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_trigger_channel_typing(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_follow_news_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t webhook_channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_channel_voice_status(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *status,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_voice_regions(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_voice_regions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_channel_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_channel_positions_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel_invites(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_channel_invite(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_channel_invite_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_permission_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_roles(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_role_position_t *positions,
    size_t position_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_edit_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_edit_message_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_crosspost_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_pin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_unpin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_own_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_user_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_all_message_reactions(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_all_message_reactions_for_emoji(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_thread_from_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_thread_from_message_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *name,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_archive_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_lock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_unlock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *communication_disabled_until,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_clear_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_move_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_bans(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_bans_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban_seconds(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t delete_message_seconds,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban_params(
    dcc_app_t *app,
    const dcc_guild_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_audit_log(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_audit_log_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t action_type,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint32_t limit,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_guild_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_guild_member_params(
    dcc_app_t *app,
    const dcc_current_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_current_guild_member_nickname(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_current_guild_member_nickname_params(
    dcc_app_t *app,
    const dcc_guild_member_nickname_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_prune_count_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_begin_guild_prune_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_begin_guild_prune_params(
    dcc_app_t *app,
    const dcc_guild_prune_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_integrations(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_sync_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_widget_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_guild_widget_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_vanity_url(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_onboarding_params(
    dcc_app_t *app,
    const dcc_onboarding_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_welcome_screen_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_welcome_screen_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_auto_moderation_rules(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_emojis(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_stickers(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_sticker_multipart(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *file,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_sticker_params(
    dcc_app_t *app,
    const dcc_guild_sticker_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_sticker(
    dcc_app_t *app,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_sticker_packs(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_guild_message_search(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_guild_message_search_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_message_search_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_events(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_events_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_users(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_users_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    uint8_t limit,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_managed_message_publish_latest(
    dcc_app_t *app,
    const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_managed_message_publish_latest_store(
    dcc_app_t *app,
    const char *key,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_managed_message_publish_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_wait_for_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API void dcc_app_wait_filter_init(dcc_app_wait_filter_t *filter);
DCC_API dcc_status_t dcc_app_wait_filter_from_ctx(
    const dcc_ctx_t *ctx,
    dcc_app_wait_filter_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_interaction_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_component(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_component_filtered(
    dcc_app_t *app,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_button(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_select(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_modal(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_message(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_message_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_event_type_t dcc_app_wait_result_type(const dcc_app_wait_result_t *result);
DCC_API const dcc_interaction_t *dcc_app_wait_result_interaction(
    const dcc_app_wait_result_t *result
);
DCC_API const dcc_message_t *dcc_app_wait_result_message(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_user_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_channel_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_guild_id(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_custom_id(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_message_content(
    const dcc_app_wait_result_t *result,
    const char *fallback
);
DCC_API size_t dcc_app_wait_result_value_count(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_value(
    const dcc_app_wait_result_t *result,
    size_t index
);
DCC_API const dcc_interaction_form_field_t *dcc_app_wait_result_form_field(
    const dcc_app_wait_result_t *result,
    const char *custom_id
);
DCC_API const char *dcc_app_wait_result_form_string(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    const char *fallback
);
DCC_API uint8_t dcc_app_wait_result_form_bool(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    uint8_t fallback
);
DCC_API size_t dcc_app_wait_result_form_value_count(
    const dcc_app_wait_result_t *result,
    const char *custom_id
);
DCC_API const char *dcc_app_wait_result_form_value(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    size_t index
);
DCC_API void dcc_app_wait_result_deinit(dcc_app_wait_result_t *result);
DCC_API dcc_status_t dcc_app_use(
    dcc_app_t *app,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_use_guild_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_dm_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_nsfw_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_channel_types(
    dcc_app_t *app,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_use_owner(dcc_app_t *app, dcc_snowflake_t user_id);
DCC_API dcc_status_t dcc_app_use_user_allowlist(
    dcc_app_t *app,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_use_required_permissions(dcc_app_t *app, dcc_permission_t permissions);
DCC_API dcc_status_t dcc_app_use_required_roles(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_use_any_role(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_use_check(
    dcc_app_t *app,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_use_cooldown(
    dcc_app_t *app,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_use(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_use_guild_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_dm_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_nsfw_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_channel_types(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_route_use_owner(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_app_route_use_user_allowlist(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_route_use_required_permissions(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_permission_t permissions
);
DCC_API dcc_status_t dcc_app_route_use_required_roles(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_use_any_role(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_use_check(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_route_use_cooldown(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_apply_policy(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
);
DCC_API dcc_status_t dcc_app_slash(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_slash_route(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_slash_builder(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_slash_builder_route(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_user_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_user_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_message_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_message_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_subcommand(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_subcommand_route(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_autocomplete(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_autocomplete_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_button(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_button_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_button_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_button_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_select(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_select_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_select_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_select_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_modal(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modal_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_modal_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modal_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_component_session(
    dcc_app_t *app,
    const dcc_component_session_t *session,
    const dcc_component_session_listener_options_t *options,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_component_session_store(
    dcc_app_t *app,
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_app_legacy_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_view_register(dcc_app_t *app, const dcc_app_view_t *view);
DCC_API dcc_status_t dcc_app_route_group_init(
    dcc_app_route_group_t *group,
    dcc_app_t *app,
    const char *custom_id_prefix,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_group_button(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_button_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_button_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_button_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_select(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_select_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_select_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_select_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_modal(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_modal_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_modal_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_modal_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_legacy_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_view_register(
    const dcc_app_route_group_t *group,
    const dcc_app_view_t *view
);
DCC_API dcc_status_t dcc_app_route_group_use(
    const dcc_app_route_group_t *group,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_group_use_guild_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_dm_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_nsfw_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_channel_types(
    const dcc_app_route_group_t *group,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_route_group_use_owner(
    const dcc_app_route_group_t *group,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_app_route_group_use_user_allowlist(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_required_permissions(
    const dcc_app_route_group_t *group,
    dcc_permission_t permissions
);
DCC_API dcc_status_t dcc_app_route_group_use_required_roles(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_any_role(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_check(
    const dcc_app_route_group_t *group,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_route_group_use_cooldown(
    const dcc_app_route_group_t *group,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_group_apply_policy(
    const dcc_app_route_group_t *group,
    const dcc_app_route_policy_t *policy
);

DCC_API dcc_status_t dcc_app_every_ms(
    dcc_app_t *app,
    uint64_t interval_ms,
    dcc_app_legacy_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_seconds(
    dcc_app_t *app,
    uint64_t interval_seconds,
    dcc_app_legacy_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_minutes(
    dcc_app_t *app,
    uint64_t interval_minutes,
    dcc_app_legacy_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_hours(
    dcc_app_t *app,
    uint64_t interval_hours,
    dcc_app_legacy_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_kst(
    dcc_app_t *app,
    const char *hhmm,
    dcc_app_legacy_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_day_at_kst(
    dcc_app_t *app,
    uint8_t hour,
    uint8_t minute,
    dcc_app_legacy_task_fn fn,
    void *user_data
);

DCC_API dcc_status_t dcc_app_build_command_plan(
    dcc_app_t *app,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    dcc_command_registry_plan_t *out
);
DCC_API dcc_status_t dcc_app_sync_commands_from_json(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const char *remote_json,
    size_t remote_json_len,
    dcc_command_registry_plan_t *out_plan,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_apply_command_plan(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_app_t *dcc_ctx_app(const dcc_ctx_t *ctx);
DCC_API dcc_client_t *dcc_ctx_client(const dcc_ctx_t *ctx);
DCC_API const dcc_event_t *dcc_ctx_event(const dcc_ctx_t *ctx);
DCC_API const dcc_interaction_t *dcc_ctx_interaction(const dcc_ctx_t *ctx);
DCC_API dcc_interaction_flow_t *dcc_ctx_flow(dcc_ctx_t *ctx);
DCC_API void *dcc_ctx_user_data(const dcc_ctx_t *ctx);
DCC_API void *dcc_ctx_state(const dcc_ctx_t *ctx);
DCC_API dcc_event_type_t dcc_ctx_event_type(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_interaction_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_application_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_guild_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_channel_id(const dcc_ctx_t *ctx);
DCC_API dcc_status_t dcc_ctx_channel_type(const dcc_ctx_t *ctx, uint32_t *out_type);
DCC_API uint8_t dcc_ctx_channel_is_type(const dcc_ctx_t *ctx, uint32_t channel_type);
DCC_API uint8_t dcc_ctx_channel_is_nsfw(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_command_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_message_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_target_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_user_id(const dcc_ctx_t *ctx);
DCC_API const dcc_guild_t *dcc_ctx_guild(const dcc_ctx_t *ctx);
DCC_API const dcc_channel_t *dcc_ctx_channel(const dcc_ctx_t *ctx);
DCC_API const dcc_user_t *dcc_ctx_user(const dcc_ctx_t *ctx);
DCC_API const dcc_member_t *dcc_ctx_member(const dcc_ctx_t *ctx);
DCC_API const dcc_user_t *dcc_ctx_resolved_user(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_member_t *dcc_ctx_resolved_member(const dcc_ctx_t *ctx, dcc_snowflake_t user_id);
DCC_API const dcc_role_t *dcc_ctx_resolved_role(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_channel_t *dcc_ctx_resolved_channel(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_message_t *dcc_ctx_resolved_message(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_attachment_t *dcc_ctx_resolved_attachment(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_user_t *dcc_ctx_target_user(const dcc_ctx_t *ctx);
DCC_API const dcc_member_t *dcc_ctx_target_member(const dcc_ctx_t *ctx);
DCC_API const dcc_role_t *dcc_ctx_target_role(const dcc_ctx_t *ctx);
DCC_API const dcc_channel_t *dcc_ctx_target_channel(const dcc_ctx_t *ctx);
DCC_API const dcc_message_t *dcc_ctx_target_message(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_has_member_permissions(const dcc_ctx_t *ctx);
DCC_API dcc_permission_t dcc_ctx_member_permissions(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_member_has_permissions(const dcc_ctx_t *ctx, dcc_permission_t permissions);
DCC_API uint8_t dcc_ctx_member_has_role(const dcc_ctx_t *ctx, dcc_snowflake_t role_id);
DCC_API uint8_t dcc_ctx_member_has_any_role(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API uint8_t dcc_ctx_member_has_all_roles(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API const dcc_component_session_result_t *dcc_ctx_component_session_result(const dcc_ctx_t *ctx);
DCC_API dcc_component_session_verify_status_t dcc_ctx_component_session_status(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_component_session_action(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_token(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_name(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_subcommand(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_subcommand_group(const dcc_ctx_t *ctx);
DCC_API dcc_status_t dcc_ctx_subcommand_path(
    const dcc_ctx_t *ctx,
    char *buffer,
    size_t buffer_len
);
DCC_API const char *dcc_ctx_custom_id(const dcc_ctx_t *ctx);
DCC_API dcc_interaction_flow_state_t dcc_ctx_response_state(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_response_state_string(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_response_sent(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_deferred(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_replied(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_followed_up(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_response_failed(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_can_followup(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_can_edit_original(const dcc_ctx_t *ctx);

DCC_API dcc_status_t dcc_ctx_reply(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_embed(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t ephemeral,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_error(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_validation_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_validation_result(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const dcc_ctx_validation_error_t *error,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_success(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_defer(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_defer_ephemeral(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_update_message(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_show_modal(
    dcc_ctx_t *ctx,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_show_modal_flow(
    dcc_ctx_t *ctx,
    const dcc_app_modal_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete_matching(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count,
    size_t max_choices,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_edit_original(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_delete_original(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_followup(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_followup_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_followup_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_with_id(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_with_thread(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    const dcc_thread_params_t *thread,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_with_thread_name(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_text_with_id(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_text_with_thread(
    dcc_ctx_t *ctx,
    const char *content,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_webhook(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_webhook_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_params(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_with_token_params(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_text(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_multipart_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_builder_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_multipart_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_application(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_gateway_bot(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_skus(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_entitlements(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_entitlements_page(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids,
    size_t sku_id_count,
    dcc_snowflake_t before_id,
    dcc_snowflake_t after_id,
    uint8_t limit,
    dcc_snowflake_t guild_id,
    uint8_t exclude_ended,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_test_entitlement_params(
    dcc_app_t *app,
    const dcc_test_entitlement_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_consume_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_params(
    dcc_app_t *app,
    const dcc_current_user_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_user(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_connections(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_guilds(
    dcc_app_t *app,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_dms(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_dm_channel(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_dm_channel_params(
    dcc_app_t *app,
    const dcc_dm_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_group_dm_recipient_params(
    dcc_app_t *app,
    const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message_text(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_leave_guild(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_invites(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_invite(
    dcc_app_t *app,
    const char *invite_code,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_invite_full(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_invite(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_stage_instance(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_template(
    dcc_app_t *app,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_from_template(
    dcc_app_t *app,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_from_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_templates(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_sync_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
