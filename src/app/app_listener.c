#include "internal/app/dcc_app_internal.h"

#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/dcc_core_internal.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct dcc_app_listener_entry {
    dcc_app_t *app;
    dcc_listener_id_t id;
    dcc_listener_kind_t kind;
    dcc_listener_handler_t handler;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
    size_t in_flight;
    uint8_t active;
    uint8_t fired;
    uint8_t detach_started;
    uint8_t detach_done;
    dcc_status_t detach_status;
    uint8_t finalize_started;
    uint8_t finalize_done;
    dcc_status_t finalize_status;
    uint8_t retired;
    dcc_app_route_id_t route_id;
    dcc_event_type_t event_type;
    dcc_listener_id_t client_listener_id;
    dcc_app_schedule_t *schedule;
    size_t args_size;
    dcc_listener_binding_kind_t binding_kind;
    dcc_ctx_option_field_binding_t *option_bindings;
    dcc_ctx_form_field_binding_t *form_bindings;
    dcc_ctx_component_field_binding_t *component_bindings;
    size_t binding_count;
    dcc_ctx_field_validator_t *validators;
    size_t validator_count;
    uint8_t suppress_validation_response;
    uint8_t once;
    size_t prefix_len;
    size_t name_len;
    char *message_command_text;
    struct dcc_app_listener_entry *next_retired;
};

static _Thread_local dcc_app_callback_frame_t *dcc_app_current_callback_frame;

dcc_status_t dcc_app_listener_sync_init(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
#if defined(_WIN32)
    InitializeCriticalSection(&app->listener_mutex);
    InitializeConditionVariable(&app->listener_cond);
#else
    pthread_mutexattr_t attributes;
    if (pthread_mutexattr_init(&attributes) != 0) {
        return DCC_ERR_STATE;
    }
    int status = pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
    if (status == 0) {
        status = pthread_mutex_init(&app->listener_mutex, &attributes);
    }
    (void)pthread_mutexattr_destroy(&attributes);
    if (status != 0) {
        return DCC_ERR_STATE;
    }
    if (pthread_cond_init(&app->listener_cond, NULL) != 0) {
        (void)pthread_mutex_destroy(&app->listener_mutex);
        return DCC_ERR_STATE;
    }
#endif
    app->listener_sync_initialized = 1U;
    return DCC_OK;
}

void dcc_app_listener_sync_deinit(dcc_app_t *app) {
    if (app == NULL || !app->listener_sync_initialized) {
        return;
    }
#if defined(_WIN32)
    DeleteCriticalSection(&app->listener_mutex);
#else
    (void)pthread_cond_destroy(&app->listener_cond);
    (void)pthread_mutex_destroy(&app->listener_mutex);
#endif
    app->listener_sync_initialized = 0U;
}

void dcc_app_listener_lock(dcc_app_t *app) {
    if (app == NULL || !app->listener_sync_initialized) {
        return;
    }
#if defined(_WIN32)
    EnterCriticalSection(&app->listener_mutex);
#else
    (void)pthread_mutex_lock(&app->listener_mutex);
#endif
}

void dcc_app_listener_unlock(dcc_app_t *app) {
    if (app == NULL || !app->listener_sync_initialized) {
        return;
    }
#if defined(_WIN32)
    LeaveCriticalSection(&app->listener_mutex);
#else
    (void)pthread_mutex_unlock(&app->listener_mutex);
#endif
}

void dcc_app_listener_wait(dcc_app_t *app) {
    if (app == NULL || !app->listener_sync_initialized) {
        return;
    }
#if defined(_WIN32)
    (void)SleepConditionVariableCS(
        &app->listener_cond,
        &app->listener_mutex,
        INFINITE
    );
#else
    (void)pthread_cond_wait(&app->listener_cond, &app->listener_mutex);
#endif
}

void dcc_app_listener_wake_all(dcc_app_t *app) {
    if (app == NULL || !app->listener_sync_initialized) {
        return;
    }
#if defined(_WIN32)
    WakeAllConditionVariable(&app->listener_cond);
#else
    (void)pthread_cond_broadcast(&app->listener_cond);
#endif
}

static uint8_t dcc_app_listener_has_full_value(
    size_t size,
    uint32_t version,
    size_t required,
    uint32_t expected_version
) {
    return size >= required && version == expected_version;
}

static uint8_t dcc_app_listener_is_route_kind(dcc_listener_kind_t kind) {
    return kind >= DCC_LISTENER_SLASH && kind <= DCC_LISTENER_MODAL_PREFIX;
}

static uint8_t dcc_app_listener_is_component_kind(dcc_listener_kind_t kind) {
    return kind >= DCC_LISTENER_BUTTON && kind <= DCC_LISTENER_MODAL_PREFIX;
}

static dcc_event_type_t dcc_app_listener_route_event(dcc_listener_kind_t kind) {
    switch (kind) {
        case DCC_LISTENER_SLASH:
        case DCC_LISTENER_SUBCOMMAND:
            return DCC_EVENT_SLASH_COMMAND;
        case DCC_LISTENER_AUTOCOMPLETE:
            return DCC_EVENT_AUTOCOMPLETE;
        case DCC_LISTENER_USER_CONTEXT_MENU:
            return DCC_EVENT_USER_CONTEXT_MENU;
        case DCC_LISTENER_MESSAGE_CONTEXT_MENU:
            return DCC_EVENT_MESSAGE_CONTEXT_MENU;
        case DCC_LISTENER_BUTTON:
        case DCC_LISTENER_BUTTON_PREFIX:
            return DCC_EVENT_BUTTON_CLICK;
        case DCC_LISTENER_SELECT:
        case DCC_LISTENER_SELECT_PREFIX:
            return DCC_EVENT_SELECT_CLICK;
        case DCC_LISTENER_MODAL:
        case DCC_LISTENER_MODAL_PREFIX:
            return DCC_EVENT_FORM_SUBMIT;
        default:
            return DCC_EVENT_MAX;
    }
}

static uint8_t dcc_app_listener_route_prefix(dcc_listener_kind_t kind) {
    return kind == DCC_LISTENER_BUTTON_PREFIX ||
           kind == DCC_LISTENER_SELECT_PREFIX ||
           kind == DCC_LISTENER_MODAL_PREFIX;
}

static void dcc_app_listener_cleanup_user(dcc_app_listener_entry_t *entry) {
    if (entry == NULL) {
        return;
    }
    if (entry->cleanup != NULL) {
        dcc_app_callback_frame_t frame;
        dcc_app_callback_frame_enter(&frame, entry->app, entry);
        entry->cleanup(entry->user_data);
        dcc_app_callback_frame_leave(&frame);
    }
}

static void dcc_app_listener_entry_free(dcc_app_listener_entry_t *entry) {
    if (entry == NULL) {
        return;
    }
    for (size_t i = 0U; i < entry->binding_count; ++i) {
        if (entry->option_bindings != NULL) {
            free((void *)entry->option_bindings[i].name);
            free((void *)entry->option_bindings[i].fallback_string);
        }
        if (entry->form_bindings != NULL) {
            free((void *)entry->form_bindings[i].custom_id);
            free((void *)entry->form_bindings[i].fallback_string);
            for (size_t j = 0U; j < entry->form_bindings[i].fallback_values_count; ++j) {
                free((void *)entry->form_bindings[i].fallback_values[j]);
            }
            free((void *)entry->form_bindings[i].fallback_values);
        }
        if (entry->component_bindings != NULL) {
            free((void *)entry->component_bindings[i].custom_id_prefix);
            free((void *)entry->component_bindings[i].fallback_string);
            for (size_t j = 0U; j < entry->component_bindings[i].fallback_values_count; ++j) {
                free((void *)entry->component_bindings[i].fallback_values[j]);
            }
            free((void *)entry->component_bindings[i].fallback_values);
        }
    }
    for (size_t i = 0U; i < entry->validator_count; ++i) {
        free((void *)entry->validators[i].field);
    }
    free(entry->option_bindings);
    free(entry->form_bindings);
    free(entry->component_bindings);
    free(entry->validators);
    free(entry->message_command_text);
    free(entry->schedule);
    free(entry);
}

static dcc_status_t dcc_app_listener_copy_string(const char *value, const char **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (value == NULL) {
        return DCC_OK;
    }
    char *copy = dcc_strdup(value);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    *out = copy;
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_copy_string_array(
    dcc_app_t *app,
    const char *const *values,
    size_t count,
    const char ***out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (count == 0U) {
        return DCC_OK;
    }
    if (values == NULL || count > SIZE_MAX / sizeof(**out)) {
        return values == NULL ? DCC_ERR_INVALID_ARG : DCC_ERR_NOMEM;
    }
    const char **copy = (const char **)calloc(count, sizeof(*copy));
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < count; ++i) {
        dcc_status_t status = dcc_app_listener_copy_string(values[i], &copy[i]);
        if (status == DCC_OK && app != NULL &&
            app->listener_test_fail_metadata_copy_after == i + 1U) {
            app->listener_test_fail_metadata_copy_after = 0U;
            status = DCC_ERR_NOMEM;
        }
        if (status != DCC_OK) {
            for (size_t j = 0U; j <= i; ++j) {
                free((void *)copy[j]);
            }
            free(copy);
            return status;
        }
    }
    *out = copy;
    return DCC_OK;
}

typedef struct dcc_app_listener_array_stage {
    dcc_app_listener_entry_t **original;
    size_t original_cap;
    uint8_t active;
} dcc_app_listener_array_stage_t;

typedef struct dcc_app_route_array_stage {
    dcc_app_route_t *original;
    size_t original_cap;
    uint8_t active;
} dcc_app_route_array_stage_t;

