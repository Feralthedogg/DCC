#ifndef DCC_APP_INTERNAL_H
#define DCC_APP_INTERNAL_H

#include "internal/dcc_windows_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

#include <dcc/app.h>
#include <dcc/app/legacy.h>
#include <dcc/tasks.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if !defined(_WIN32)
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_app_schedule_kind {
  DCC_APP_SCHEDULE_INTERVAL = 0,
  DCC_APP_SCHEDULE_DAILY_KST
} dcc_app_schedule_kind_t;

typedef dcc_status_t (*dcc_app_canonical_schedule_fn)(void *listener_state,
                                                      dcc_app_t *app);

typedef enum dcc_app_response_state {
  DCC_APP_RESPONSE_READY = 0,
  DCC_APP_RESPONSE_DEFERRED = 1,
  DCC_APP_RESPONSE_REPLIED = 2,
  DCC_APP_RESPONSE_FAILED = 3,
  DCC_APP_RESPONSE_CLAIMED = 4
} dcc_app_response_state_t;

typedef struct dcc_app_auto_defer {
  dcc_interaction_flow_t *flow;
  atomic_uint refs;
  atomic_bool done;
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
  dcc_app_legacy_handler_fn legacy_handler;
  void *user_data;
  void *context_user_data;
  void *listener_state;
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
  dcc_app_legacy_task_fn fn;
  dcc_app_canonical_schedule_fn canonical_fn;
  void *user_data;
  void *listener_state;
  atomic_bool cancelled;
} dcc_app_schedule_t;

typedef struct dcc_app_listener_entry dcc_app_listener_entry_t;

typedef struct dcc_app_callback_frame {
  dcc_app_t *app;
  void *listener_state;
  struct dcc_app_callback_frame *previous;
} dcc_app_callback_frame_t;

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
  dcc_app_listener_entry_t **listeners;
  size_t listener_count;
  size_t listener_cap;
  dcc_app_listener_entry_t *retired_listeners;
  dcc_listener_id_t next_listener_id;
  dcc_app_route_id_t next_route_id;
  dcc_task_group_t *tasks;
  dcc_status_t task_reap_status;
  dcc_app_error_fn error_handler;
  void *error_user_data;
  uint8_t default_error_responses;
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
  uint8_t listener_sync_initialized;
  uint8_t listener_destroying;
  uint8_t tearing_down;
  uint8_t task_reaping;
  void (*listener_test_before_route_remove)(void *user_data);
  void *listener_test_before_route_remove_data;
  uint8_t listener_test_fail_policy_allocation;
  uint8_t listener_test_fail_schedule_allocation;
  uint8_t listener_test_fail_task_cancel;
  dcc_status_t *listener_test_task_cancel_status_out;
  uint8_t listener_test_fail_task_reap;
  size_t listener_test_fail_metadata_copy_after;
#if defined(_WIN32)
  CRITICAL_SECTION listener_mutex;
  CONDITION_VARIABLE listener_cond;
#else
  pthread_mutex_t listener_mutex;
  pthread_cond_t listener_cond;
#endif
  atomic_bool stopping;
  uint8_t started;
};

struct dcc_ctx {
  dcc_app_t *app;
  dcc_client_t *client;
  const dcc_event_t *event;
  const dcc_interaction_t *interaction;
  dcc_interaction_flow_t *flow;
  const dcc_component_session_result_t *component_session;
  dcc_app_auto_defer_t *auto_defer;
  void *user_data;
};

dcc_status_t dcc_app_add_route(dcc_app_t *app, dcc_event_type_t type,
                               const char *key, uint8_t prefix,
                               dcc_app_legacy_handler_fn handler,
                               void *user_data, dcc_app_route_id_t *out_route);
dcc_status_t dcc_app_add_route_with_cleanup(
    dcc_app_t *app, dcc_event_type_t type, const char *key, uint8_t prefix,
    dcc_app_legacy_handler_fn handler, void *user_data,
    void (*user_data_cleanup)(void *user_data), dcc_app_route_id_t *out_route);
dcc_status_t dcc_app_add_canonical_route_with_cleanup(
    dcc_app_t *app, dcc_event_type_t type, const char *key, uint8_t prefix,
    dcc_app_handler_fn handler, void *user_data, void *context_user_data,
    void *listener_state, void (*user_data_cleanup)(void *user_data),
    dcc_app_route_id_t *out_route);
dcc_status_t dcc_app_remove_route_internal(dcc_app_t *app,
                                           dcc_app_route_id_t route_id);
dcc_status_t dcc_app_use_internal(dcc_app_t *app,
                                  dcc_app_middleware_fn middleware,
                                  void *user_data,
                                  void (*cleanup)(void *user_data));
