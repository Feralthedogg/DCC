#include <dcc/app.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <sched.h>
#include <time.h>
#endif

typedef int (*test_thread_fn)(void *arg);

typedef struct test_thread {
    test_thread_fn fn;
    void *arg;
    int result;
#if defined(_WIN32)
    HANDLE handle;
#else
    pthread_t handle;
#endif
} test_thread_t;

static uint64_t test_now_ms(void) {
#if defined(_WIN32)
    return (uint64_t)GetTickCount64();
#else
    struct timespec now;
    (void)clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * UINT64_C(1000) + (uint64_t)now.tv_nsec / UINT64_C(1000000);
#endif
}

static void test_yield(void) {
#if defined(_WIN32)
    (void)SwitchToThread();
#else
    (void)sched_yield();
#endif
}

static int test_wait_true(atomic_bool *value, unsigned timeout_ms) {
    uint64_t deadline = test_now_ms() + timeout_ms;
    while (!atomic_load_explicit(value, memory_order_acquire)) {
        if (test_now_ms() >= deadline) {
            return 0;
        }
        test_yield();
    }
    return 1;
}

static int test_wait_uint_at_least(atomic_uint *value, unsigned minimum, unsigned timeout_ms) {
    uint64_t deadline = test_now_ms() + timeout_ms;
    while (atomic_load_explicit(value, memory_order_acquire) < minimum) {
        if (test_now_ms() >= deadline) {
            return 0;
        }
        test_yield();
    }
    return 1;
}

#if defined(_WIN32)
static DWORD WINAPI test_thread_trampoline(LPVOID arg) {
    test_thread_t *thread = (test_thread_t *)arg;
    thread->result = thread->fn(thread->arg);
    return 0U;
}
#else
static void *test_thread_trampoline(void *arg) {
    test_thread_t *thread = (test_thread_t *)arg;
    thread->result = thread->fn(thread->arg);
    return NULL;
}
#endif

static int test_thread_start(test_thread_t *thread, test_thread_fn fn, void *arg) {
    memset(thread, 0, sizeof(*thread));
    thread->fn = fn;
    thread->arg = arg;
#if defined(_WIN32)
    thread->handle = CreateThread(NULL, 0U, test_thread_trampoline, thread, 0U, NULL);
    return thread->handle != NULL ? 0 : -1;
#else
    return pthread_create(&thread->handle, NULL, test_thread_trampoline, thread);
#endif
}

static int test_thread_join(test_thread_t *thread) {
#if defined(_WIN32)
    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
        return -1;
    }
    (void)CloseHandle(thread->handle);
#else
    if (pthread_join(thread->handle, NULL) != 0) {
        return -1;
    }
#endif
    return thread->result;
}

typedef enum concurrent_kind {
    CONCURRENT_ROUTE = 1,
    CONCURRENT_EVENT,
    CONCURRENT_TASK
} concurrent_kind_t;

typedef struct concurrent_case {
    dcc_app_t *app;
    dcc_client_t *client;
    dcc_listener_id_t id;
    dcc_app_listener_entry_t *entry;
    dcc_app_schedule_t *schedule;
    concurrent_kind_t kind;
    atomic_bool entered;
    atomic_bool release;
    atomic_bool operation_started;
    atomic_bool operation_done;
    atomic_bool cleanup;
    atomic_bool detach_cleanup_entered;
    atomic_bool detach_cleanup_release;
    atomic_bool detach_remove_entered;
    atomic_bool detach_remove_release;
    atomic_bool middleware_dispatch_entered;
    atomic_bool middleware_dispatch_release;
    atomic_bool dispatch_start;
    atomic_uint dispatch_ready_count;
    atomic_uint handler_count;
    atomic_uint cleanup_count;
    atomic_uint original_route_middleware_count;
    atomic_uint added_route_middleware_count;
    atomic_int self_status;
    atomic_bool cleanup_was_early;
} concurrent_case_t;