typedef struct dcc_app_schedule_array_stage {
    dcc_app_schedule_t **original;
    size_t original_cap;
    uint8_t active;
} dcc_app_schedule_array_stage_t;

static dcc_status_t dcc_app_listener_stage_pointer_array(
    void **array,
    size_t count,
    size_t *cap,
    size_t item_size,
    size_t initial_cap,
    size_t need,
    void **out_original,
    size_t *out_original_cap,
    uint8_t *out_active
) {
    *out_original = NULL;
    *out_original_cap = 0U;
    *out_active = 0U;
    if (need <= *cap) {
        return DCC_OK;
    }
    size_t next_cap = *cap != 0U ? *cap : initial_cap;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / item_size) {
        return DCC_ERR_NOMEM;
    }
    void *staged = malloc(next_cap * item_size);
    if (staged == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (count != 0U) {
        memcpy(staged, *array, count * item_size);
    }
    *out_original = *array;
    *out_original_cap = *cap;
    *array = staged;
    *cap = next_cap;
    *out_active = 1U;
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_stage_listener_array(
    dcc_app_t *app,
    dcc_app_listener_array_stage_t *stage
) {
    memset(stage, 0, sizeof(*stage));
    return dcc_app_listener_stage_pointer_array(
        (void **)&app->listeners,
        app->listener_count,
        &app->listener_cap,
        sizeof(*app->listeners),
        8U,
        app->listener_count + 1U,
        (void **)&stage->original,
        &stage->original_cap,
        &stage->active
    );
}

static dcc_status_t dcc_app_listener_stage_route_array(
    dcc_app_t *app,
    dcc_app_route_array_stage_t *stage
) {
    memset(stage, 0, sizeof(*stage));
    return dcc_app_listener_stage_pointer_array(
        (void **)&app->routes,
        app->route_count,
        &app->route_cap,
        sizeof(*app->routes),
        8U,
        app->route_count + 1U,
        (void **)&stage->original,
        &stage->original_cap,
        &stage->active
    );
}

static dcc_status_t dcc_app_listener_stage_schedule_array(
    dcc_app_t *app,
    dcc_app_schedule_array_stage_t *stage
) {
    memset(stage, 0, sizeof(*stage));
    return dcc_app_listener_stage_pointer_array(
        (void **)&app->schedules,
        app->schedule_count,
        &app->schedule_cap,
        sizeof(*app->schedules),
        4U,
        app->schedule_count + 1U,
        (void **)&stage->original,
        &stage->original_cap,
        &stage->active
    );
}

static void dcc_app_listener_finish_listener_array(
    dcc_app_t *app,
    dcc_app_listener_array_stage_t *stage,
    uint8_t commit
) {
    if (!stage->active) {
        return;
    }
    if (commit) {
        free(stage->original);
    } else {
        free(app->listeners);
        app->listeners = stage->original;
        app->listener_cap = stage->original_cap;
    }
    stage->active = 0U;
}

static void dcc_app_listener_finish_route_array(
    dcc_app_t *app,
    dcc_app_route_array_stage_t *stage,
    uint8_t commit
) {
    if (!stage->active) {
        return;
    }
    if (commit) {
        free(stage->original);
    } else {
        free(app->routes);
        app->routes = stage->original;
        app->route_cap = stage->original_cap;
    }
    stage->active = 0U;
}

static void dcc_app_listener_finish_schedule_array(
    dcc_app_t *app,
    dcc_app_schedule_array_stage_t *stage,
    uint8_t commit
) {
    if (!stage->active) {
        return;
    }
    if (commit) {
        free(stage->original);
    } else {
        free(app->schedules);
        app->schedules = stage->original;
        app->schedule_cap = stage->original_cap;
    }
    stage->active = 0U;
}

static uint8_t dcc_app_listener_is_current(const dcc_app_listener_entry_t *entry) {
    for (dcc_app_callback_frame_t *frame = dcc_app_current_callback_frame;
         frame != NULL;
         frame = frame->previous) {
        if (frame->listener_state == entry) {
            return 1U;
        }
    }
    return 0U;
}

void dcc_app_callback_frame_enter(
    dcc_app_callback_frame_t *frame,
    dcc_app_t *app,
    void *listener_state
) {
    if (frame == NULL) {
        return;
    }
    frame->app = app;
    frame->listener_state = listener_state;
    frame->previous = dcc_app_current_callback_frame;
    dcc_app_current_callback_frame = frame;
}

void dcc_app_callback_frame_leave(dcc_app_callback_frame_t *frame) {
    if (dcc_app_current_callback_frame == frame) {
        dcc_app_current_callback_frame = frame->previous;
    }
}

uint8_t dcc_app_callback_frame_active(const dcc_app_t *app) {
    for (dcc_app_callback_frame_t *frame = dcc_app_current_callback_frame;
         frame != NULL;
         frame = frame->previous) {
        if (frame->app == app) {
            return 1U;
        }
    }
    return 0U;
}

static void dcc_app_listener_retire_locked(
    dcc_app_t *app,
    dcc_app_listener_entry_t *entry
) {
    if (entry->retired) {
        return;
    }
    for (size_t i = 0U; i < app->listener_count; ++i) {
        if (app->listeners[i] != entry) {
            continue;
        }
        if (i + 1U < app->listener_count) {
            memmove(
                &app->listeners[i],
                &app->listeners[i + 1U],
                (app->listener_count - i - 1U) * sizeof(*app->listeners)
            );
        }
        app->listener_count--;
        app->listeners[app->listener_count] = NULL;
        break;
    }
    entry->retired = 1U;
    entry->next_retired = app->retired_listeners;
    app->retired_listeners = entry;
}

static dcc_status_t dcc_app_listener_detach_source(
    dcc_app_listener_entry_t *entry,
    uint8_t wait_for_completion
) {
    if (entry == NULL || entry->app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_t *app = entry->app;
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_event_type_t event_type = DCC_EVENT_READY;
    dcc_listener_id_t event_id = 0U;
    dcc_app_schedule_t *schedule = NULL;
    dcc_app_listener_lock(app);
    if (entry->detach_done) {
        dcc_status_t status = entry->detach_status;
        dcc_app_listener_unlock(app);
        return status;
    }
    if (entry->detach_started) {
        while (wait_for_completion && !entry->detach_done) {
            dcc_app_listener_wait(app);
        }
        dcc_status_t status = entry->detach_done ? entry->detach_status : DCC_OK;
        dcc_app_listener_unlock(app);
        return status;
    }
    if (entry->route_id != DCC_APP_ROUTE_INVALID) {
        if (entry->in_flight != 0U) {
            dcc_app_listener_unlock(app);
            return DCC_OK;
        }
        route_id = entry->route_id;
        entry->route_id = DCC_APP_ROUTE_INVALID;
    } else if (entry->client_listener_id != 0U) {
        event_type = entry->event_type;
        event_id = entry->client_listener_id;
        entry->client_listener_id = 0U;
    } else if (entry->schedule != NULL) {
        schedule = entry->schedule;
    }
    entry->detach_started = 1U;
    dcc_app_listener_unlock(app);

    dcc_status_t status = DCC_OK;
    if (route_id != DCC_APP_ROUTE_INVALID) {
        if (app->listener_test_before_route_remove != NULL) {
            app->listener_test_before_route_remove(
                app->listener_test_before_route_remove_data
            );
        }
        status = dcc_app_remove_route_internal(app, route_id);
    } else if (event_id != 0U) {
        status = dcc_client_off(app->client, event_type, event_id);
    } else if (schedule != NULL) {
        dcc_app_cancel_canonical_schedule(app, schedule);
    }
    dcc_app_listener_lock(app);
    entry->detach_status = status;
    entry->detach_done = 1U;
    dcc_app_listener_wake_all(app);
    dcc_app_listener_unlock(app);
    return status;
}

static dcc_status_t dcc_app_listener_finalize(
    dcc_app_listener_entry_t *entry,
    uint8_t wait_for_completion
) {
    if (entry == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_t *app = entry->app;
    dcc_app_listener_lock(app);
    if (entry->active || entry->in_flight != 0U) {
        dcc_app_listener_unlock(app);
        return DCC_OK;
    }
    if (entry->finalize_done) {
        dcc_status_t status = entry->finalize_status;
        dcc_app_listener_unlock(app);
        return status;
    }
    if (entry->finalize_started) {
        while (wait_for_completion && !entry->finalize_done) {
            dcc_app_listener_wait(app);
        }
        dcc_status_t status = entry->finalize_done ? entry->finalize_status : DCC_OK;
        dcc_app_listener_unlock(app);
        return status;
    }
    entry->finalize_started = 1U;
    dcc_app_listener_unlock(app);

    dcc_status_t status = dcc_app_listener_detach_source(entry, 1U);
    dcc_app_listener_cleanup_user(entry);
    dcc_app_listener_lock(app);
    entry->finalize_status = status;
    entry->finalize_done = 1U;
    dcc_app_listener_wake_all(app);
    dcc_app_listener_unlock(app);
    return status;
}

uint8_t dcc_app_listener_acquire(void *listener_state) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)listener_state;
    if (entry == NULL || entry->app == NULL) {
        return 0U;
    }
    dcc_app_t *app = entry->app;
    uint8_t claimed_once = 0U;
    dcc_app_listener_lock(app);
    if (!entry->active || app->listener_destroying) {
        dcc_app_listener_unlock(app);
        return 0U;
    }
    if (entry->once) {
        if (entry->fired) {
            dcc_app_listener_unlock(app);
            return 0U;
        }
        entry->fired = 1U;
        entry->active = 0U;
        dcc_app_listener_retire_locked(app, entry);
        claimed_once = 1U;
    }
    entry->in_flight++;
    dcc_app_listener_unlock(app);
    if (claimed_once) {
        (void)dcc_app_listener_detach_source(entry, 1U);
    }
    return 1U;
}

void dcc_app_listener_release(void *listener_state) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)listener_state;
    if (entry == NULL || entry->app == NULL) {
        return;
    }
    dcc_app_t *app = entry->app;
    uint8_t finalize = 0U;
    dcc_app_listener_lock(app);
    if (entry->in_flight != 0U) {
        entry->in_flight--;
    }
    if (entry->in_flight == 0U) {
        finalize = !entry->active;
        dcc_app_listener_wake_all(app);
    }
    dcc_app_listener_unlock(app);
    if (finalize) {
        (void)dcc_app_listener_finalize(entry, 0U);
    }
}

