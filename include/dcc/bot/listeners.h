#ifndef DCC_BOT_LISTENERS_H
#define DCC_BOT_LISTENERS_H

#include <dcc/app/listeners.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_listener_config {
    size_t size;
    uint32_t version;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
    const dcc_application_command_builder_t *command;
    const dcc_listener_route_policy_t *policy;
    size_t args_size;
    const dcc_listener_bindings_t *bindings;
    const dcc_listener_validators_t *validators;
    const dcc_listener_validation_policy_t *validation;
    uint8_t once;
} dcc_listener_config_t;

enum { DCC_LISTENER_CONFIG_VERSION = 1U };

/** Initializes an empty borrowed listener configuration. */
#define DCC_LISTENER_CONFIG_INIT \
    {sizeof(dcc_listener_config_t), DCC_LISTENER_CONFIG_VERSION, NULL, NULL, \
     NULL, NULL, 0U, NULL, NULL, NULL, 0U}

/** Returns an empty listener configuration by value. */
static inline dcc_listener_config_t DCC_LISTENER_CONFIG(void) {
    dcc_listener_config_t value = DCC_LISTENER_CONFIG_INIT;
    return value;
}

/** Returns an empty configuration borrowing `policy` through registration. */
static inline dcc_listener_config_t DCC_LISTENER_CONFIG_WITH_POLICY(
    const dcc_listener_route_policy_t *policy
) {
    dcc_listener_config_t value = DCC_LISTENER_CONFIG();
    value.policy = policy;
    return value;
}

/** Wraps a plain route handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_PLAIN_HANDLER(
    dcc_app_handler_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.plain = handler;
    return value;
}

/** Wraps a typed route handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_TYPED_HANDLER(
    dcc_app_typed_handler_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.typed = handler;
    return value;
}

/** Wraps a generic event handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_EVENT_HANDLER(
    dcc_app_event_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.event = handler;
    return value;
}

/** Wraps a READY handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_READY_HANDLER(
    dcc_app_ready_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.ready = handler;
    return value;
}

/** Wraps a message event handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_MESSAGE_HANDLER(
    dcc_app_message_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.message = handler;
    return value;
}

/** Wraps a message-command handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_MESSAGE_COMMAND_HANDLER(
    dcc_app_message_command_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.message_command = handler;
    return value;
}

/** Wraps a scheduled task handler without casting it. */
static inline dcc_listener_handler_t DCC_LISTENER_TASK_HANDLER(
    dcc_app_task_fn handler
) {
    dcc_listener_handler_t value = {0};
    value.task = handler;
    return value;
}

/** Returns a versioned interval schedule value. */
static inline dcc_listener_schedule_target_t DCC_LISTENER_INTERVAL(
    uint64_t interval_ms
) {
    dcc_listener_schedule_target_t value = {
        sizeof(value), DCC_LISTENER_TARGET_VERSION,
        DCC_LISTENER_SCHEDULE_INTERVAL, interval_ms, 0U, 0U};
    return value;
}

/** Returns a versioned daily Korea-time schedule value. */
static inline dcc_listener_schedule_target_t DCC_LISTENER_DAILY_KST(
    uint8_t hour,
    uint8_t minute
) {
    dcc_listener_schedule_target_t value = {
        sizeof(value), DCC_LISTENER_TARGET_VERSION,
        DCC_LISTENER_SCHEDULE_DAILY_KST, 0U, hour, minute};
    return value;
}

static inline dcc_listener_t dcc_bot_listener_base(
    dcc_listener_kind_t kind,
    dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    dcc_listener_t value;
    dcc_listener_init(&value, kind);
    if (config.size < sizeof(config) ||
        config.version != DCC_LISTENER_CONFIG_VERSION ||
        (config.once != 0U &&
         !(kind == DCC_LISTENER_EVENT || kind == DCC_LISTENER_READY ||
           (kind >= DCC_LISTENER_MESSAGE_CREATE &&
            kind <= DCC_LISTENER_MESSAGE_DELETE)))) {
        value.size = 0U;
        return value;
    }
    value.handler = handler;
    value.user_data = config.user_data;
    value.cleanup = config.cleanup;
    value.policy = config.policy;
    value.args_size = config.args_size;
    value.bindings = config.bindings;
    value.validators = config.validators;
    value.validation = config.validation;
    if (kind == DCC_LISTENER_EVENT || kind == DCC_LISTENER_READY ||
        (kind >= DCC_LISTENER_MESSAGE_CREATE &&
         kind <= DCC_LISTENER_MESSAGE_DELETE)) {
        value.target.event.once = config.once;
    }
    return value;
}

static inline dcc_listener_t dcc_bot_route_listener(
    dcc_listener_kind_t kind,
    const char *name,
    const char *description,
    const char *command_name,
    const char *subcommand_path,
    dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    dcc_listener_t value = dcc_bot_listener_base(kind, handler, config);
    if (config.command != NULL) {
        value.target.route.command = config.command;
        value.target.route.command_name = command_name;
        value.target.route.subcommand_path = subcommand_path;
    } else {
        value.target.route.name = name;
        value.target.route.description = description;
        value.target.route.command_name = command_name;
        value.target.route.subcommand_path = subcommand_path;
    }
    return value;
}

