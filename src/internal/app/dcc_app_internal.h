#ifndef DCC_APP_INTERNAL_H
#define DCC_APP_INTERNAL_H

#include <dcc/app.h>
#include <dcc/tasks.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_app_schedule_kind {
    DCC_APP_SCHEDULE_INTERVAL = 0,
    DCC_APP_SCHEDULE_DAILY_KST
} dcc_app_schedule_kind_t;

typedef enum dcc_app_response_state {
    DCC_APP_RESPONSE_READY = 0,
    DCC_APP_RESPONSE_DEFERRED = 1,
    DCC_APP_RESPONSE_REPLIED = 2,
    DCC_APP_RESPONSE_FAILED = 3
} dcc_app_response_state_t;

typedef struct dcc_app_auto_defer {
    dcc_client_t *client;
    dcc_interaction_t interaction;
    char *token;
    atomic_uint refs;
    atomic_bool done;
    atomic_int response_state;
    uint64_t after_ms;
    uint8_t ephemeral;
} dcc_app_auto_defer_t;

typedef struct dcc_app_middleware {
    dcc_app_middleware_fn fn;
    void *user_data;
    void (*cleanup)(void *user_data);
} dcc_app_middleware_t;

typedef struct dcc_app_route {
    dcc_app_route_id_t id;
    dcc_event_type_t type;
    char *key;
    uint8_t prefix;
    dcc_app_handler_fn handler;
    void *user_data;
    void (*user_data_cleanup)(void *user_data);
    dcc_app_middleware_t *middlewares;
    size_t middleware_count;
    size_t middleware_cap;
} dcc_app_route_t;

typedef struct dcc_app_schedule {
    dcc_app_t *app;
    dcc_app_schedule_kind_t kind;
    uint64_t interval_ms;
    uint8_t hour;
    uint8_t minute;
    dcc_app_task_fn fn;
    void *user_data;
} dcc_app_schedule_t;

typedef struct dcc_app_component_session_listener_entry {
    dcc_component_session_listener_t listener;
    void *state;
} dcc_app_component_session_listener_entry_t;

typedef struct dcc_app_event_listener_entry {
    dcc_event_type_t type;
    dcc_listener_id_t id;
    void *state;
} dcc_app_event_listener_entry_t;

typedef struct dcc_app_module_entry {
    const char *name;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
} dcc_app_module_entry_t;

struct dcc_app {
    dcc_client_t *client;
    dcc_command_registry_t registry;
    dcc_store_t store;
    dcc_command_registry_options_t command_registry_options;
    dcc_app_route_t *routes;
    size_t route_count;
    size_t route_cap;
    dcc_app_middleware_t *middlewares;
    size_t middleware_count;
    size_t middleware_cap;
    dcc_app_schedule_t **schedules;
    size_t schedule_count;
    size_t schedule_cap;
    dcc_app_component_session_listener_entry_t *component_session_listeners;
    size_t component_session_listener_count;
    size_t component_session_listener_cap;
    dcc_app_event_listener_entry_t *event_listeners;
    size_t event_listener_count;
    size_t event_listener_cap;
    dcc_app_module_entry_t *modules;
    size_t module_count;
    size_t module_cap;
    dcc_app_route_id_t next_route_id;
    dcc_task_group_t *tasks;
    dcc_app_error_fn error_handler;
    void *error_user_data;
    void *state;
    dcc_app_cleanup_fn state_cleanup;
    dcc_app_command_sync_options_t command_sync_options;
    dcc_listener_id_t command_sync_listener_id;
    uint64_t auto_defer_after_ms;
    uint8_t auto_defer_ephemeral;
    uint8_t command_sync_on_ready;
    uint8_t command_sync_listener_registered;
    uint8_t command_sync_ran;
    uint8_t store_open;
    atomic_bool stopping;
    uint8_t started;
};

struct dcc_ctx {
    dcc_app_t *app;
    dcc_client_t *client;
    const dcc_event_t *event;
    const dcc_interaction_t *interaction;
    dcc_interaction_flow_t flow;
    const dcc_component_session_result_t *component_session;
    dcc_app_auto_defer_t *auto_defer;
    void *user_data;
};

dcc_status_t dcc_app_add_route(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *key,
    uint8_t prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
dcc_status_t dcc_app_add_route_with_cleanup(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *key,
    uint8_t prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    void (*user_data_cleanup)(void *user_data),
    dcc_app_route_id_t *out_route
);
dcc_status_t dcc_app_use_internal(
    dcc_app_t *app,
    dcc_app_middleware_fn middleware,
    void *user_data,
    void (*cleanup)(void *user_data)
);
dcc_status_t dcc_app_on_event_listener_internal(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *state
);
void dcc_app_dispatch_event(dcc_client_t *client, const dcc_event_t *event, void *user_data);
dcc_status_t dcc_app_dispatch_handler(
    dcc_app_t *app,
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_interaction_t *interaction,
    const dcc_component_session_result_t *component_session,
    const dcc_app_middleware_t *route_middlewares,
    size_t route_middleware_count,
    dcc_app_handler_fn handler,
    void *user_data
);
dcc_status_t dcc_app_route_use_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_middleware_fn middleware,
    void *user_data,
    void (*cleanup)(void *user_data)
);
dcc_status_t dcc_app_route_apply_policy_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
);
dcc_status_t dcc_app_route_group_validate_internal(const dcc_app_route_group_t *group);
uint8_t dcc_app_route_group_matches_internal(
    const dcc_app_route_group_t *group,
    const dcc_app_route_t *route
);
dcc_status_t dcc_app_subcommand_key(
    const char *command_name,
    const char *subcommand_path,
    char *buffer,
    size_t buffer_len
);
dcc_status_t dcc_app_auto_defer_start(dcc_ctx_t *ctx);
void dcc_app_auto_defer_finish(dcc_ctx_t *ctx);
dcc_app_response_state_t dcc_app_auto_defer_response_state(const dcc_ctx_t *ctx);
dcc_status_t dcc_app_auto_defer_claim_initial(dcc_ctx_t *ctx, dcc_app_response_state_t state);
void dcc_app_auto_defer_mark(dcc_ctx_t *ctx, dcc_app_response_state_t state, dcc_status_t status);
dcc_status_t dcc_app_start_schedules(dcc_app_t *app);
void dcc_app_stop_schedules(dcc_app_t *app);
dcc_status_t dcc_app_register_command_sync_listener(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