static void concurrent_case_init(concurrent_case_t *state, concurrent_kind_t kind) {
    memset(state, 0, sizeof(*state));
    state->kind = kind;
    atomic_init(&state->entered, false);
    atomic_init(&state->release, false);
    atomic_init(&state->operation_started, false);
    atomic_init(&state->operation_done, false);
    atomic_init(&state->cleanup, false);
    atomic_init(&state->detach_cleanup_entered, false);
    atomic_init(&state->detach_cleanup_release, false);
    atomic_init(&state->detach_remove_entered, false);
    atomic_init(&state->detach_remove_release, false);
    atomic_init(&state->middleware_dispatch_entered, false);
    atomic_init(&state->middleware_dispatch_release, false);
    atomic_init(&state->dispatch_start, false);
    atomic_init(&state->dispatch_ready_count, 0U);
    atomic_init(&state->handler_count, 0U);
    atomic_init(&state->cleanup_count, 0U);
    atomic_init(&state->original_route_middleware_count, 0U);
    atomic_init(&state->added_route_middleware_count, 0U);
    atomic_init(&state->self_status, DCC_OK);
    atomic_init(&state->cleanup_was_early, false);
}

static void concurrent_cleanup(void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    (void)atomic_fetch_add_explicit(&state->cleanup_count, 1U, memory_order_relaxed);
    atomic_store_explicit(&state->cleanup, true, memory_order_release);
}

static dcc_status_t concurrent_middleware(dcc_ctx_t *ctx, void *user_data) {
    return ctx != NULL && user_data != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static dcc_status_t blocking_global_middleware(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&state->middleware_dispatch_entered, true, memory_order_release);
    while (!atomic_load_explicit(&state->middleware_dispatch_release, memory_order_acquire)) {
        test_yield();
    }
    return DCC_OK;
}

static dcc_status_t original_route_middleware(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(
        &state->original_route_middleware_count,
        1U,
        memory_order_relaxed
    );
    return DCC_OK;
}

static dcc_status_t added_route_middleware(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(
        &state->added_route_middleware_count,
        1U,
        memory_order_relaxed
    );
    return DCC_OK;
}

static void blocking_middleware_cleanup(void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    atomic_store_explicit(&state->detach_cleanup_entered, true, memory_order_release);
    while (!atomic_load_explicit(&state->detach_cleanup_release, memory_order_acquire)) {
        test_yield();
    }
}

static void blocking_before_route_remove(void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    atomic_store_explicit(&state->detach_remove_entered, true, memory_order_release);
    while (!atomic_load_explicit(&state->detach_remove_release, memory_order_acquire)) {
        test_yield();
    }
}

static void concurrent_handler_enter(concurrent_case_t *state) {
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    atomic_store_explicit(&state->entered, true, memory_order_release);
    while (!atomic_load_explicit(&state->release, memory_order_acquire)) {
        test_yield();
    }
}

static dcc_status_t concurrent_route_handler(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    concurrent_handler_enter(state);
    return DCC_OK;
}

static dcc_status_t self_unlisten_route_handler(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    atomic_store_explicit(
        &state->self_status,
        dcc_app_unlisten(state->app, state->id),
        memory_order_relaxed
    );
    if (atomic_load_explicit(&state->cleanup_count, memory_order_relaxed) != 0U) {
        atomic_store_explicit(&state->cleanup_was_early, true, memory_order_relaxed);
    }
    return DCC_OK;
}

