#include <dcc/app.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"

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
    dcc_listener_id_t id;
    dcc_app_listener_entry_t *entry;
    dcc_app_schedule_t *schedule;
    concurrent_kind_t kind;
    atomic_bool entered;
    atomic_bool release;
    atomic_bool operation_started;
    atomic_bool operation_done;
    atomic_bool cleanup;
    atomic_uint handler_count;
    atomic_uint cleanup_count;
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
    atomic_init(&state->handler_count, 0U);
    atomic_init(&state->cleanup_count, 0U);
    atomic_init(&state->self_status, DCC_OK);
    atomic_init(&state->cleanup_was_early, false);
}

static void concurrent_cleanup(void *user_data) {
    concurrent_case_t *state = (concurrent_case_t *)user_data;
    (void)atomic_fetch_add_explicit(&state->cleanup_count, 1U, memory_order_relaxed);
    atomic_store_explicit(&state->cleanup, true, memory_order_release);
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
    if (status == DCC_OK && state->kind == CONCURRENT_TASK) {
        state->entry = state->app->listeners[state->app->listener_count - 1U];
        state->schedule = state->app->schedules[state->app->schedule_count - 1U];
    }
    return status;
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

static int test_once_listener(void) {
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
    if (dcc_app_listen(state.app, &listener, &state.id) != DCC_OK ||
        dispatch_event(&state) != 0 || dispatch_event(&state) != 0) {
        return 1;
    }
    int failed = atomic_load_explicit(&state.handler_count, memory_order_relaxed) != 1U ||
                 atomic_load_explicit(&state.cleanup_count, memory_order_relaxed) != 1U ||
                 state.app->listener_count != 0U;
    dcc_app_destroy(state.app);
    if (failed) {
        fprintf(stderr, "once listener did not detach and clean itself after one dispatch\n");
        return 1;
    }
    return 0;
}

static int test_destroy_waits_for_route(void) {
    concurrent_case_t state;
    concurrent_case_init(&state, CONCURRENT_ROUTE);
    if (create_app(&state.app) != DCC_OK || register_case(&state) != DCC_OK) {
        return 1;
    }
    test_thread_t dispatch;
    test_thread_t destroy;
    if (test_thread_start(&dispatch, dispatch_route, &state) != 0 ||
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
        fprintf(stderr, "App destruction cleaned a route while its handler was in flight\n");
        return 1;
    }
    return 0;
}

int main(void) {
    return test_concurrent_unlisten(CONCURRENT_ROUTE) != 0 ||
           test_concurrent_unlisten(CONCURRENT_EVENT) != 0 ||
           test_concurrent_unlisten(CONCURRENT_TASK) != 0 ||
           test_self_unlisten() != 0 ||
           test_once_listener() != 0 ||
           test_destroy_waits_for_route() != 0;
}
