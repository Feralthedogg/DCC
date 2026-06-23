/* Included by <dcc/app.h>. */
typedef struct dcc_app dcc_app_t;
typedef struct dcc_ctx dcc_ctx_t;
typedef uint64_t dcc_app_route_id_t;

#define DCC_APP_ROUTE_INVALID UINT64_C(0)

typedef void (*dcc_app_handler_fn)(dcc_ctx_t *ctx, void *user_data);
typedef void (*dcc_app_typed_handler_fn)(dcc_ctx_t *ctx, void *args, void *user_data);
typedef dcc_status_t (*dcc_app_middleware_fn)(dcc_ctx_t *ctx, void *user_data);
typedef dcc_status_t (*dcc_app_check_fn)(dcc_ctx_t *ctx, void *user_data);
typedef void (*dcc_app_error_fn)(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message,
    void *user_data
);
typedef void (*dcc_app_event_fn)(dcc_app_t *app, const dcc_event_t *event, void *user_data);
typedef void (*dcc_app_ready_fn)(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data);
typedef void (*dcc_app_message_fn)(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
);
typedef void (*dcc_app_message_command_fn)(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
);
typedef void (*dcc_app_infer_guild_cb)(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t guild_id,
    dcc_status_t status,
    void *user_data
);
typedef void (*dcc_app_task_fn)(dcc_app_t *app, void *user_data);
typedef void (*dcc_app_cleanup_fn)(void *user_data);
typedef dcc_status_t (*dcc_app_module_setup_fn)(dcc_app_t *app, void *user_data);

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
    dcc_app_handler_fn handler;
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
    dcc_app_handler_fn handler;
    void *user_data;
} dcc_app_component_session_route_t;

typedef struct dcc_app_route_group {
    size_t size;
    dcc_app_t *app;
    const char *custom_id_prefix;
    void *user_data;
} dcc_app_route_group_t;

typedef struct dcc_app_wait_result {
    size_t size;
    dcc_event_snapshot_t snapshot;
    dcc_interaction_t *interaction;
    dcc_message_t *message;
} dcc_app_wait_result_t;

typedef struct dcc_app_wait_filter {
    size_t size;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    const char *custom_id;
    const char *custom_id_prefix;
} dcc_app_wait_filter_t;

typedef enum dcc_ctx_option_bind_type {
    DCC_CTX_OPTION_BIND_STRING = 1,
    DCC_CTX_OPTION_BIND_INTEGER,
    DCC_CTX_OPTION_BIND_NUMBER,
    DCC_CTX_OPTION_BIND_BOOLEAN,
    DCC_CTX_OPTION_BIND_SNOWFLAKE,
    DCC_CTX_OPTION_BIND_USER,
    DCC_CTX_OPTION_BIND_MEMBER,
    DCC_CTX_OPTION_BIND_ROLE,
    DCC_CTX_OPTION_BIND_CHANNEL,
    DCC_CTX_OPTION_BIND_MESSAGE,
    DCC_CTX_OPTION_BIND_ATTACHMENT
} dcc_ctx_option_bind_type_t;

typedef struct dcc_ctx_option_binding {
    size_t size;
    const char *name;
    dcc_ctx_option_bind_type_t type;
    void *out;
    uint8_t required;
    const char *fallback_string;
    int64_t fallback_integer;
    double fallback_number;
    uint8_t fallback_boolean;
    dcc_snowflake_t fallback_snowflake;
} dcc_ctx_option_binding_t;

typedef struct dcc_ctx_option_field_binding {
    size_t size;
    const char *name;
    dcc_ctx_option_bind_type_t type;
    size_t field_offset;
    uint8_t required;
    const char *fallback_string;
    int64_t fallback_integer;
    double fallback_number;
    uint8_t fallback_boolean;
    dcc_snowflake_t fallback_snowflake;
} dcc_ctx_option_field_binding_t;

typedef struct dcc_ctx_option_field_binding_list {
    const dcc_ctx_option_field_binding_t *items;
    size_t count;
} dcc_ctx_option_field_binding_list_t;