static dcc_status_t concurrent_event_handler(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (app == NULL || event == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    concurrent_handler_enter(state);
    return DCC_OK;
}

static dcc_status_t once_event_handler(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (app == NULL || event == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    return DCC_OK;
}

static dcc_status_t concurrent_task_handler(dcc_app_t *app, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (app == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    concurrent_handler_enter(state);
    return DCC_OK;
}

static dcc_status_t self_destroy_route_handler(dcc_ctx_t *ctx, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    atomic_store_explicit(
        &state->self_status,
        dcc_app_destroy(state->app),
        memory_order_release
    );
    return DCC_OK;
}

static dcc_status_t self_destroy_event_handler(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (app == NULL || event == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    atomic_store_explicit(&state->self_status, dcc_app_destroy(app), memory_order_release);
    return DCC_OK;
}

static dcc_status_t self_destroy_task_handler(dcc_app_t *app, void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    if (app == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)atomic_fetch_add_explicit(&state->handler_count, 1U, memory_order_relaxed);
    atomic_store_explicit(&state->self_status, dcc_app_destroy(app), memory_order_release);
    return DCC_OK;
}

static int dispatch_route(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.data.interaction.name = "concurrent";
    event.data.interaction.token = "token";
    event.data.interaction.id = 1U;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(
        &dcc_app_client(state->app)->events,
        dcc_app_client(state->app),
        &event
    ) == DCC_OK ? 0 : 1;
}

static int dispatch_autocomplete(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_AUTOCOMPLETE;
    event.data.interaction.name = "concurrent";
    event.data.interaction.token = "token";
    event.data.interaction.id = 1U;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(
        &dcc_app_client(state->app)->events,
        dcc_app_client(state->app),
        &event
    ) == DCC_OK ? 0 : 1;
}

static int dispatch_event(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_LOG;
    return dcc_event_bus_dispatch(
        &dcc_app_client(state->app)->events,
        dcc_app_client(state->app),
        &event
    ) == DCC_OK ? 0 : 1;
}

static int dispatch_event_gated(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    (void)atomic_fetch_add_explicit(&state->dispatch_ready_count, 1U, memory_order_release);
    while (!atomic_load_explicit(&state->dispatch_start, memory_order_acquire)) {
        test_yield();
    }
    return dispatch_event(arg);
}

static int dispatch_task(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    return dcc_app_listener_run_task(state->entry, state->app) == DCC_OK ? 0 : 1;
}

static int run_unlisten(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    atomic_store_explicit(&state->operation_started, true, memory_order_release);
    int result = dcc_app_unlisten(state->app, state->id) == DCC_OK ? 0 : 1;
    atomic_store_explicit(&state->operation_done, true, memory_order_release);
    return result;
}

static int run_destroy(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    atomic_store_explicit(&state->operation_started, true, memory_order_release);
    dcc_app_destroy(state->app);
    state->app = NULL;
    atomic_store_explicit(&state->operation_done, true, memory_order_release);
    return 0;
}

static int run_client_runtime(void *arg) {
    concurrent_case_t *state = (concurrent_case_t *)arg;
    dcc_status_t status = dcc_runtime_run(&state->client->runtime);
    return status == DCC_OK || status == DCC_ERR_CANCELED ? 0 : 1;
}

static dcc_status_t create_app(dcc_app_t **out) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    return dcc_app_create(&options, out);
}

static dcc_status_t register_case(concurrent_case_t *state) {
    dcc_listener_t listener;
    if (state->kind == CONCURRENT_ROUTE) {
        dcc_listener_init(&listener, DCC_LISTENER_SLASH);
        listener.handler.plain = concurrent_route_handler;
        listener.target.route.name = "concurrent";
        listener.target.route.description = "concurrent route";
    } else if (state->kind == CONCURRENT_EVENT) {
        dcc_listener_init(&listener, DCC_LISTENER_EVENT);
        listener.handler.event = concurrent_event_handler;
        listener.target.event.type = DCC_EVENT_LOG;
    } else {
        dcc_listener_init(&listener, DCC_LISTENER_TASK);
        listener.handler.task = concurrent_task_handler;
        listener.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
        listener.target.schedule.interval_ms = UINT64_C(60000);
    }
    listener.user_data = state;
    listener.cleanup = concurrent_cleanup;
    dcc_status_t status = dcc_app_listen(state->app, &listener, &state->id);
    if (status == DCC_OK) {
        state->entry = state->app->listeners[state->app->listener_count - 1U];
    }
    if (status == DCC_OK && state->kind == CONCURRENT_TASK) {
        state->schedule = state->app->schedules[state->app->schedule_count - 1U];
    }
    return status;
}

static int test_detach_completion_precedes_listener_cleanup(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_ROUTE);
    if (create_app(&state.app) != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_AUTOCOMPLETE);
    listener.handler.plain = concurrent_route_handler;
    listener.user_data = &state;
    listener.cleanup = concurrent_cleanup;
    listener.target.route.name = "concurrent";
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK ||
        state.app->route_count != 1U ||
        dcc_app_route_use_internal(
            state.app,
            state.app->routes[0].id,
            concurrent_middleware,
            &state,
            blocking_middleware_cleanup
        ) != DCC_OK) {
        return 1;
    }
    state.app->listener_test_before_route_remove = blocking_before_route_remove;
    state.app->listener_test_before_route_remove_data = &state;

    test_thread_t dispatch;
    test_thread_t unlisten;
    if (test_thread_start(&dispatch, dispatch_autocomplete, &state) != 0 ||
        !test_wait_true(&state.entered, 2000U) ||
        test_thread_start(&unlisten, run_unlisten, &state) != 0 ||
        !test_wait_true(&state.operation_started, 2000U)) {
        return 1;
    }
    atomic_store_explicit(&state.release, true, memory_order_release);
    if (!test_wait_true(&state.detach_remove_entered, 2000U)) {
        return 1;
    }

    int cleanup_before_remove = test_wait_true(&state.cleanup, 250U);
    int unlisten_before_remove = atomic_load_explicit(&state.operation_done, memory_order_acquire);
    atomic_store_explicit(&state.detach_remove_release, true, memory_order_release);
    if (!test_wait_true(&state.detach_cleanup_entered, 2000U)) {
        return 1;
    }

    int cleanup_before_detach = test_wait_true(&state.cleanup, 250U);
    int unlisten_before_detach = atomic_load_explicit(&state.operation_done, memory_order_acquire);
    atomic_store_explicit(&state.detach_cleanup_release, true, memory_order_release);
    int dispatch_result = test_thread_join(&dispatch);
    int unlisten_result = test_thread_join(&unlisten);

    state.app->listener_test_before_route_remove = NULL;
    state.app->listener_test_before_route_remove_data = NULL;
    dcc_listener_t replacement;
    dcc_listener_init(&replacement, DCC_LISTENER_AUTOCOMPLETE);
    replacement.handler.plain = concurrent_route_handler;
    replacement.user_data = &state;
    replacement.target.route.name = "concurrent";
    dcc_listener_id_t replacement_id = 0U;
    dcc_status_t replacement_status = dcc_app_listen(
        state.app,
        &replacement,
        &replacement_id
    );
    int replacement_dispatch = replacement_status == DCC_OK ? dispatch_autocomplete(&state) : 1;
    dcc_status_t replacement_unlisten = replacement_status == DCC_OK
        ? dcc_app_unlisten(state.app, replacement_id)
        : replacement_status;
    dcc_app_destroy(state.app);

    if (cleanup_before_remove || unlisten_before_remove || cleanup_before_detach ||
        unlisten_before_detach || dispatch_result != 0 ||
        unlisten_result != 0 || replacement_status != DCC_OK || replacement_dispatch != 0 ||
        replacement_unlisten != DCC_OK) {
        fprintf(
            stderr,
            "listener finalized before route detach completed: "
            "pre-remove-cleanup=%d pre-remove-unlisten=%d cleanup=%d unlisten=%d\n",
            cleanup_before_remove,
            unlisten_before_remove,
            cleanup_before_detach,
            unlisten_before_detach
        );
        return 1;
    }
    return 0;
}

static int test_concurrent_unlisten(concurrent_kind_t kind) {
    concurrent_case_t state;
    concurrent_case_init(&state, kind);
    if (create_app(&state.app) != DCC_OK || register_case(&state) != DCC_OK) {
        return 1;
    }
    test_thread_t dispatch;
    test_thread_t unlisten;
    test_thread_fn dispatch_fn = kind == CONCURRENT_ROUTE
        ? dispatch_route
        : kind == CONCURRENT_EVENT ? dispatch_event : dispatch_task;
    if (test_thread_start(&dispatch, dispatch_fn, &state) != 0 ||
        !test_wait_true(&state.entered, 2000U) ||
        test_thread_start(&unlisten, run_unlisten, &state) != 0 ||
        !test_wait_true(&state.operation_started, 2000U)) {
        return 1;
    }

    int cleanup_before_release = test_wait_true(&state.cleanup, 100U);
    atomic_store_explicit(&state.release, true, memory_order_release);
    int dispatch_result = test_thread_join(&dispatch);
    int unlisten_result = test_thread_join(&unlisten);
    unsigned cleanup_count = atomic_load_explicit(&state.cleanup_count, memory_order_relaxed);
    unsigned handler_count = atomic_load_explicit(&state.handler_count, memory_order_relaxed);
    size_t listener_count = state.app->listener_count;
    size_t route_count = state.app->route_count;
    size_t schedule_count = state.app->schedule_count;
    int task_cancelled = kind != CONCURRENT_TASK ||
        (state.schedule != NULL &&
         atomic_load_explicit(&state.schedule->cancelled, memory_order_acquire));
    dcc_app_destroy(state.app);
    if (cleanup_before_release || dispatch_result != 0 || unlisten_result != 0 ||
        cleanup_count != 1U || handler_count != 1U || listener_count != 0U ||
        route_count != 0U || schedule_count != 0U || !task_cancelled) {
        fprintf(
            stderr,
            "kind %d cleanup/detach mismatch: early=%d cleanup=%u handler=%u "
            "listeners=%zu routes=%zu schedules=%zu cancelled=%d\n",
            (int)kind,
            cleanup_before_release,
            cleanup_count,
            handler_count,
            listener_count,
            route_count,
            schedule_count,
            task_cancelled
        );
        return 1;
    }
    return 0;
}

static int test_route_middleware_dispatch_snapshot(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_ROUTE);
    if (create_app(&state.app) != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_AUTOCOMPLETE);
    listener.handler.plain = concurrent_route_handler;
    listener.user_data = &state;
    listener.target.route.name = "concurrent";
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK ||
        state.app->route_count != 1U ||
        dcc_app_route_use(
            state.app,
            state.app->routes[0].id,
            original_route_middleware,
            &state
        ) != DCC_OK ||
        dcc_app_use(state.app, blocking_global_middleware, &state) != DCC_OK) {
        return 1;
    }
    atomic_store_explicit(&state.release, true, memory_order_release);

    test_thread_t dispatch;
    if (test_thread_start(&dispatch, dispatch_autocomplete, &state) != 0 ||
        !test_wait_true(&state.middleware_dispatch_entered, 2000U)) {
        return 1;
    }
    if (dcc_app_route_use(
            state.app,
            state.app->routes[0].id,
            added_route_middleware,
            &state
        ) != DCC_OK ||
        dcc_app_route_use(
            state.app,
            state.app->routes[0].id,
            added_route_middleware,
            &state
        ) != DCC_OK) {
        return 1;
    }
    atomic_store_explicit(&state.middleware_dispatch_release, true, memory_order_release);
    int dispatch_result = test_thread_join(&dispatch);
    unsigned first_original = atomic_load_explicit(
        &state.original_route_middleware_count,
        memory_order_relaxed
    );
    unsigned first_added = atomic_load_explicit(
        &state.added_route_middleware_count,
        memory_order_relaxed
    );
    int second_dispatch_result = dispatch_autocomplete(&state);
    unsigned final_original = atomic_load_explicit(
        &state.original_route_middleware_count,
        memory_order_relaxed
    );
    unsigned final_added = atomic_load_explicit(
        &state.added_route_middleware_count,
        memory_order_relaxed
    );
    dcc_status_t destroy_status = dcc_app_destroy(state.app);
    if (dispatch_result != 0 || first_original != 1U || first_added != 0U ||
        second_dispatch_result != 0 || final_original != 2U || final_added != 2U ||
        destroy_status != DCC_OK) {
        fprintf(
            stderr,
            "route middleware dispatch snapshot mismatch: first=%u/%u final=%u/%u\n",
            first_original,
            first_added,
            final_original,
            final_added
        );
        return 1;
    }
    return 0;
}