void dcc_listener_init(dcc_listener_t *listener, dcc_listener_kind_t kind) {
    if (listener == NULL) {
        return;
    }
    memset(listener, 0, sizeof(*listener));
    listener->size = sizeof(*listener);
    listener->version = DCC_LISTENER_VERSION;
    listener->kind = kind;
    listener->policy.size = sizeof(listener->policy);
    listener->policy.version = DCC_LISTENER_ROUTE_POLICY_VERSION;
    listener->policy.cooldown.size = sizeof(listener->policy.cooldown);
    listener->policy.cooldown.version = DCC_LISTENER_COOLDOWN_VERSION;
    listener->bindings.size = sizeof(listener->bindings);
    listener->bindings.version = DCC_LISTENER_BINDINGS_VERSION;
    listener->validators.size = sizeof(listener->validators);
    listener->validators.version = DCC_LISTENER_VALIDATORS_VERSION;
    listener->validation.size = sizeof(listener->validation);
    listener->validation.version = DCC_LISTENER_VALIDATION_POLICY_VERSION;
    if (dcc_app_listener_is_route_kind(kind)) {
        listener->target.route.size = sizeof(listener->target.route);
        listener->target.route.version = DCC_LISTENER_TARGET_VERSION;
    } else if (kind == DCC_LISTENER_EVENT || kind == DCC_LISTENER_READY ||
               (kind >= DCC_LISTENER_MESSAGE_CREATE &&
                kind <= DCC_LISTENER_MESSAGE_DELETE)) {
        listener->target.event.size = sizeof(listener->target.event);
        listener->target.event.version = DCC_LISTENER_TARGET_VERSION;
    } else if (kind == DCC_LISTENER_MESSAGE_COMMAND) {
        listener->target.message_command.size = sizeof(listener->target.message_command);
        listener->target.message_command.version = DCC_LISTENER_TARGET_VERSION;
    } else if (kind == DCC_LISTENER_TASK) {
        listener->target.schedule.size = sizeof(listener->target.schedule);
        listener->target.schedule.version = DCC_LISTENER_TARGET_VERSION;
    }
}