typedef enum dcc_ctx_form_bind_type {
    DCC_CTX_FORM_BIND_STRING = 1,
    DCC_CTX_FORM_BIND_BOOLEAN,
    DCC_CTX_FORM_BIND_VALUES
} dcc_ctx_form_bind_type_t;

typedef struct dcc_ctx_form_binding {
    size_t size;
    const char *custom_id;
    dcc_ctx_form_bind_type_t type;
    void *out;
    size_t *out_count;
    uint8_t required;
    const char *fallback_string;
    uint8_t fallback_boolean;
    const char **fallback_values;
    size_t fallback_values_count;
} dcc_ctx_form_binding_t;

typedef struct dcc_ctx_form_field_binding {
    size_t size;
    const char *custom_id;
    dcc_ctx_form_bind_type_t type;
    size_t field_offset;
    size_t count_offset;
    uint8_t has_count_offset;
    uint8_t required;
    const char *fallback_string;
    uint8_t fallback_boolean;
    const char **fallback_values;
    size_t fallback_values_count;
} dcc_ctx_form_field_binding_t;

typedef struct dcc_ctx_form_field_binding_list {
    const dcc_ctx_form_field_binding_t *items;
    size_t count;
} dcc_ctx_form_field_binding_list_t;

typedef enum dcc_ctx_component_bind_type {
    DCC_CTX_COMPONENT_BIND_CUSTOM_ID = 1,
    DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX,
    DCC_CTX_COMPONENT_BIND_VALUES,
    DCC_CTX_COMPONENT_BIND_FIRST_VALUE,
    DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE
} dcc_ctx_component_bind_type_t;

typedef struct dcc_ctx_component_binding {
    size_t size;
    dcc_ctx_component_bind_type_t type;
    const char *custom_id_prefix;
    void *out;
    size_t *out_count;
    uint8_t required;
    const char *fallback_string;
    const char **fallback_values;
    size_t fallback_values_count;
    uint32_t fallback_component_type;
} dcc_ctx_component_binding_t;

typedef struct dcc_ctx_component_field_binding {
    size_t size;
    dcc_ctx_component_bind_type_t type;
    const char *custom_id_prefix;
    size_t field_offset;
    size_t count_offset;
    uint8_t has_count_offset;
    uint8_t required;
    const char *fallback_string;
    const char **fallback_values;
    size_t fallback_values_count;
    uint32_t fallback_component_type;
} dcc_ctx_component_field_binding_t;

typedef struct dcc_ctx_component_field_binding_list {
    const dcc_ctx_component_field_binding_t *items;
    size_t count;
} dcc_ctx_component_field_binding_list_t;

typedef enum dcc_ctx_field_validate_type {
    DCC_CTX_FIELD_VALIDATE_STRING_REQUIRED = 1,
    DCC_CTX_FIELD_VALIDATE_STRING_LENGTH,
    DCC_CTX_FIELD_VALIDATE_INTEGER_RANGE,
    DCC_CTX_FIELD_VALIDATE_NUMBER_RANGE,
    DCC_CTX_FIELD_VALIDATE_DATE_YYYYMMDD,
    DCC_CTX_FIELD_VALIDATE_DATE_MMDD,
    DCC_CTX_FIELD_VALIDATE_DATE_MMDD_OR_YYYYMMDD
} dcc_ctx_field_validate_type_t;

typedef struct dcc_ctx_field_validator {
    size_t size;
    const char *field;
    dcc_ctx_field_validate_type_t type;
    size_t field_offset;
    size_t min_length;
    size_t max_length;
    int64_t min_integer;
    int64_t max_integer;
    double min_number;
    double max_number;
} dcc_ctx_field_validator_t;

typedef struct dcc_ctx_validation_error {
    size_t size;
    const char *field;
    const char *message;
    dcc_status_t status;
} dcc_ctx_validation_error_t;

typedef struct dcc_ctx_parsed_date {
    size_t size;
    int year;
    uint8_t month;
    uint8_t day;
    uint8_t has_year;
} dcc_ctx_parsed_date_t;

typedef struct dcc_ctx_field_validator_list {
    const dcc_ctx_field_validator_t *items;
    size_t count;
} dcc_ctx_field_validator_list_t;