static int test_self_unlisten(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_ROUTE);
    if (create_app(&state.app) != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_SLASH);
    listener.handler.plain = self_unlisten_route_handler;
    listener.user_data = &state;
    listener.cleanup = concurrent_cleanup;
    listener.target.route.name = "concurrent";
    listener.target.route.description = "self unlisten";
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK || dispatch_route(&state) != 0) {
        return 1;
    }
    int failed = atomic_load_explicit(&state.self_status, memory_order_relaxed) != DCC_OK ||
                 atomic_load_explicit(&state.cleanup_was_early, memory_order_relaxed) ||
                 atomic_load_explicit(&state.cleanup_count, memory_order_relaxed) != 1U;
    dcc_app_destroy(state.app);
    if (failed) {
        fprintf(stderr, "self-unlisten cleaned user data before the handler returned\n");
        return 1;
    }
    return 0;
}

static int test_self_destroy(concurrent_kind_t kind) {
    concurrent_case_t state;
    concurrent_case_init(&state, kind);
    if (create_app(&state.app) != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener;
    if (kind == CONCURRENT_ROUTE) {
        dcc_listener_init(&listener, DCC_LISTENER_SLASH);
        listener.handler.plain = self_destroy_route_handler;
        listener.target.route.name = "concurrent";
        listener.target.route.description = "self destroy";
    } else if (kind == CONCURRENT_EVENT) {
        dcc_listener_init(&listener, DCC_LISTENER_EVENT);
        listener.handler.event = self_destroy_event_handler;
        listener.target.event.type = DCC_EVENT_LOG;
    } else {
        dcc_listener_init(&listener, DCC_LISTENER_TASK);
        listener.handler.task = self_destroy_task_handler;
        listener.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
        listener.target.schedule.interval_ms = UINT64_C(60000);
    }
    listener.user_data = &state;
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK) {
        return 1;
    }
    state.entry = state.app->listeners[state.app->listener_count - 1U];
    int dispatch_result = kind == CONCURRENT_ROUTE
        ? dispatch_route(&state)
        : kind == CONCURRENT_EVENT ? dispatch_event(&state) : dispatch_task(&state);
    dcc_status_t self_status = (dcc_status_t)atomic_load_explicit(
        &state.self_status,
        memory_order_acquire
    );
    int app_unchanged = state.app->listener_count == 1U &&
                        dcc_app_client(state.app) != NULL;
    dcc_status_t unlisten_status = dcc_app_unlisten(state.app, state.id);
    dcc_status_t destroy_status = dcc_app_destroy(state.app);
    if (dispatch_result != 0 || self_status != DCC_ERR_STATE || !app_unchanged ||
        unlisten_status != DCC_OK || destroy_status != DCC_OK) {
        fprintf(
            stderr,
            "kind %d self-destroy contract failed: dispatch=%d self=%d unchanged=%d "
            "unlisten=%d destroy=%d\n",
            (int)kind,
            dispatch_result,
            self_status,
            app_unchanged,
            unlisten_status,
            destroy_status
        );
        return 1;
    }
    return 0;
}