/** Builds a slash listener borrowing all metadata until registration returns. */
static inline dcc_listener_t DCC_LISTEN_SLASH_WITH(
    const char *name, const char *description,
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_SLASH, name, description,
                                  NULL, NULL, handler, config);
}

/** Builds a default-config slash listener. */
static inline dcc_listener_t DCC_LISTEN_SLASH(
    const char *name, const char *description, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_SLASH_WITH(name, description,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a subcommand listener with an explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_SUBCOMMAND_WITH(
    const char *command_name, const char *subcommand_path,
    const char *description, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_SUBCOMMAND, NULL, description,
        command_name, subcommand_path, handler, config);
}

/** Builds a default-config subcommand listener. */
static inline dcc_listener_t DCC_LISTEN_SUBCOMMAND(
    const char *command_name, const char *subcommand_path,
    const char *description, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_SUBCOMMAND_WITH(command_name, subcommand_path,
        description, DCC_LISTENER_PLAIN_HANDLER(handler),
        DCC_LISTENER_CONFIG());
}

/** Builds an autocomplete listener with an explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_AUTOCOMPLETE_WITH(
    const char *command_name, const char *description,
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_AUTOCOMPLETE, command_name,
        description, NULL, NULL, handler, config);
}

/** Builds a default-config autocomplete listener. */
static inline dcc_listener_t DCC_LISTEN_AUTOCOMPLETE(
    const char *command_name, const char *description,
    dcc_app_handler_fn handler
) {
    return DCC_LISTEN_AUTOCOMPLETE_WITH(command_name, description,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds subcommand autocomplete with an explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_WITH(
    const char *command_name, const char *subcommand_path,
    const char *description, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_AUTOCOMPLETE, NULL, description,
        command_name, subcommand_path, handler, config);
}

/** Builds default-config subcommand autocomplete. */
static inline dcc_listener_t DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE(
    const char *command_name, const char *subcommand_path,
    const char *description, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_WITH(command_name,
        subcommand_path, description, DCC_LISTENER_PLAIN_HANDLER(handler),
        DCC_LISTENER_CONFIG());
}

/** Builds a user context-menu listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_USER_CONTEXT_MENU_WITH(
    const char *name, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_USER_CONTEXT_MENU, name, NULL,
                                  NULL, NULL, handler, config);
}

/** Builds a default-config user context-menu listener. */
static inline dcc_listener_t DCC_LISTEN_USER_CONTEXT_MENU(
    const char *name, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_USER_CONTEXT_MENU_WITH(name,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a message context-menu listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_CONTEXT_MENU_WITH(
    const char *name, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_route_listener(DCC_LISTENER_MESSAGE_CONTEXT_MENU, name,
                                  NULL, NULL, NULL, handler, config);
}

/** Builds a default-config message context-menu listener. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_CONTEXT_MENU(
    const char *name, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_MESSAGE_CONTEXT_MENU_WITH(name,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

static inline dcc_listener_t dcc_bot_component_listener(
    dcc_listener_kind_t kind, const char *custom_id,
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_route_listener(kind, custom_id, NULL, NULL, NULL,
                                  handler, config);
}

/** Builds an exact button listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_BUTTON_WITH(
    const char *custom_id, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_BUTTON, custom_id,
                                      handler, config);
}

/** Builds a default-config exact button listener. */
static inline dcc_listener_t DCC_LISTEN_BUTTON(
    const char *custom_id, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_BUTTON_WITH(custom_id,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a button-prefix listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_BUTTON_PREFIX_WITH(
    const char *custom_id_prefix, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_BUTTON_PREFIX,
                                      custom_id_prefix, handler, config);
}

/** Builds a default-config button-prefix listener. */
static inline dcc_listener_t DCC_LISTEN_BUTTON_PREFIX(
    const char *custom_id_prefix, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_BUTTON_PREFIX_WITH(custom_id_prefix,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds an exact select listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_SELECT_WITH(
    const char *custom_id, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_SELECT, custom_id,
                                      handler, config);
}

/** Builds a default-config exact select listener. */
static inline dcc_listener_t DCC_LISTEN_SELECT(
    const char *custom_id, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_SELECT_WITH(custom_id,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a select-prefix listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_SELECT_PREFIX_WITH(
    const char *custom_id_prefix, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_SELECT_PREFIX,
                                      custom_id_prefix, handler, config);
}

/** Builds a default-config select-prefix listener. */
static inline dcc_listener_t DCC_LISTEN_SELECT_PREFIX(
    const char *custom_id_prefix, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_SELECT_PREFIX_WITH(custom_id_prefix,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds an exact modal listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MODAL_WITH(
    const char *custom_id, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_MODAL, custom_id,
                                      handler, config);
}

/** Builds a default-config exact modal listener. */
static inline dcc_listener_t DCC_LISTEN_MODAL(
    const char *custom_id, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_MODAL_WITH(custom_id,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a modal-prefix listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MODAL_PREFIX_WITH(
    const char *custom_id_prefix, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_component_listener(DCC_LISTENER_MODAL_PREFIX,
                                      custom_id_prefix, handler, config);
}

/** Builds a default-config modal-prefix listener. */
static inline dcc_listener_t DCC_LISTEN_MODAL_PREFIX(
    const char *custom_id_prefix, dcc_app_handler_fn handler
) {
    return DCC_LISTEN_MODAL_PREFIX_WITH(custom_id_prefix,
        DCC_LISTENER_PLAIN_HANDLER(handler), DCC_LISTENER_CONFIG());
}

static inline dcc_listener_t dcc_bot_event_listener(
    dcc_listener_kind_t kind, dcc_event_type_t type,
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    dcc_listener_t value = dcc_bot_listener_base(kind, handler, config);
    value.target.event.type = type;
    return value;
}

/** Builds a generic event listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_EVENT_WITH(
    dcc_event_type_t type, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    return dcc_bot_event_listener(DCC_LISTENER_EVENT, type, handler, config);
}

/** Builds a default-config generic event listener. */
static inline dcc_listener_t DCC_LISTEN_EVENT(
    dcc_event_type_t type, dcc_app_event_fn handler
) {
    return DCC_LISTEN_EVENT_WITH(type, DCC_LISTENER_EVENT_HANDLER(handler),
                                 DCC_LISTENER_CONFIG());
}

/** Builds a READY listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_READY_WITH(
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_event_listener(DCC_LISTENER_READY, DCC_EVENT_READY,
                                  handler, config);
}

/** Builds a default-config READY listener. */
static inline dcc_listener_t DCC_LISTEN_READY(dcc_app_ready_fn handler) {
    return DCC_LISTEN_READY_WITH(DCC_LISTENER_READY_HANDLER(handler),
                                 DCC_LISTENER_CONFIG());
}

/** Builds a message-create listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_CREATE_WITH(
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_event_listener(DCC_LISTENER_MESSAGE_CREATE,
                                  DCC_EVENT_MESSAGE_CREATE, handler, config);
}

/** Builds a default-config message-create listener. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_CREATE(
    dcc_app_message_fn handler
) {
    return DCC_LISTEN_MESSAGE_CREATE_WITH(DCC_LISTENER_MESSAGE_HANDLER(handler),
                                          DCC_LISTENER_CONFIG());
}

/** Builds a message-update listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_UPDATE_WITH(
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_event_listener(DCC_LISTENER_MESSAGE_UPDATE,
                                  DCC_EVENT_MESSAGE_UPDATE, handler, config);
}

/** Builds a default-config message-update listener. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_UPDATE(
    dcc_app_message_fn handler
) {
    return DCC_LISTEN_MESSAGE_UPDATE_WITH(DCC_LISTENER_MESSAGE_HANDLER(handler),
                                          DCC_LISTENER_CONFIG());
}

/** Builds a message-delete listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_DELETE_WITH(
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    return dcc_bot_event_listener(DCC_LISTENER_MESSAGE_DELETE,
                                  DCC_EVENT_MESSAGE_DELETE, handler, config);
}

/** Builds a default-config message-delete listener. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_DELETE(
    dcc_app_message_fn handler
) {
    return DCC_LISTEN_MESSAGE_DELETE_WITH(DCC_LISTENER_MESSAGE_HANDLER(handler),
                                          DCC_LISTENER_CONFIG());
}

/** Builds a message-command listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_COMMAND_WITH(
    const char *prefix, const char *name, dcc_listener_handler_t handler,
    dcc_listener_config_t config
) {
    dcc_listener_t value = dcc_bot_listener_base(
        DCC_LISTENER_MESSAGE_COMMAND, handler, config);
    value.target.message_command.prefix = prefix;
    value.target.message_command.name = name;
    return value;
}

/** Builds a default-config message-command listener. */
static inline dcc_listener_t DCC_LISTEN_MESSAGE_COMMAND(
    const char *prefix, const char *name, dcc_app_message_command_fn handler
) {
    return DCC_LISTEN_MESSAGE_COMMAND_WITH(prefix, name,
        DCC_LISTENER_MESSAGE_COMMAND_HANDLER(handler), DCC_LISTENER_CONFIG());
}

/** Builds a task listener with explicit configuration. */
static inline dcc_listener_t DCC_LISTEN_TASK_WITH(
    dcc_listener_schedule_target_t schedule,
    dcc_listener_handler_t handler, dcc_listener_config_t config
) {
    dcc_listener_t value = dcc_bot_listener_base(DCC_LISTENER_TASK,
                                                  handler, config);
    value.target.schedule = schedule;
    return value;
}

/** Builds a default-config task listener. */
static inline dcc_listener_t DCC_LISTEN_TASK(
    dcc_listener_schedule_target_t schedule, dcc_app_task_fn handler
) {
    return DCC_LISTEN_TASK_WITH(schedule, DCC_LISTENER_TASK_HANDLER(handler),
                                DCC_LISTENER_CONFIG());
}

#ifdef __cplusplus
}
#endif

#endif