dcc_status_t dcc_app_on_event_listener_internal(dcc_app_t *app,
                                                dcc_event_type_t type,
                                                dcc_event_cb cb, void *state);
void dcc_app_dispatch_event(dcc_client_t *client, const dcc_event_t *event,
                            void *user_data);
dcc_status_t dcc_app_dispatch_handler(
    dcc_app_t *app, dcc_client_t *client, const dcc_event_t *event,
    const dcc_interaction_t *interaction,
    const dcc_component_session_result_t *component_session,
    const dcc_app_middleware_t *route_middlewares,
    size_t route_middleware_count, dcc_app_legacy_handler_fn handler,
    void *user_data);
dcc_status_t dcc_app_dispatch_canonical_handler(
    dcc_app_t *app, dcc_client_t *client, const dcc_event_t *event,
    const dcc_interaction_t *interaction,
    const dcc_component_session_result_t *component_session,
    const dcc_app_middleware_t *route_middlewares,
    size_t route_middleware_count, dcc_app_handler_fn handler, void *user_data,
    void *context_user_data);
dcc_status_t dcc_app_route_use_internal(dcc_app_t *app,
                                        dcc_app_route_id_t route_id,
                                        dcc_app_middleware_fn middleware,
                                        void *user_data,
                                        void (*cleanup)(void *user_data));
dcc_status_t
dcc_app_route_apply_policy_internal(dcc_app_t *app, dcc_app_route_id_t route_id,
                                    const dcc_app_route_policy_t *policy);
dcc_status_t
dcc_app_route_group_validate_internal(const dcc_app_route_group_t *group);
uint8_t dcc_app_route_group_matches_internal(const dcc_app_route_group_t *group,
                                             const dcc_app_route_t *route);
dcc_status_t dcc_app_subcommand_key(const char *command_name,
                                    const char *subcommand_path, char *buffer,
                                    size_t buffer_len);
dcc_status_t dcc_app_auto_defer_start(dcc_ctx_t *ctx);
void dcc_app_auto_defer_finish(dcc_ctx_t *ctx);
uint8_t dcc_ctx_initial_response_admitted(const dcc_ctx_t *ctx);
dcc_status_t dcc_app_start_schedules(dcc_app_t *app);
dcc_status_t dcc_app_request_stop_schedules(dcc_app_t *app);
dcc_status_t dcc_app_reap_schedules(dcc_app_t *app);
dcc_status_t dcc_app_register_command_sync_listener(dcc_app_t *app);
void dcc_app_listener_destroy_all(dcc_app_t *app);
void dcc_app_listener_reclaim_all(dcc_app_t *app);
dcc_status_t dcc_app_listener_sync_init(dcc_app_t *app);
void dcc_app_listener_sync_deinit(dcc_app_t *app);
void dcc_app_listener_lock(dcc_app_t *app);
void dcc_app_listener_unlock(dcc_app_t *app);
void dcc_app_listener_wait(dcc_app_t *app);
void dcc_app_listener_wake_all(dcc_app_t *app);
void dcc_app_callback_frame_enter(dcc_app_callback_frame_t *frame,
                                  dcc_app_t *app, void *listener_state);
void dcc_app_callback_frame_leave(dcc_app_callback_frame_t *frame);
uint8_t dcc_app_callback_frame_active(const dcc_app_t *app);
uint8_t dcc_app_listener_acquire(void *listener_state);
void dcc_app_listener_release(void *listener_state);
uint8_t dcc_app_listener_active(const void *listener_state);
dcc_status_t dcc_app_listener_run_task(void *listener_state, dcc_app_t *app);
dcc_status_t dcc_app_add_canonical_schedule(
    dcc_app_t *app, dcc_app_schedule_kind_t kind, uint64_t interval_ms,
    uint8_t hour, uint8_t minute, dcc_app_canonical_schedule_fn fn,
    void *listener_state, dcc_app_schedule_t **out_schedule);
void dcc_app_cancel_canonical_schedule(dcc_app_t *app,
                                       dcc_app_schedule_t *schedule);
dcc_status_t dcc_app_attach_error_sink(dcc_app_t *app);
void dcc_app_detach_error_sink(dcc_app_t *app);
void dcc_app_notify_error(dcc_app_t *app, const dcc_error_t *error);
dcc_status_t dcc_app_report_handler_error(dcc_app_t *app, dcc_ctx_t *ctx,
                                          dcc_status_t status,
                                          const char *operation,
                                          const char *message);

#ifdef __cplusplus
}
#endif

#endif