static int test_concurrent_once_listener(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_EVENT);
    if (create_app(&state.app) != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_EVENT);
    listener.handler.event = once_event_handler;
    listener.user_data = &state;
    listener.cleanup = concurrent_cleanup;
    listener.target.event.type = DCC_EVENT_LOG;
    listener.target.event.once = 1U;
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK) {
        return 1;
    }
    test_thread_t first;
    test_thread_t second;
    if (test_thread_start(&first, dispatch_event_gated, &state) != 0 ||
        test_thread_start(&second, dispatch_event_gated, &state) != 0 ||
        !test_wait_uint_at_least(&state.dispatch_ready_count, 2U, 2000U)) {
        fprintf(stderr, "concurrent once dispatch threads did not reach their start barrier\n");
        return 1;
    }
    atomic_store_explicit(&state.dispatch_start, true, memory_order_release);
    int first_result = test_thread_join(&first);
    int second_result = test_thread_join(&second);
    int failed = atomic_load_explicit(&state.handler_count, memory_order_relaxed) != 1U ||
                 atomic_load_explicit(&state.cleanup_count, memory_order_relaxed) != 1U ||
                 state.app->listener_count != 0U || first_result != 0 || second_result != 0;
    dcc_app_destroy(state.app);
    if (failed) {
        fprintf(stderr, "once listener did not detach and clean itself after one dispatch\n");
        return 1;
    }
    return 0;
}

