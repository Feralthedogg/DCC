#ifndef DCC_APP_LISTENERS_H
#define DCC_APP_LISTENERS_H

#include <dcc/app/base.h>
#include <dcc/application_command.h>
#include <dcc/export.h>
#include <dcc/permissions.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_LISTENER_VERSION 1U
#define DCC_LISTENER_ROUTE_POLICY_VERSION 1U
#define DCC_LISTENER_MIDDLEWARE_VERSION 1U
#define DCC_LISTENER_CHECK_VERSION 1U
#define DCC_LISTENER_COOLDOWN_VERSION 1U
#define DCC_LISTENER_BINDINGS_VERSION 1U
#define DCC_LISTENER_BINDING_VERSION 1U
#define DCC_LISTENER_VALIDATORS_VERSION 1U
#define DCC_LISTENER_VALIDATOR_VERSION 1U
#define DCC_LISTENER_VALIDATION_POLICY_VERSION 1U
#define DCC_LISTENER_TARGET_VERSION 1U

typedef enum dcc_listener_kind {
    DCC_LISTENER_NONE = 0,
    DCC_LISTENER_SLASH,
    DCC_LISTENER_SUBCOMMAND,
    DCC_LISTENER_AUTOCOMPLETE,
    DCC_LISTENER_USER_CONTEXT_MENU,
    DCC_LISTENER_MESSAGE_CONTEXT_MENU,
    DCC_LISTENER_BUTTON,
    DCC_LISTENER_BUTTON_PREFIX,
    DCC_LISTENER_SELECT,
    DCC_LISTENER_SELECT_PREFIX,
    DCC_LISTENER_MODAL,
    DCC_LISTENER_MODAL_PREFIX,
    DCC_LISTENER_EVENT,
    DCC_LISTENER_READY,
    DCC_LISTENER_MESSAGE_CREATE,
    DCC_LISTENER_MESSAGE_UPDATE,
    DCC_LISTENER_MESSAGE_DELETE,
    DCC_LISTENER_MESSAGE_COMMAND,
    DCC_LISTENER_TASK
} dcc_listener_kind_t;

typedef union dcc_listener_handler {
    dcc_app_handler_fn plain;
    dcc_app_typed_handler_fn typed;
    dcc_app_event_fn event;
    dcc_app_ready_fn ready;
    dcc_app_message_fn message;
    dcc_app_message_command_fn message_command;
    dcc_app_task_fn task;
} dcc_listener_handler_t;

/*
 * The listener kind selects the callback signature. For route kinds,
 * args_size == 0 selects plain and args_size > 0 selects typed; the binding
 * kind must then match that route family. C does not expose a union's active
 * member at runtime, so callers must initialize the selected member only.
 */

typedef struct dcc_listener_middleware {
    size_t size;
    uint32_t version;
    dcc_app_middleware_fn callback;
    void *user_data;
} dcc_listener_middleware_t;

typedef struct dcc_listener_check {
    size_t size;
    uint32_t version;
    dcc_app_check_fn callback;
    void *user_data;
    const char *title;
    const char *description;
} dcc_listener_check_t;

typedef enum dcc_listener_cooldown_bucket {
    DCC_LISTENER_COOLDOWN_NONE = 0,
    DCC_LISTENER_COOLDOWN_GLOBAL,
    DCC_LISTENER_COOLDOWN_USER,
    DCC_LISTENER_COOLDOWN_CHANNEL,
    DCC_LISTENER_COOLDOWN_GUILD
} dcc_listener_cooldown_bucket_t;

typedef struct dcc_listener_cooldown {
    size_t size;
    uint32_t version;
    dcc_listener_cooldown_bucket_t bucket;
    uint32_t limit;
    uint64_t window_ms;
} dcc_listener_cooldown_t;

typedef struct dcc_listener_route_policy {
    size_t size;
    uint32_t version;
    const dcc_listener_middleware_t *middlewares;
    size_t middleware_count;
    const dcc_snowflake_t *owner_user_ids;
    size_t owner_user_id_count;
    dcc_permission_t required_permissions;
    uint8_t guild_only;
    dcc_listener_cooldown_t cooldown;
    const dcc_listener_check_t *checks;
    size_t check_count;
    uint8_t dm_only;
    uint8_t nsfw_only;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_snowflake_t *required_role_ids;
    size_t required_role_id_count;
    const dcc_snowflake_t *any_role_ids;
    size_t any_role_id_count;
} dcc_listener_route_policy_t;

typedef enum dcc_listener_binding_kind {
    DCC_LISTENER_BIND_NONE = 0,
    DCC_LISTENER_BIND_OPTIONS,
    DCC_LISTENER_BIND_FORM,
    DCC_LISTENER_BIND_COMPONENT
} dcc_listener_binding_kind_t;