typedef struct dcc_app_command_sync_options {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_command_registry_options_t command_registry;
    uint8_t apply;
    uint8_t once;
    uint8_t infer_application_id;
} dcc_app_command_sync_options_t;

typedef struct dcc_app_module {
    size_t size;
    const char *name;
    dcc_app_module_setup_fn setup;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
} dcc_app_module_t;

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
    dcc_app_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_slash_command_t;

typedef struct dcc_app_extension_subcommand {
    size_t size;
    const char *command_name;
    const char *subcommand_path;
    dcc_app_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_subcommand_t;

typedef struct dcc_app_extension_autocomplete {
    size_t size;
    const char *name;
    dcc_app_handler_fn handler;
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
    dcc_app_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_context_menu_t;

typedef struct dcc_app_extension_component_route {
    size_t size;
    dcc_app_extension_component_route_type_t type;
    const char *custom_id;
    dcc_app_handler_fn handler;
    void *user_data;
    dcc_app_route_id_t *out_route;
    dcc_app_route_policy_t policy;
} dcc_app_extension_component_route_t;

typedef struct dcc_app_extension_event {
    size_t size;
    dcc_app_extension_event_type_t type;
    dcc_event_type_t event_type;
    dcc_app_event_fn event_handler;
    dcc_app_ready_fn ready_handler;
    dcc_app_message_fn message_handler;
    void *user_data;
    uint8_t once;
} dcc_app_extension_event_t;

typedef struct dcc_app_extension_message_command {
    size_t size;
    const char *prefix;
    const char *name;
    dcc_app_message_command_fn handler;
    void *user_data;
} dcc_app_extension_message_command_t;

typedef struct dcc_app_extension_task {
    size_t size;
    dcc_app_extension_task_type_t type;
    uint64_t interval;
    const char *hhmm;
    uint8_t hour;
    uint8_t minute;
    dcc_app_task_fn handler;
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
    dcc_app_typed_handler_fn handler;
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
    dcc_app_typed_handler_fn handler;
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
    dcc_app_typed_handler_fn handler;
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
    dcc_app_typed_handler_fn handler;
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
    dcc_app_typed_handler_fn handler;
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

typedef struct dcc_app_options {
    size_t size;
    dcc_client_options_t client;
    dcc_command_registry_options_t command_registry;
    uint64_t auto_defer_after_ms;
    uint8_t auto_defer_ephemeral;
    dcc_app_command_sync_options_t command_sync;
    uint8_t command_sync_on_ready;
    const char *store_file_path;
} dcc_app_options_t;

DCC_API void dcc_app_command_sync_options_init(dcc_app_command_sync_options_t *options);
DCC_API void dcc_app_options_init(dcc_app_options_t *options);
DCC_API dcc_status_t dcc_app_validate_definition_env(const dcc_app_definition_t *definition);
DCC_API dcc_status_t dcc_app_options_from_env(dcc_app_options_t *options, const char *token_env);
DCC_API dcc_status_t dcc_app_create(const dcc_app_options_t *options, dcc_app_t **out);
DCC_API dcc_status_t dcc_app_create_from_env(const char *token_env, dcc_app_t **out);
DCC_API void dcc_app_destroy(dcc_app_t *app);

DCC_API dcc_client_t *dcc_app_client(dcc_app_t *app);
DCC_API dcc_command_registry_t *dcc_app_command_registry(dcc_app_t *app);
DCC_API dcc_store_t *dcc_app_store(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_store_open_file(dcc_app_t *app, const char *path);
DCC_API void dcc_app_store_close(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_set_state(
    dcc_app_t *app,
    void *state,
    dcc_app_cleanup_fn cleanup
);
DCC_API void *dcc_app_state(const dcc_app_t *app);
DCC_API void dcc_app_clear_state(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_module_register(dcc_app_t *app, const dcc_app_module_t *module);
DCC_API size_t dcc_app_module_count(const dcc_app_t *app);
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
DCC_API dcc_status_t dcc_app_run_from_env_defined(
    const char *token_env,
    const dcc_app_definition_t *definition
);
DCC_API dcc_status_t dcc_app_run_dotenv_defined(
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