static int test_destroy_waits_for_dispatch(concurrent_kind_t kind) {
    concurrent_case_t state;
    concurrent_case_init(&state, kind);
    if (create_app(&state.app) != DCC_OK || register_case(&state) != DCC_OK) {
        return 1;
    }
    test_thread_t dispatch;
    test_thread_t destroy;
    test_thread_fn dispatch_fn = kind == CONCURRENT_ROUTE ? dispatch_route : dispatch_event;
    if (test_thread_start(&dispatch, dispatch_fn, &state) != 0 ||
        !test_wait_true(&state.entered, 2000U) ||
        test_thread_start(&destroy, run_destroy, &state) != 0 ||
        !test_wait_true(&state.operation_started, 2000U)) {
        return 1;
    }
    int cleanup_before_release = test_wait_true(&state.cleanup, 100U);
    atomic_store_explicit(&state.release, true, memory_order_release);
    int dispatch_result = test_thread_join(&dispatch);
    int destroy_result = test_thread_join(&destroy);
    unsigned cleanup_count = atomic_load_explicit(&state.cleanup_count, memory_order_relaxed);
    if (cleanup_before_release || dispatch_result != 0 || destroy_result != 0 ||
        cleanup_count != 1U) {
        fprintf(stderr, "App destruction cleaned kind %d while its handler was in flight\n", (int)kind);
        return 1;
    }
    return 0;
}