static dcc_status_t dcc_app_listener_validate_policy(
    const dcc_listener_route_policy_t *policy
) {
    if (!dcc_app_listener_has_full_value(
            policy->size,
            policy->version,
            sizeof(*policy),
            DCC_LISTENER_ROUTE_POLICY_VERSION
        ) ||
        ((policy->middleware_count == 0U) != (policy->middlewares == NULL)) ||
        ((policy->owner_user_id_count == 0U) != (policy->owner_user_ids == NULL)) ||
        ((policy->check_count == 0U) != (policy->checks == NULL)) ||
        ((policy->channel_type_count == 0U) != (policy->channel_types == NULL)) ||
        ((policy->required_role_id_count == 0U) != (policy->required_role_ids == NULL)) ||
        ((policy->any_role_id_count == 0U) != (policy->any_role_ids == NULL)) ||
        policy->guild_only > 1U || policy->dm_only > 1U || policy->nsfw_only > 1U ||
        (policy->dm_only != 0U &&
         (policy->guild_only != 0U || policy->nsfw_only != 0U ||
          policy->required_permissions != 0U || policy->channel_type_count != 0U ||
          policy->required_role_id_count != 0U || policy->any_role_id_count != 0U ||
          policy->cooldown.bucket == DCC_LISTENER_COOLDOWN_GUILD))) {
        return DCC_ERR_INVALID_ARG;
    }
    if (policy->middleware_count > SIZE_MAX / sizeof(*policy->middlewares) ||
        policy->owner_user_id_count > SIZE_MAX / sizeof(*policy->owner_user_ids) ||
        policy->check_count > SIZE_MAX / sizeof(*policy->checks) ||
        policy->channel_type_count > SIZE_MAX / sizeof(*policy->channel_types) ||
        policy->required_role_id_count > SIZE_MAX / sizeof(*policy->required_role_ids) ||
        policy->any_role_id_count > SIZE_MAX / sizeof(*policy->any_role_ids)) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < policy->middleware_count; ++i) {
        const dcc_listener_middleware_t *item = &policy->middlewares[i];
        if (!dcc_app_listener_has_full_value(
                item->size, item->version, sizeof(*item), DCC_LISTENER_MIDDLEWARE_VERSION
            ) ||
            item->callback == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    for (size_t i = 0U; i < policy->check_count; ++i) {
        const dcc_listener_check_t *item = &policy->checks[i];
        if (!dcc_app_listener_has_full_value(
                item->size, item->version, sizeof(*item), DCC_LISTENER_CHECK_VERSION
            ) ||
            item->callback == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    const dcc_listener_cooldown_t *cooldown = &policy->cooldown;
    if (!dcc_app_listener_has_full_value(
            cooldown->size,
            cooldown->version,
            sizeof(*cooldown),
            DCC_LISTENER_COOLDOWN_VERSION
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cooldown->bucket == DCC_LISTENER_COOLDOWN_NONE) {
        return cooldown->limit == 0U && cooldown->window_ms == 0U
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (cooldown->bucket < DCC_LISTENER_COOLDOWN_GLOBAL ||
        cooldown->bucket > DCC_LISTENER_COOLDOWN_GUILD ||
        cooldown->limit == 0U || cooldown->window_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static uint8_t dcc_app_listener_policy_is_empty(
    const dcc_listener_route_policy_t *policy
) {
    return policy->middlewares == NULL && policy->middleware_count == 0U &&
           policy->owner_user_ids == NULL && policy->owner_user_id_count == 0U &&
           policy->required_permissions == 0U && policy->guild_only == 0U &&
           policy->cooldown.bucket == DCC_LISTENER_COOLDOWN_NONE &&
           policy->cooldown.limit == 0U && policy->cooldown.window_ms == 0U &&
           policy->checks == NULL && policy->check_count == 0U &&
           policy->dm_only == 0U && policy->nsfw_only == 0U &&
           policy->channel_types == NULL && policy->channel_type_count == 0U &&
           policy->required_role_ids == NULL && policy->required_role_id_count == 0U &&
           policy->any_role_ids == NULL && policy->any_role_id_count == 0U;
}

static uint8_t dcc_app_listener_binding_has_zero_scalar_fallbacks(
    const dcc_listener_binding_t *item
) {
    return item->fallback_integer == 0 && item->fallback_number == 0.0 &&
           item->fallback_boolean == 0U && item->fallback_snowflake == 0U &&
           item->fallback_component_type == 0U;
}

static uint8_t dcc_app_listener_binding_fallbacks_are_valid(
    dcc_listener_binding_kind_t kind,
    const dcc_listener_binding_t *item
) {
    if ((item->fallback_values_count == 0U) != (item->fallback_values == NULL)) {
        return 0U;
    }
    if (kind == DCC_LISTENER_BIND_OPTIONS) {
        if (item->custom_id_prefix != NULL || item->count_offset != 0U ||
            item->has_count_offset != 0U || item->fallback_values != NULL ||
            item->fallback_component_type != 0U) {
            return 0U;
        }
        switch ((dcc_ctx_option_bind_type_t)item->type) {
            case DCC_CTX_OPTION_BIND_STRING:
                return item->fallback_integer == 0 && item->fallback_number == 0.0 &&
                       item->fallback_boolean == 0U && item->fallback_snowflake == 0U;
            case DCC_CTX_OPTION_BIND_INTEGER:
                return item->fallback_string == NULL && item->fallback_number == 0.0 &&
                       item->fallback_boolean == 0U && item->fallback_snowflake == 0U;
            case DCC_CTX_OPTION_BIND_NUMBER:
                return item->fallback_string == NULL && item->fallback_integer == 0 &&
                       item->fallback_boolean == 0U && item->fallback_snowflake == 0U;
            case DCC_CTX_OPTION_BIND_BOOLEAN:
                return item->fallback_string == NULL && item->fallback_integer == 0 &&
                       item->fallback_number == 0.0 && item->fallback_snowflake == 0U &&
                       item->fallback_boolean <= 1U;
            case DCC_CTX_OPTION_BIND_SNOWFLAKE:
                return item->fallback_string == NULL && item->fallback_integer == 0 &&
                       item->fallback_number == 0.0 && item->fallback_boolean == 0U;
            case DCC_CTX_OPTION_BIND_USER:
            case DCC_CTX_OPTION_BIND_MEMBER:
            case DCC_CTX_OPTION_BIND_ROLE:
            case DCC_CTX_OPTION_BIND_CHANNEL:
            case DCC_CTX_OPTION_BIND_MESSAGE:
            case DCC_CTX_OPTION_BIND_ATTACHMENT:
                return item->fallback_string == NULL &&
                       dcc_app_listener_binding_has_zero_scalar_fallbacks(item);
        }
        return 0U;
    }
    if (kind == DCC_LISTENER_BIND_FORM) {
        if (item->custom_id_prefix != NULL || item->fallback_integer != 0 ||
            item->fallback_number != 0.0 || item->fallback_snowflake != 0U ||
            item->fallback_component_type != 0U) {
            return 0U;
        }
        if (item->type == DCC_CTX_FORM_BIND_STRING) {
            return item->fallback_boolean == 0U && item->fallback_values == NULL &&
                   item->count_offset == 0U && item->has_count_offset == 0U;
        }
        if (item->type == DCC_CTX_FORM_BIND_BOOLEAN) {
            return item->fallback_string == NULL && item->fallback_boolean <= 1U &&
                   item->fallback_values == NULL && item->count_offset == 0U &&
                   item->has_count_offset == 0U;
        }
        return item->type == DCC_CTX_FORM_BIND_VALUES && item->fallback_string == NULL &&
               item->fallback_boolean == 0U;
    }
    if (item->name != NULL || item->fallback_integer != 0 ||
        item->fallback_number != 0.0 || item->fallback_boolean != 0U ||
        item->fallback_snowflake != 0U) {
        return 0U;
    }
    if (item->type == DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX) {
        if (item->custom_id_prefix == NULL || item->custom_id_prefix[0] == '\0') {
            return 0U;
        }
    } else if (item->custom_id_prefix != NULL) {
        return 0U;
    }
    if (item->type == DCC_CTX_COMPONENT_BIND_VALUES) {
        return item->fallback_string == NULL && item->fallback_component_type == 0U;
    }
    if (item->type == DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE) {
        return item->fallback_string == NULL && item->fallback_values == NULL &&
               item->count_offset == 0U && item->has_count_offset == 0U;
    }
    return item->fallback_values == NULL && item->fallback_component_type == 0U &&
           item->count_offset == 0U && item->has_count_offset == 0U;
}

static uint8_t dcc_app_listener_validator_inactive_fields_are_zero(
    const dcc_listener_validator_t *item
) {
    switch (item->kind) {
        case DCC_LISTENER_VALIDATE_STRING_REQUIRED:
        case DCC_LISTENER_VALIDATE_DATE_YYYYMMDD:
        case DCC_LISTENER_VALIDATE_DATE_MMDD:
        case DCC_LISTENER_VALIDATE_DATE_MMDD_OR_YYYYMMDD:
            return item->min_length == 0U && item->max_length == 0U &&
                   item->min_integer == 0 && item->max_integer == 0 &&
                   item->min_number == 0.0 && item->max_number == 0.0;
        case DCC_LISTENER_VALIDATE_STRING_LENGTH:
            return item->min_integer == 0 && item->max_integer == 0 &&
                   item->min_number == 0.0 && item->max_number == 0.0;
        case DCC_LISTENER_VALIDATE_INTEGER_RANGE:
            return item->min_length == 0U && item->max_length == 0U &&
                   item->min_number == 0.0 && item->max_number == 0.0;
        case DCC_LISTENER_VALIDATE_NUMBER_RANGE:
            return item->min_length == 0U && item->max_length == 0U &&
                   item->min_integer == 0 && item->max_integer == 0;
    }
    return 0U;
}

static uint8_t dcc_app_listener_range_fits(
    size_t offset,
    size_t width,
    size_t total
) {
    return offset <= total && width <= total - offset;
}

static size_t dcc_app_listener_binding_width(
    dcc_listener_binding_kind_t kind,
    uint32_t type
) {
    if (kind == DCC_LISTENER_BIND_OPTIONS) {
        switch ((dcc_ctx_option_bind_type_t)type) {
            case DCC_CTX_OPTION_BIND_INTEGER:
                return sizeof(int64_t);
            case DCC_CTX_OPTION_BIND_NUMBER:
                return sizeof(double);
            case DCC_CTX_OPTION_BIND_BOOLEAN:
                return sizeof(uint8_t);
            case DCC_CTX_OPTION_BIND_SNOWFLAKE:
                return sizeof(dcc_snowflake_t);
            case DCC_CTX_OPTION_BIND_STRING:
            case DCC_CTX_OPTION_BIND_USER:
            case DCC_CTX_OPTION_BIND_MEMBER:
            case DCC_CTX_OPTION_BIND_ROLE:
            case DCC_CTX_OPTION_BIND_CHANNEL:
            case DCC_CTX_OPTION_BIND_MESSAGE:
            case DCC_CTX_OPTION_BIND_ATTACHMENT:
                return sizeof(void *);
        }
    } else if (kind == DCC_LISTENER_BIND_FORM) {
        return type == DCC_CTX_FORM_BIND_BOOLEAN ? sizeof(uint8_t) : sizeof(void *);
    } else if (kind == DCC_LISTENER_BIND_COMPONENT) {
        return type == DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE
            ? sizeof(uint32_t)
            : sizeof(void *);
    }
    return 0U;
}

static size_t dcc_app_listener_validator_width(dcc_listener_validator_kind_t kind) {
    switch (kind) {
        case DCC_LISTENER_VALIDATE_INTEGER_RANGE:
            return sizeof(int64_t);
        case DCC_LISTENER_VALIDATE_NUMBER_RANGE:
            return sizeof(double);
        case DCC_LISTENER_VALIDATE_STRING_REQUIRED:
        case DCC_LISTENER_VALIDATE_STRING_LENGTH:
        case DCC_LISTENER_VALIDATE_DATE_YYYYMMDD:
        case DCC_LISTENER_VALIDATE_DATE_MMDD:
        case DCC_LISTENER_VALIDATE_DATE_MMDD_OR_YYYYMMDD:
            return sizeof(void *);
    }
    return 0U;
}

static dcc_status_t dcc_app_listener_validate_bindings(
    const dcc_listener_t *listener
) {
    const dcc_listener_bindings_t *bindings = &listener->bindings;
    const dcc_listener_validators_t *validators = &listener->validators;
    const dcc_listener_validation_policy_t *validation = &listener->validation;
    if (!dcc_app_listener_has_full_value(
            bindings->size,
            bindings->version,
            sizeof(*bindings),
            DCC_LISTENER_BINDINGS_VERSION
        ) ||
        !dcc_app_listener_has_full_value(
            validators->size,
            validators->version,
            sizeof(*validators),
            DCC_LISTENER_VALIDATORS_VERSION
        ) ||
        !dcc_app_listener_has_full_value(
            validation->size,
            validation->version,
            sizeof(*validation),
            DCC_LISTENER_VALIDATION_POLICY_VERSION
        ) || validation->suppress_response > 1U) {
        return DCC_ERR_INVALID_ARG;
    }

    if (listener->args_size == 0U) {
        return bindings->kind == DCC_LISTENER_BIND_NONE && bindings->count == 0U &&
                       bindings->items.options == NULL && validators->count == 0U &&
                       validators->items == NULL && validation->suppress_response == 0U
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (bindings->kind < DCC_LISTENER_BIND_OPTIONS ||
        bindings->kind > DCC_LISTENER_BIND_COMPONENT ||
        bindings->count == 0U || bindings->items.options == NULL ||
        ((validators->count == 0U) != (validators->items == NULL))) {
        return DCC_ERR_INVALID_ARG;
    }
    if (bindings->count > SIZE_MAX / sizeof(*bindings->items.options) ||
        validators->count > SIZE_MAX / sizeof(*validators->items)) {
        return DCC_ERR_NOMEM;
    }
    if ((bindings->kind == DCC_LISTENER_BIND_OPTIONS &&
         !(listener->kind == DCC_LISTENER_SLASH ||
           listener->kind == DCC_LISTENER_SUBCOMMAND ||
           listener->kind == DCC_LISTENER_AUTOCOMPLETE)) ||
        (bindings->kind == DCC_LISTENER_BIND_FORM &&
         !(listener->kind == DCC_LISTENER_MODAL ||
           listener->kind == DCC_LISTENER_MODAL_PREFIX)) ||
        (bindings->kind == DCC_LISTENER_BIND_COMPONENT &&
         !(listener->kind == DCC_LISTENER_BUTTON ||
           listener->kind == DCC_LISTENER_BUTTON_PREFIX ||
           listener->kind == DCC_LISTENER_SELECT ||
           listener->kind == DCC_LISTENER_SELECT_PREFIX))) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < bindings->count; ++i) {
        const dcc_listener_binding_t *item = &bindings->items.options[i];
        if (!dcc_app_listener_has_full_value(
                item->size, item->version, sizeof(*item), DCC_LISTENER_BINDING_VERSION
            ) ||
            item->type == 0U || item->has_count_offset > 1U || item->required > 1U ||
            item->fallback_boolean > 1U) {
            return DCC_ERR_INVALID_ARG;
        }
        if (bindings->kind == DCC_LISTENER_BIND_OPTIONS &&
            (item->name == NULL || item->name[0] == '\0' ||
             item->type > DCC_CTX_OPTION_BIND_ATTACHMENT)) {
            return DCC_ERR_INVALID_ARG;
        }
        if (bindings->kind == DCC_LISTENER_BIND_FORM &&
            (item->name == NULL || item->name[0] == '\0' ||
             item->type > DCC_CTX_FORM_BIND_VALUES)) {
            return DCC_ERR_INVALID_ARG;
        }
        if (bindings->kind == DCC_LISTENER_BIND_COMPONENT &&
            (item->type < DCC_CTX_COMPONENT_BIND_CUSTOM_ID ||
             item->type > DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE)) {
            return DCC_ERR_INVALID_ARG;
        }
        size_t width = dcc_app_listener_binding_width(bindings->kind, item->type);
        uint8_t supports_count =
            (bindings->kind == DCC_LISTENER_BIND_FORM &&
             item->type == DCC_CTX_FORM_BIND_VALUES) ||
            (bindings->kind == DCC_LISTENER_BIND_COMPONENT &&
             item->type == DCC_CTX_COMPONENT_BIND_VALUES);
        if (width == 0U ||
            !dcc_app_listener_range_fits(item->field_offset, width, listener->args_size) ||
            (item->has_count_offset != 0U &&
             (!supports_count ||
              !dcc_app_listener_range_fits(
                  item->count_offset,
                  sizeof(size_t),
                  listener->args_size
              ))) ||
            (item->has_count_offset == 0U && item->count_offset != 0U) ||
            !dcc_app_listener_binding_fallbacks_are_valid(bindings->kind, item)) {
            return DCC_ERR_INVALID_ARG;
        }
        if (item->fallback_values_count > SIZE_MAX / sizeof(*item->fallback_values)) {
            return DCC_ERR_NOMEM;
        }
        for (size_t value_index = 0U; value_index < item->fallback_values_count;
             ++value_index) {
            if (item->fallback_values[value_index] == NULL) {
                return DCC_ERR_INVALID_ARG;
            }
        }
    }
    for (size_t i = 0U; i < validators->count; ++i) {
        const dcc_listener_validator_t *item = &validators->items[i];
        if (!dcc_app_listener_has_full_value(
                item->size, item->version, sizeof(*item), DCC_LISTENER_VALIDATOR_VERSION
            ) ||
            item->field == NULL || item->field[0] == '\0' ||
            item->kind < DCC_LISTENER_VALIDATE_STRING_REQUIRED ||
            item->kind > DCC_LISTENER_VALIDATE_DATE_MMDD_OR_YYYYMMDD ||
            !dcc_app_listener_validator_inactive_fields_are_zero(item) ||
            !dcc_app_listener_range_fits(
                item->field_offset,
                dcc_app_listener_validator_width(item->kind),
                listener->args_size
            ) ||
            (item->kind == DCC_LISTENER_VALIDATE_STRING_LENGTH &&
             item->max_length != 0U && item->min_length > item->max_length) ||
            (item->kind == DCC_LISTENER_VALIDATE_INTEGER_RANGE &&
             item->min_integer > item->max_integer) ||
            (item->kind == DCC_LISTENER_VALIDATE_NUMBER_RANGE &&
             (item->min_number != item->min_number ||
              item->max_number != item->max_number ||
              item->min_number > item->max_number))) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_validate_command_schema(
    dcc_listener_kind_t kind,
    const dcc_application_command_builder_t *command
) {
    if (command == NULL) {
        return DCC_OK;
    }
    if (dcc_app_listener_is_component_kind(kind) ||
        command->has_name != 1U || command->name == NULL || command->name[0] == '\0' ||
        command->has_description > 1U || command->has_type > 1U ||
        command->has_default_member_permissions > 1U ||
        command->default_member_permissions_null > 1U ||
        command->has_dm_permission > 1U || command->dm_permission > 1U ||
        command->has_nsfw > 1U || command->nsfw > 1U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_application_command_type_t expected_type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    if (kind == DCC_LISTENER_USER_CONTEXT_MENU) {
        expected_type = DCC_APPLICATION_COMMAND_USER;
    } else if (kind == DCC_LISTENER_MESSAGE_CONTEXT_MENU) {
        expected_type = DCC_APPLICATION_COMMAND_MESSAGE;
    }
    uint32_t actual_type = dcc_command_registry_builder_type(command);
    if (actual_type != (uint32_t)expected_type) {
        return DCC_ERR_INVALID_ARG;
    }
    if (expected_type == DCC_APPLICATION_COMMAND_CHAT_INPUT) {
        return command->has_description == 1U && command->description != NULL &&
                       command->description[0] != '\0'
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    return command->has_description == 0U && command->description == NULL &&
                   command->description_localizations_json == NULL &&
                   command->options_json == NULL && command->options == NULL &&
                   command->options_count == 0U
        ? DCC_OK
        : DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_listener_validate_target(const dcc_listener_t *listener) {
    if (dcc_app_listener_is_route_kind(listener->kind)) {
        const dcc_listener_route_target_t *target = &listener->target.route;
        if (!dcc_app_listener_has_full_value(
                target->size, target->version, sizeof(*target), DCC_LISTENER_TARGET_VERSION
            )) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t command_status = dcc_app_listener_validate_command_schema(
            listener->kind,
            target->command
        );
        if (command_status != DCC_OK) {
            return command_status;
        }
        if (listener->kind == DCC_LISTENER_SUBCOMMAND) {
            return target->name == NULL &&
                           target->command_name != NULL && target->command_name[0] != '\0' &&
                           target->subcommand_path != NULL && target->subcommand_path[0] != '\0' &&
                           ((target->command == NULL && target->description != NULL &&
                             target->description[0] != '\0') ||
                            (target->command != NULL && target->description == NULL))
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        if (listener->kind == DCC_LISTENER_AUTOCOMPLETE &&
            (target->command_name != NULL || target->subcommand_path != NULL)) {
            return target->name == NULL &&
                           target->command_name != NULL && target->command_name[0] != '\0' &&
                           target->subcommand_path != NULL && target->subcommand_path[0] != '\0' &&
                           ((target->command == NULL && target->description != NULL &&
                             target->description[0] != '\0') ||
                            (target->command != NULL && target->description == NULL))
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        if (target->command_name != NULL || target->subcommand_path != NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (dcc_app_listener_is_component_kind(listener->kind)) {
            return target->command == NULL && target->description == NULL &&
                           target->name != NULL && target->name[0] != '\0'
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        if (listener->kind == DCC_LISTENER_USER_CONTEXT_MENU ||
            listener->kind == DCC_LISTENER_MESSAGE_CONTEXT_MENU) {
            if (target->description != NULL) {
                return DCC_ERR_INVALID_ARG;
            }
        }
        if (target->command != NULL) {
            return target->name == NULL && target->description == NULL &&
                           target->command->name != NULL && target->command->name[0] != '\0'
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        return target->name != NULL && target->name[0] != '\0' &&
                       ((listener->kind != DCC_LISTENER_SLASH &&
                         listener->kind != DCC_LISTENER_AUTOCOMPLETE) ||
                        (target->description != NULL && target->description[0] != '\0'))
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (listener->kind == DCC_LISTENER_EVENT || listener->kind == DCC_LISTENER_READY ||
        (listener->kind >= DCC_LISTENER_MESSAGE_CREATE &&
         listener->kind <= DCC_LISTENER_MESSAGE_DELETE)) {
        const dcc_listener_event_target_t *target = &listener->target.event;
        if (!dcc_app_listener_has_full_value(
                target->size, target->version, sizeof(*target), DCC_LISTENER_TARGET_VERSION
            ) ||
            target->once > 1U) {
            return DCC_ERR_INVALID_ARG;
        }
        if (listener->kind == DCC_LISTENER_EVENT) {
            return target->type >= 0 && target->type < DCC_EVENT_MAX
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        return target->type == DCC_EVENT_READY ? DCC_OK : DCC_ERR_INVALID_ARG;
    }
    if (listener->kind == DCC_LISTENER_MESSAGE_COMMAND) {
        const dcc_listener_message_command_target_t *target = &listener->target.message_command;
        return dcc_app_listener_has_full_value(
                   target->size, target->version, sizeof(*target), DCC_LISTENER_TARGET_VERSION
               ) &&
                       target->prefix != NULL && target->prefix[0] != '\0' &&
                       target->name != NULL && target->name[0] != '\0'
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (listener->kind == DCC_LISTENER_TASK) {
        const dcc_listener_schedule_target_t *target = &listener->target.schedule;
        if (!dcc_app_listener_has_full_value(
                target->size, target->version, sizeof(*target), DCC_LISTENER_TARGET_VERSION
            )) {
            return DCC_ERR_INVALID_ARG;
        }
        if (target->kind == DCC_LISTENER_SCHEDULE_INTERVAL) {
            return target->interval_ms != 0U && target->hour == 0U && target->minute == 0U
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        }
        return target->kind == DCC_LISTENER_SCHEDULE_DAILY_KST &&
                       target->interval_ms == 0U && target->hour <= 23U && target->minute <= 59U
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    return DCC_ERR_INVALID_ARG;
}

static uint8_t dcc_app_listener_handler_is_set(const dcc_listener_t *listener) {
    if (dcc_app_listener_is_route_kind(listener->kind)) {
        return listener->args_size == 0U
            ? listener->handler.plain != NULL
            : listener->handler.typed != NULL;
    }
    switch (listener->kind) {
        case DCC_LISTENER_EVENT:
            return listener->handler.event != NULL;
        case DCC_LISTENER_READY:
            return listener->handler.ready != NULL;
        case DCC_LISTENER_MESSAGE_CREATE:
        case DCC_LISTENER_MESSAGE_UPDATE:
        case DCC_LISTENER_MESSAGE_DELETE:
            return listener->handler.message != NULL;
        case DCC_LISTENER_MESSAGE_COMMAND:
            return listener->handler.message_command != NULL;
        case DCC_LISTENER_TASK:
            return listener->handler.task != NULL;
        default:
            return 0U;
    }
}

static dcc_status_t dcc_app_listener_validate(const dcc_listener_t *listener) {
    if (listener == NULL || listener->size < sizeof(*listener) ||
        listener->version != DCC_LISTENER_VERSION ||
        listener->kind <= DCC_LISTENER_NONE || listener->kind > DCC_LISTENER_TASK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_app_listener_validate_policy(&listener->policy);
    if (status == DCC_OK) {
        status = dcc_app_listener_validate_bindings(listener);
    }
    if (status == DCC_OK) {
        status = dcc_app_listener_validate_target(listener);
    }
    if (status != DCC_OK) {
        return status;
    }
    if (!dcc_app_listener_handler_is_set(listener)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_app_listener_is_route_kind(listener->kind) &&
        (!dcc_app_listener_policy_is_empty(&listener->policy) ||
         listener->args_size != 0U ||
         listener->bindings.kind != DCC_LISTENER_BIND_NONE ||
         listener->bindings.items.options != NULL || listener->bindings.count != 0U ||
         listener->validators.items != NULL || listener->validators.count != 0U ||
         listener->validation.suppress_response != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_copy_bindings(
    dcc_app_listener_entry_t *entry,
    const dcc_listener_t *listener
) {
    if (listener->args_size == 0U) {
        return DCC_OK;
    }
    entry->args_size = listener->args_size;
    entry->binding_kind = listener->bindings.kind;
    entry->binding_count = listener->bindings.count;
    entry->suppress_validation_response = listener->validation.suppress_response;
    size_t count = listener->bindings.count;
    const dcc_listener_binding_t *items = listener->bindings.items.options;
    if (entry->binding_kind == DCC_LISTENER_BIND_OPTIONS) {
        entry->option_bindings = (dcc_ctx_option_field_binding_t *)calloc(
            count,
            sizeof(*entry->option_bindings)
        );
        if (entry->option_bindings == NULL) {
            return DCC_ERR_NOMEM;
        }
        for (size_t i = 0U; i < count; ++i) {
            entry->option_bindings[i] = (dcc_ctx_option_field_binding_t){
                .size = sizeof(entry->option_bindings[i]),
                .type = (dcc_ctx_option_bind_type_t)items[i].type,
                .field_offset = items[i].field_offset, .required = items[i].required,
                .fallback_integer = items[i].fallback_integer,
                .fallback_number = items[i].fallback_number,
                .fallback_boolean = items[i].fallback_boolean,
                .fallback_snowflake = items[i].fallback_snowflake,
            };
            dcc_status_t status = dcc_app_listener_copy_string(
                items[i].name,
                &entry->option_bindings[i].name
            );
            if (status == DCC_OK) {
                status = dcc_app_listener_copy_string(
                    items[i].fallback_string,
                    &entry->option_bindings[i].fallback_string
                );
            }
            if (status != DCC_OK) {
                return status;
            }
        }
    } else if (entry->binding_kind == DCC_LISTENER_BIND_FORM) {
        entry->form_bindings = (dcc_ctx_form_field_binding_t *)calloc(
            count,
            sizeof(*entry->form_bindings)
        );
        if (entry->form_bindings == NULL) {
            return DCC_ERR_NOMEM;
        }
        for (size_t i = 0U; i < count; ++i) {
            entry->form_bindings[i] = (dcc_ctx_form_field_binding_t){
                .size = sizeof(entry->form_bindings[i]),
                .type = (dcc_ctx_form_bind_type_t)items[i].type,
                .field_offset = items[i].field_offset, .count_offset = items[i].count_offset,
                .has_count_offset = items[i].has_count_offset, .required = items[i].required,
                .fallback_boolean = items[i].fallback_boolean,
            };
            dcc_status_t status = dcc_app_listener_copy_string(
                items[i].name,
                &entry->form_bindings[i].custom_id
            );
            if (status == DCC_OK) {
                status = dcc_app_listener_copy_string(
                    items[i].fallback_string,
                    &entry->form_bindings[i].fallback_string
                );
            }
            if (status == DCC_OK) {
                status = dcc_app_listener_copy_string_array(
                    entry->app,
                    items[i].fallback_values,
                    items[i].fallback_values_count,
                    &entry->form_bindings[i].fallback_values
                );
            }
            if (status == DCC_OK) {
                entry->form_bindings[i].fallback_values_count =
                    items[i].fallback_values_count;
            }
            if (status != DCC_OK) {
                return status;
            }
        }
    } else {
        entry->component_bindings = (dcc_ctx_component_field_binding_t *)calloc(
            count,
            sizeof(*entry->component_bindings)
        );
        if (entry->component_bindings == NULL) {
            return DCC_ERR_NOMEM;
        }
        for (size_t i = 0U; i < count; ++i) {
            entry->component_bindings[i] = (dcc_ctx_component_field_binding_t){
                .size = sizeof(entry->component_bindings[i]),
                .type = (dcc_ctx_component_bind_type_t)items[i].type,
                .field_offset = items[i].field_offset, .count_offset = items[i].count_offset,
                .has_count_offset = items[i].has_count_offset, .required = items[i].required,
                .fallback_component_type = items[i].fallback_component_type,
            };
            dcc_status_t status = dcc_app_listener_copy_string(
                items[i].custom_id_prefix,
                &entry->component_bindings[i].custom_id_prefix
            );
            if (status == DCC_OK) {
                status = dcc_app_listener_copy_string(
                    items[i].fallback_string,
                    &entry->component_bindings[i].fallback_string
                );
            }
            if (status == DCC_OK) {
                status = dcc_app_listener_copy_string_array(
                    entry->app,
                    items[i].fallback_values,
                    items[i].fallback_values_count,
                    &entry->component_bindings[i].fallback_values
                );
            }
            if (status == DCC_OK) {
                entry->component_bindings[i].fallback_values_count =
                    items[i].fallback_values_count;
            }
            if (status != DCC_OK) {
                return status;
            }
        }
    }

    if (listener->validators.count == 0U) {
        return DCC_OK;
    }
    entry->validators = (dcc_ctx_field_validator_t *)calloc(
        listener->validators.count,
        sizeof(*entry->validators)
    );
    if (entry->validators == NULL) {
        return DCC_ERR_NOMEM;
    }
    entry->validator_count = listener->validators.count;
    for (size_t i = 0U; i < entry->validator_count; ++i) {
        const dcc_listener_validator_t *item = &listener->validators.items[i];
        entry->validators[i] = (dcc_ctx_field_validator_t){
            .size = sizeof(entry->validators[i]),
            .type = (dcc_ctx_field_validate_type_t)item->kind,
            .field_offset = item->field_offset, .min_length = item->min_length,
            .max_length = item->max_length, .min_integer = item->min_integer,
            .max_integer = item->max_integer, .min_number = item->min_number,
            .max_number = item->max_number,
        };
        dcc_status_t status = dcc_app_listener_copy_string(
            item->field,
            &entry->validators[i].field
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_bind_args(
    dcc_app_listener_entry_t *entry,
    dcc_ctx_t *ctx,
    void *args
) {
    if (entry->binding_kind == DCC_LISTENER_BIND_OPTIONS) {
        return dcc_ctx_bind_option_fields(ctx, args, entry->option_bindings, entry->binding_count);
    }
    if (entry->binding_kind == DCC_LISTENER_BIND_FORM) {
        return dcc_ctx_bind_form_fields(ctx, args, entry->form_bindings, entry->binding_count);
    }
    return dcc_ctx_bind_component_fields(
        ctx,
        args,
        entry->component_bindings,
        entry->binding_count
    );
}

static dcc_status_t dcc_app_listener_route_dispatch(dcc_ctx_t *ctx, void *user_data) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)user_data;
    if (entry == NULL) {
        return DCC_OK;
    }
    dcc_app_callback_frame_t frame;
    dcc_app_callback_frame_enter(&frame, entry->app, entry);
    if (entry->args_size == 0U) {
        dcc_status_t status = entry->handler.plain(ctx, entry->user_data);
        dcc_app_callback_frame_leave(&frame);
        return status;
    }

    void *args = calloc(1U, entry->args_size);
    if (args == NULL) {
        dcc_app_callback_frame_leave(&frame);
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_listener_bind_args(entry, ctx, args);
    dcc_ctx_validation_error_t error;
    memset(&error, 0, sizeof(error));
    uint8_t validated = 0U;
    if (status == DCC_OK) {
        validated = 1U;
        status = dcc_ctx_validate_fields(
            args,
            entry->validators,
            entry->validator_count,
            &error
        );
    }
    if (status != DCC_OK && !entry->suppress_validation_response) {
        if (entry->kind == DCC_LISTENER_AUTOCOMPLETE) {
            dcc_autocomplete_builder_t autocomplete;
            dcc_autocomplete_builder_init(&autocomplete);
            (void)dcc_ctx_reply_autocomplete(ctx, &autocomplete, NULL, NULL);
        } else if (validated) {
            (void)dcc_ctx_reply_validation_result(ctx, status, &error, NULL, NULL);
        } else {
            (void)dcc_ctx_reply_validation_error(ctx, status, NULL, NULL);
        }
    }
    if (status == DCC_OK) {
        status = entry->handler.typed(ctx, args, entry->user_data);
    }
    free(args);
    dcc_app_callback_frame_leave(&frame);
    return status;
}

static dcc_status_t dcc_app_listener_apply_policy(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_listener_route_policy_t *policy
) {
    dcc_app_extension_middleware_t *middlewares = NULL;
    dcc_app_check_t *checks = NULL;
    if (app->listener_test_fail_policy_allocation) {
        app->listener_test_fail_policy_allocation = 0U;
        return DCC_ERR_NOMEM;
    }
    if (policy->middleware_count != 0U) {
        middlewares = (dcc_app_extension_middleware_t *)calloc(
            policy->middleware_count,
            sizeof(*middlewares)
        );
        if (middlewares == NULL) {
            return DCC_ERR_NOMEM;
        }
        for (size_t i = 0U; i < policy->middleware_count; ++i) {
            middlewares[i] = (dcc_app_extension_middleware_t){
                .size = sizeof(middlewares[i]),
                .middleware = policy->middlewares[i].callback,
                .user_data = policy->middlewares[i].user_data,
            };
        }
    }
    if (policy->check_count != 0U) {
        checks = (dcc_app_check_t *)calloc(policy->check_count, sizeof(*checks));
        if (checks == NULL) {
            free(middlewares);
            return DCC_ERR_NOMEM;
        }
        for (size_t i = 0U; i < policy->check_count; ++i) {
            checks[i] = (dcc_app_check_t){
                .size = sizeof(checks[i]), .check = policy->checks[i].callback,
                .user_data = policy->checks[i].user_data,
                .title = policy->checks[i].title,
                .description = policy->checks[i].description,
            };
        }
    }
    dcc_app_route_policy_t legacy = {
        .size = sizeof(legacy), .middlewares = middlewares,
        .middleware_count = policy->middleware_count,
        .owner_user_ids = policy->owner_user_ids,
        .owner_user_id_count = policy->owner_user_id_count,
        .required_permissions = policy->required_permissions,
        .guild_only = policy->guild_only,
        .checks = checks, .check_count = policy->check_count,
        .dm_only = policy->dm_only, .nsfw_only = policy->nsfw_only,
        .channel_types = policy->channel_types,
        .channel_type_count = policy->channel_type_count,
        .required_role_ids = policy->required_role_ids,
        .required_role_id_count = policy->required_role_id_count,
        .any_role_ids = policy->any_role_ids,
        .any_role_id_count = policy->any_role_id_count,
    };
    if (policy->cooldown.bucket != DCC_LISTENER_COOLDOWN_NONE) {
        legacy.cooldown = (dcc_app_cooldown_options_t){
            .size = sizeof(legacy.cooldown),
            .bucket = (dcc_app_cooldown_bucket_t)policy->cooldown.bucket,
            .limit = policy->cooldown.limit,
            .window_ms = policy->cooldown.window_ms,
        };
    }
    dcc_status_t status = dcc_app_route_apply_policy_internal(app, route_id, &legacy);
    free(middlewares);
    free(checks);
    return status;
}

static dcc_status_t dcc_app_listener_prepare_command_schema(
    dcc_app_t *app,
    const dcc_listener_t *listener,
    dcc_command_registry_add_transaction_t *transaction
) {
    const dcc_listener_route_target_t *target = &listener->target.route;
    if (dcc_app_listener_is_component_kind(listener->kind)) {
        return DCC_OK;
    }
    if (target->command != NULL) {
        return dcc_command_registry_add_prepare(transaction, &app->registry, target->command);
    }
    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    const char *command_name = listener->kind == DCC_LISTENER_SUBCOMMAND ||
            (listener->kind == DCC_LISTENER_AUTOCOMPLETE && target->command_name != NULL)
        ? target->command_name
        : target->name;
    dcc_status_t status = dcc_application_command_builder_set_name(&command, command_name);
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_type(
            &command,
            listener->kind == DCC_LISTENER_USER_CONTEXT_MENU
                ? DCC_APPLICATION_COMMAND_USER
                : listener->kind == DCC_LISTENER_MESSAGE_CONTEXT_MENU
                    ? DCC_APPLICATION_COMMAND_MESSAGE
                    : DCC_APPLICATION_COMMAND_CHAT_INPUT
        );
    }
    if (status == DCC_OK &&
        (listener->kind == DCC_LISTENER_SLASH ||
         listener->kind == DCC_LISTENER_SUBCOMMAND ||
         listener->kind == DCC_LISTENER_AUTOCOMPLETE)) {
        status = dcc_application_command_builder_set_description(
            &command,
            target->description
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_add_prepare(transaction, &app->registry, &command);
    }
    return status;
}

static dcc_status_t dcc_app_listener_route_key(
    const dcc_listener_t *listener,
    const char **out_key,
    char *key_buffer,
    size_t key_buffer_size
) {
    const dcc_listener_route_target_t *target = &listener->target.route;
    if (listener->kind == DCC_LISTENER_SUBCOMMAND ||
        (listener->kind == DCC_LISTENER_AUTOCOMPLETE && target->command_name != NULL)) {
        dcc_status_t status = dcc_app_subcommand_key(
            target->command_name,
            target->subcommand_path,
            key_buffer,
            key_buffer_size
        );
        if (status == DCC_OK) {
            *out_key = key_buffer;
        }
        return status;
    }
    *out_key = target->command != NULL ? target->command->name : target->name;
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_register_route(
    dcc_app_listener_entry_t *entry,
    const dcc_listener_t *listener
) {
    char key_buffer[256];
    const char *key = NULL;
    dcc_status_t status = dcc_app_listener_route_key(
        listener,
        &key,
        key_buffer,
        sizeof(key_buffer)
    );
    dcc_app_route_id_t previous_route_id = entry->app->next_route_id;
    if (status == DCC_OK) {
        status = dcc_app_add_canonical_route_with_cleanup(
            entry->app,
            dcc_app_listener_route_event(listener->kind),
            key,
            dcc_app_listener_route_prefix(listener->kind),
            dcc_app_listener_route_dispatch,
            entry,
            entry->user_data,
            entry,
            NULL,
            &entry->route_id
        );
    }
    if (status == DCC_OK) {
        status = dcc_app_listener_apply_policy(entry->app, entry->route_id, &listener->policy);
    }
    if (status != DCC_OK && entry->route_id != DCC_APP_ROUTE_INVALID) {
        dcc_app_route_id_t failed_route_id = entry->route_id;
        (void)dcc_app_remove_route_internal(entry->app, entry->route_id);
        entry->route_id = DCC_APP_ROUTE_INVALID;
        dcc_app_listener_lock(entry->app);
        if (entry->app->next_route_id == failed_route_id) {
            entry->app->next_route_id = previous_route_id;
        }
        dcc_app_listener_unlock(entry->app);
    }
    return status;
}

static void dcc_app_listener_report_status(
    dcc_app_listener_entry_t *entry,
    dcc_client_t *client,
    const dcc_event_t *event,
    dcc_status_t status
) {
    if (entry == NULL || status == DCC_OK) {
        return;
    }
    dcc_ctx_t ctx = {
        .app = entry->app,
        .client = client,
        .event = event,
        .interaction = event != NULL ? dcc_event_interaction(event) : NULL,
        .user_data = entry->user_data,
    };
    if (ctx.interaction != NULL) {
        dcc_flow_init(&ctx.flow, client, ctx.interaction);
    }
    if (entry->app->error_handler != NULL) {
        entry->app->error_handler(
            &ctx,
            status,
            dcc_status_string(status),
            entry->app->error_user_data
        );
    } else if (ctx.interaction != NULL) {
        (void)dcc_ctx_handle_error(&ctx, status, dcc_status_string(status));
    }
}

static const char *dcc_app_listener_message_command_args(
    const dcc_app_listener_entry_t *entry,
    const char *content
) {
    if (entry == NULL || content == NULL || entry->message_command_text == NULL ||
        strncmp(content, entry->message_command_text, entry->prefix_len) != 0) {
        return NULL;
    }
    const char *name = entry->message_command_text + entry->prefix_len + 1U;
    const char *cursor = content + entry->prefix_len;
    if (strncmp(cursor, name, entry->name_len) != 0) {
        return NULL;
    }
    cursor += entry->name_len;
    if (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '\r' &&
        *cursor != '\n') {
        return NULL;
    }
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n') {
        cursor++;
    }
    return cursor;
}

static void dcc_app_listener_event_dispatch(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)user_data;
    if (entry == NULL || event == NULL || !dcc_app_listener_acquire(entry)) {
        return;
    }
    dcc_app_callback_frame_t frame;
    dcc_app_callback_frame_enter(&frame, entry->app, entry);
    dcc_status_t status = DCC_OK;
    if (entry->kind == DCC_LISTENER_EVENT) {
        status = entry->handler.event(entry->app, event, entry->user_data);
    } else if (entry->kind == DCC_LISTENER_READY) {
        const dcc_ready_event_t *ready = dcc_event_ready(event);
        if (ready != NULL) {
            status = entry->handler.ready(entry->app, ready, entry->user_data);
        }
    } else if (entry->kind >= DCC_LISTENER_MESSAGE_CREATE &&
               entry->kind <= DCC_LISTENER_MESSAGE_DELETE) {
        const dcc_message_t *message = dcc_event_message(event);
        if (message != NULL) {
            status = entry->handler.message(entry->app, message, event, entry->user_data);
        }
    } else if (entry->kind == DCC_LISTENER_MESSAGE_COMMAND) {
        const dcc_message_t *message = dcc_event_message(event);
        const char *args = message != NULL
            ? dcc_app_listener_message_command_args(entry, message->content)
            : NULL;
        if (args != NULL) {
            status = entry->handler.message_command(
                entry->app,
                message,
                args,
                event,
                entry->user_data
            );
        }
    }
    dcc_app_listener_report_status(entry, client, event, status);
    dcc_app_callback_frame_leave(&frame);
    dcc_app_listener_release(entry);
}

static dcc_status_t dcc_app_listener_register_event(
    dcc_app_listener_entry_t *entry,
    const dcc_listener_t *listener
) {
    if (listener->kind == DCC_LISTENER_EVENT) {
        entry->event_type = listener->target.event.type;
    } else if (listener->kind == DCC_LISTENER_READY) {
        entry->event_type = DCC_EVENT_READY;
    } else if (listener->kind == DCC_LISTENER_MESSAGE_CREATE ||
               listener->kind == DCC_LISTENER_MESSAGE_COMMAND) {
        entry->event_type = DCC_EVENT_MESSAGE_CREATE;
    } else if (listener->kind == DCC_LISTENER_MESSAGE_UPDATE) {
        entry->event_type = DCC_EVENT_MESSAGE_UPDATE;
    } else {
        entry->event_type = DCC_EVENT_MESSAGE_DELETE;
    }
    entry->once = listener->kind == DCC_LISTENER_MESSAGE_COMMAND
        ? 0U
        : listener->target.event.once;
    return dcc_client_on(
        entry->app->client,
        entry->event_type,
        dcc_app_listener_event_dispatch,
        entry,
        &entry->client_listener_id
    );
}

static dcc_status_t dcc_app_listener_copy_message_command(
    dcc_app_listener_entry_t *entry,
    const dcc_listener_message_command_target_t *target
) {
    entry->prefix_len = strlen(target->prefix);
    entry->name_len = strlen(target->name);
    if (entry->name_len > SIZE_MAX - 2U ||
        entry->prefix_len > SIZE_MAX - entry->name_len - 2U) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < entry->name_len; ++i) {
        if (target->name[i] == ' ' || target->name[i] == '\t' ||
            target->name[i] == '\r' || target->name[i] == '\n') {
            return DCC_ERR_INVALID_ARG;
        }
    }
    entry->message_command_text = (char *)malloc(entry->prefix_len + entry->name_len + 2U);
    if (entry->message_command_text == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(entry->message_command_text, target->prefix, entry->prefix_len + 1U);
    memcpy(
        entry->message_command_text + entry->prefix_len + 1U,
        target->name,
        entry->name_len + 1U
    );
    return DCC_OK;
}

static dcc_status_t dcc_app_listener_register_task(
    dcc_app_listener_entry_t *entry,
    const dcc_listener_t *listener
) {
    const dcc_listener_schedule_target_t *target = &listener->target.schedule;
    return dcc_app_add_canonical_schedule(
        entry->app,
        target->kind == DCC_LISTENER_SCHEDULE_INTERVAL
            ? DCC_APP_SCHEDULE_INTERVAL
            : DCC_APP_SCHEDULE_DAILY_KST,
        target->interval_ms,
        target->hour,
        target->minute,
        dcc_app_listener_run_task,
        entry,
        &entry->schedule
    );
}

dcc_status_t dcc_app_listen(
    dcc_app_t *app,
    const dcc_listener_t *listener,
    dcc_listener_id_t *out_id
) {
    if (out_id != NULL) {
        *out_id = 0U;
    }
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_app_listener_validate(listener);
    if (status != DCC_OK) {
        return status;
    }
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)calloc(1U, sizeof(*entry));
    if (entry == NULL) {
        return DCC_ERR_NOMEM;
    }
    entry->app = app;
    entry->kind = listener->kind;
    entry->handler = listener->handler;
    entry->user_data = listener->user_data;
    entry->cleanup = listener->cleanup;
    entry->route_id = DCC_APP_ROUTE_INVALID;

    status = dcc_app_listener_copy_bindings(entry, listener);
    if (status == DCC_OK && listener->kind == DCC_LISTENER_MESSAGE_COMMAND) {
        status = dcc_app_listener_copy_message_command(entry, &listener->target.message_command);
    }
    if (status != DCC_OK) {
        dcc_app_listener_entry_free(entry);
        return status;
    }

    dcc_app_listener_array_stage_t listener_stage;
    dcc_app_route_array_stage_t route_stage;
    dcc_app_schedule_array_stage_t schedule_stage;
    dcc_command_registry_add_transaction_t command_transaction;
    memset(&listener_stage, 0, sizeof(listener_stage));
    memset(&route_stage, 0, sizeof(route_stage));
    memset(&schedule_stage, 0, sizeof(schedule_stage));
    memset(&command_transaction, 0, sizeof(command_transaction));
    dcc_app_listener_lock(app);
    if (app->listener_destroying || app->listener_count == SIZE_MAX ||
        app->next_listener_id == UINT64_MAX) {
        status = app->listener_destroying ? DCC_ERR_STATE : DCC_ERR_NOMEM;
    } else if (dcc_app_listener_is_route_kind(listener->kind)) {
        status = dcc_app_listener_prepare_command_schema(
            app,
            listener,
            &command_transaction
        );
    }
    if (status == DCC_OK) {
        status = dcc_app_listener_stage_listener_array(app, &listener_stage);
    }
    if (status == DCC_OK && dcc_app_listener_is_route_kind(listener->kind)) {
        status = dcc_app_listener_stage_route_array(app, &route_stage);
    } else if (status == DCC_OK && listener->kind == DCC_LISTENER_TASK) {
        status = dcc_app_listener_stage_schedule_array(app, &schedule_stage);
    }
    if (status == DCC_OK && dcc_app_listener_is_route_kind(listener->kind)) {
        status = dcc_app_listener_register_route(entry, listener);
    } else if (status == DCC_OK && listener->kind >= DCC_LISTENER_EVENT &&
               listener->kind <= DCC_LISTENER_MESSAGE_COMMAND) {
        status = dcc_app_listener_register_event(entry, listener);
    } else if (status == DCC_OK) {
        status = dcc_app_listener_register_task(entry, listener);
    }
    if (status == DCC_OK) {
        dcc_command_registry_add_commit(&command_transaction);
        entry->id = ++app->next_listener_id;
        entry->active = 1U;
        app->listeners[app->listener_count++] = entry;
    }
    if (status != DCC_OK) {
        dcc_command_registry_add_abort(&command_transaction);
    }
    dcc_app_listener_finish_schedule_array(app, &schedule_stage, status == DCC_OK);
    dcc_app_listener_finish_route_array(app, &route_stage, status == DCC_OK);
    dcc_app_listener_finish_listener_array(app, &listener_stage, status == DCC_OK);
    dcc_app_listener_unlock(app);
    if (status != DCC_OK) {
        dcc_app_listener_entry_free(entry);
        return status;
    }
    if (out_id != NULL) {
        *out_id = entry->id;
    }
    return DCC_OK;
}

dcc_status_t dcc_app_unlisten(dcc_app_t *app, dcc_listener_id_t id) {
    if (app == NULL || id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_entry_t *entry = NULL;
    dcc_app_listener_lock(app);
    for (size_t i = 0U; i < app->listener_count; ++i) {
        if (app->listeners[i] != NULL && app->listeners[i]->id == id) {
            entry = app->listeners[i];
            break;
        }
    }
    if (entry == NULL || !entry->active) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_INVALID_ARG;
    }
    entry->active = 0U;
    dcc_app_listener_retire_locked(app, entry);
    uint8_t self_unlisten = dcc_app_listener_is_current(entry);
    dcc_app_listener_unlock(app);

    dcc_status_t status = dcc_app_listener_detach_source(
        entry,
        self_unlisten ? 0U : 1U
    );
    if (self_unlisten) {
        return status == DCC_OK ? DCC_OK : DCC_ERR_STATE;
    }
    dcc_app_listener_lock(app);
    while (entry->in_flight != 0U) {
        dcc_app_listener_wait(app);
    }
    dcc_app_listener_unlock(app);
    status = dcc_app_listener_finalize(entry, 1U);
    return status == DCC_OK ? DCC_OK : DCC_ERR_STATE;
}

void dcc_app_listener_destroy_all(dcc_app_t *app) {
    if (app == NULL) {
        return;
    }
    dcc_app_listener_lock(app);
    app->listener_destroying = 1U;
    while (app->listener_count != 0U) {
        dcc_app_listener_entry_t *entry = app->listeners[app->listener_count - 1U];
        entry->active = 0U;
        dcc_app_listener_retire_locked(app, entry);
    }
    dcc_app_listener_unlock(app);

    for (dcc_app_listener_entry_t *entry = app->retired_listeners;
         entry != NULL;
         entry = entry->next_retired) {
        (void)dcc_app_listener_detach_source(entry, 1U);
    }
    for (dcc_app_listener_entry_t *entry = app->retired_listeners;
         entry != NULL;
         entry = entry->next_retired) {
        dcc_app_listener_lock(app);
        while (entry->in_flight != 0U) {
            dcc_app_listener_wait(app);
        }
        dcc_app_listener_unlock(app);
        (void)dcc_app_listener_finalize(entry, 1U);
    }
    free(app->listeners);
    app->listeners = NULL;
    app->listener_count = 0U;
    app->listener_cap = 0U;
}

void dcc_app_listener_reclaim_all(dcc_app_t *app) {
    if (app == NULL) {
        return;
    }
    dcc_app_listener_lock(app);
    dcc_app_listener_entry_t *entry = app->retired_listeners;
    app->retired_listeners = NULL;
    dcc_app_listener_unlock(app);
    while (entry != NULL) {
        dcc_app_listener_entry_t *next = entry->next_retired;
        dcc_app_listener_entry_free(entry);
        entry = next;
    }
}

uint8_t dcc_app_listener_active(const void *listener_state) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)listener_state;
    if (entry == NULL || entry->app == NULL) {
        return 0U;
    }
    dcc_app_listener_lock(entry->app);
    uint8_t active = entry->active;
    dcc_app_listener_unlock(entry->app);
    return active;
}

dcc_status_t dcc_app_listener_run_task(void *listener_state, dcc_app_t *app) {
    dcc_app_listener_entry_t *entry = (dcc_app_listener_entry_t *)listener_state;
    if (entry == NULL || app == NULL || !dcc_app_listener_acquire(entry)) {
        return DCC_OK;
    }
    dcc_app_callback_frame_t frame;
    dcc_app_callback_frame_enter(&frame, entry->app, entry);
    dcc_status_t status = entry->handler.task(app, entry->user_data);
    dcc_app_listener_report_status(entry, app->client, NULL, status);
    dcc_app_callback_frame_leave(&frame);
    dcc_app_listener_release(entry);
    return status;
}