typedef struct dcc_listener_binding {
    size_t size;
    uint32_t version;
    const char *name;
    uint32_t type;
    size_t field_offset;
    size_t count_offset;
    uint8_t has_count_offset;
    uint8_t required;
    const char *custom_id_prefix;
    const char *fallback_string;
    int64_t fallback_integer;
    double fallback_number;
    uint8_t fallback_boolean;
    dcc_snowflake_t fallback_snowflake;
    const char **fallback_values;
    size_t fallback_values_count;
    uint32_t fallback_component_type;
} dcc_listener_binding_t;

typedef union dcc_listener_binding_items {
    const dcc_listener_binding_t *options;
    const dcc_listener_binding_t *forms;
    const dcc_listener_binding_t *components;
} dcc_listener_binding_items_t;

typedef struct dcc_listener_bindings {
    size_t size;
    uint32_t version;
    dcc_listener_binding_kind_t kind;
    dcc_listener_binding_items_t items;
    size_t count;
} dcc_listener_bindings_t;

typedef enum dcc_listener_validator_kind {
    DCC_LISTENER_VALIDATE_STRING_REQUIRED = 1,
    DCC_LISTENER_VALIDATE_STRING_LENGTH,
    DCC_LISTENER_VALIDATE_INTEGER_RANGE,
    DCC_LISTENER_VALIDATE_NUMBER_RANGE,
    DCC_LISTENER_VALIDATE_DATE_YYYYMMDD,
    DCC_LISTENER_VALIDATE_DATE_MMDD,
    DCC_LISTENER_VALIDATE_DATE_MMDD_OR_YYYYMMDD
} dcc_listener_validator_kind_t;

typedef struct dcc_listener_validator {
    size_t size;
    uint32_t version;
    const char *field;
    dcc_listener_validator_kind_t kind;
    size_t field_offset;
    size_t min_length;
    size_t max_length;
    int64_t min_integer;
    int64_t max_integer;
    double min_number;
    double max_number;
} dcc_listener_validator_t;

typedef struct dcc_listener_validators {
    size_t size;
    uint32_t version;
    const dcc_listener_validator_t *items;
    size_t count;
} dcc_listener_validators_t;

typedef struct dcc_listener_validation_policy {
    size_t size;
    uint32_t version;
    uint8_t suppress_response;
} dcc_listener_validation_policy_t;

typedef struct dcc_listener_route_target {
    size_t size;
    uint32_t version;
    const char *name;
    const char *description;
    const char *command_name;
    const char *subcommand_path;
    const dcc_application_command_builder_t *command;
} dcc_listener_route_target_t;

typedef struct dcc_listener_event_target {
    size_t size;
    uint32_t version;
    dcc_event_type_t type;
    uint8_t once;
} dcc_listener_event_target_t;

typedef struct dcc_listener_message_command_target {
    size_t size;
    uint32_t version;
    const char *prefix;
    const char *name;
} dcc_listener_message_command_target_t;

typedef enum dcc_listener_schedule_kind {
    DCC_LISTENER_SCHEDULE_INTERVAL = 1,
    DCC_LISTENER_SCHEDULE_DAILY_KST
} dcc_listener_schedule_kind_t;

typedef struct dcc_listener_schedule_target {
    size_t size;
    uint32_t version;
    dcc_listener_schedule_kind_t kind;
    uint64_t interval_ms;
    uint8_t hour;
    uint8_t minute;
} dcc_listener_schedule_target_t;

typedef union dcc_listener_target {
    dcc_listener_route_target_t route;
    dcc_listener_event_target_t event;
    dcc_listener_message_command_target_t message_command;
    dcc_listener_schedule_target_t schedule;
} dcc_listener_target_t;

typedef struct dcc_listener {
    size_t size;
    uint32_t version;
    dcc_listener_kind_t kind;
    dcc_listener_handler_t handler;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
    dcc_listener_route_policy_t policy;
    size_t args_size;
    dcc_listener_bindings_t bindings;
    dcc_listener_validators_t validators;
    dcc_listener_validation_policy_t validation;
    dcc_listener_target_t target;
} dcc_listener_t;

/** Initializes a listener and all nested values for version 1. */
DCC_API void dcc_listener_init(dcc_listener_t *listener, dcc_listener_kind_t kind);

/** Registers one validated listener and optionally returns its owned ID. */
DCC_API dcc_status_t dcc_app_listen(
    dcc_app_t *app,
    const dcc_listener_t *listener,
    dcc_listener_id_t *out_id
);

/** Removes a listener, prevents later dispatch, and runs cleanup once. */
DCC_API dcc_status_t dcc_app_unlisten(dcc_app_t *app, dcc_listener_id_t id);

#ifdef __cplusplus
}
#endif

#endif