static int test_destroy_waits_for_real_scheduled_task(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_TASK);
    if (create_app(&state.app) != DCC_OK || register_case(&state) != DCC_OK) {
        fprintf(stderr, "real scheduled task setup failed\n");
        return 1;
    }
    state.schedule->interval_ms = 1U;
    dcc_status_t start_status = dcc_app_start(state.app);
    state.client = dcc_app_client(state.app);
    test_thread_t runtime;
    if (start_status != DCC_OK ||
        test_thread_start(&runtime, run_client_runtime, &state) != 0 ||
        !test_wait_true(&state.entered, 2000U)) {
        fprintf(stderr, "real scheduled task did not start: status=%d\n", start_status);
        return 1;
    }
    test_thread_t destroy;
    if (test_thread_start(&destroy, run_destroy, &state) != 0 ||
        !test_wait_true(&state.operation_started, 2000U)) {
        fprintf(stderr, "real scheduled task destroy thread did not start\n");
        return 1;
    }
    int cleanup_before_release = test_wait_true(&state.cleanup, 100U);
    atomic_store_explicit(&state.release, true, memory_order_release);
    int destroy_result = test_thread_join(&destroy);
    int runtime_result = test_thread_join(&runtime);
    unsigned cleanup_count = atomic_load_explicit(&state.cleanup_count, memory_order_relaxed);
    unsigned handler_count = atomic_load_explicit(&state.handler_count, memory_order_relaxed);
    if (cleanup_before_release || destroy_result != 0 || runtime_result != 0 || cleanup_count != 1U ||
        handler_count != 1U) {
        fprintf(
            stderr,
            "App destruction raced a real scheduled task: early=%d cleanup=%u handler=%u\n",
            cleanup_before_release,
            cleanup_count,
            handler_count
        );
        return 1;
    }
    return 0;
}

int main(void) {
    return test_detach_completion_precedes_listener_cleanup() != 0 ||
           test_concurrent_unlisten(CONCURRENT_ROUTE) != 0 ||
           test_concurrent_unlisten(CONCURRENT_EVENT) != 0 ||
           test_concurrent_unlisten(CONCURRENT_TASK) != 0 ||
           test_route_middleware_dispatch_snapshot() != 0 ||
           test_self_unlisten() != 0 ||
           test_self_destroy(CONCURRENT_ROUTE) != 0 ||
           test_self_destroy(CONCURRENT_EVENT) != 0 ||
           test_self_destroy(CONCURRENT_TASK) != 0 ||
           test_concurrent_once_listener() != 0 ||
           test_destroy_waits_for_dispatch(CONCURRENT_ROUTE) != 0 ||
           test_destroy_waits_for_dispatch(CONCURRENT_EVENT) != 0 ||
           test_destroy_waits_for_real_scheduled_task() != 0 ||
           dcc_app_destroy(NULL) != DCC_OK;
}
