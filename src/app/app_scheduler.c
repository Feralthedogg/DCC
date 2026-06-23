#include "internal/app/dcc_app_internal.h"

#include <llam/runtime.h>

#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define DCC_APP_KST_OFFSET_SECONDS ((time_t)(9 * 60 * 60))

static dcc_status_t dcc_app_schedules_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->schedule_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->schedule_cap != 0U ? app->schedule_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->schedules)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_schedule_t **next =
        (dcc_app_schedule_t **)realloc(app->schedules, next_cap * sizeof(*app->schedules));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->schedules = next;
    app->schedule_cap = next_cap;
    return DCC_OK;
}

static uint8_t dcc_app_stopping(const dcc_app_t *app) {
    return app == NULL || atomic_load_explicit(&app->stopping, memory_order_acquire);
}

static int dcc_app_sleep_interruptible(dcc_app_t *app, uint64_t delay_ms) {
    uint64_t remaining = delay_ms;
    while (remaining > 0U && !dcc_app_stopping(app)) {
        uint64_t slice = remaining > 1000U ? 1000U : remaining;
        if (llam_sleep_ns(slice * UINT64_C(1000000)) != 0) {
            return -1;
        }
        remaining -= slice;
    }
    return dcc_app_stopping(app) ? -1 : 0;
}

static uint64_t dcc_app_daily_kst_delay_ms(uint8_t hour, uint8_t minute) {
    time_t now = time(NULL);
    time_t kst = now + DCC_APP_KST_OFFSET_SECONDS;
    struct tm tm_value;
#if defined(_WIN32)
    gmtime_s(&tm_value, &kst);
#else
    gmtime_r(&kst, &tm_value);
#endif
    uint64_t current_seconds =
        (uint64_t)tm_value.tm_hour * UINT64_C(3600) +
        (uint64_t)tm_value.tm_min * UINT64_C(60) +
        (uint64_t)tm_value.tm_sec;
    uint64_t target_seconds = (uint64_t)hour * UINT64_C(3600) + (uint64_t)minute * UINT64_C(60);
    uint64_t delta_seconds = target_seconds > current_seconds
        ? target_seconds - current_seconds
        : UINT64_C(86400) - current_seconds + target_seconds;
    return delta_seconds * UINT64_C(1000);
}

static void dcc_app_schedule_task(void *arg) {
    dcc_app_schedule_t *schedule = (dcc_app_schedule_t *)arg;
    if (schedule == NULL || schedule->app == NULL || schedule->fn == NULL) {
        return;
    }
    while (!dcc_app_stopping(schedule->app)) {
        uint64_t delay_ms = schedule->kind == DCC_APP_SCHEDULE_DAILY_KST
            ? dcc_app_daily_kst_delay_ms(schedule->hour, schedule->minute)
            : schedule->interval_ms;
        if (delay_ms == 0U) {
            delay_ms = 1U;
        }
        if (dcc_app_sleep_interruptible(schedule->app, delay_ms) != 0) {
            break;
        }
        if (!dcc_app_stopping(schedule->app)) {
            schedule->fn(schedule->app, schedule->user_data);
        }
    }
}

static dcc_status_t dcc_app_add_schedule(
    dcc_app_t *app,
    dcc_app_schedule_kind_t kind,
    uint64_t interval_ms,
    uint8_t hour,
    uint8_t minute,
    dcc_app_task_fn fn,
    void *user_data
) {
    if (app == NULL || fn == NULL || (kind == DCC_APP_SCHEDULE_INTERVAL && interval_ms == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (kind == DCC_APP_SCHEDULE_DAILY_KST && (hour > 23U || minute > 59U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->schedule_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_schedules_reserve(app, app->schedule_count + 1U);
    if (status != DCC_OK) {
        return status;
    }
    dcc_app_schedule_t *schedule = (dcc_app_schedule_t *)calloc(1U, sizeof(*schedule));
    if (schedule == NULL) {
        return DCC_ERR_NOMEM;
    }
    schedule->app = app;
    schedule->kind = kind;
    schedule->interval_ms = interval_ms;
    schedule->hour = hour;
    schedule->minute = minute;
    schedule->fn = fn;
    schedule->user_data = user_data;
    app->schedules[app->schedule_count++] = schedule;

    if (app->tasks != NULL && app->started) {
        status = dcc_task_group_spawn(app->tasks, dcc_app_schedule_task, schedule, NULL);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_app_every_ms(
    dcc_app_t *app,
    uint64_t interval_ms,
    dcc_app_task_fn fn,
    void *user_data
) {
    return dcc_app_add_schedule(app, DCC_APP_SCHEDULE_INTERVAL, interval_ms, 0U, 0U, fn, user_data);
}

dcc_status_t dcc_app_every_seconds(
    dcc_app_t *app,
    uint64_t interval_seconds,
    dcc_app_task_fn fn,
    void *user_data
) {
    if (interval_seconds > UINT64_MAX / UINT64_C(1000)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_every_ms(app, interval_seconds * UINT64_C(1000), fn, user_data);
}

dcc_status_t dcc_app_every_minutes(
    dcc_app_t *app,
    uint64_t interval_minutes,
    dcc_app_task_fn fn,
    void *user_data
) {
    if (interval_minutes > UINT64_MAX / UINT64_C(60000)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_every_ms(app, interval_minutes * UINT64_C(60000), fn, user_data);
}

dcc_status_t dcc_app_every_hours(
    dcc_app_t *app,
    uint64_t interval_hours,
    dcc_app_task_fn fn,
    void *user_data
) {
    if (interval_hours > UINT64_MAX / UINT64_C(3600000)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_every_ms(app, interval_hours * UINT64_C(3600000), fn, user_data);
}

dcc_status_t dcc_app_every_kst(
    dcc_app_t *app,
    const char *hhmm,
    dcc_app_task_fn fn,
    void *user_data
) {
    if (hhmm == NULL ||
        hhmm[0] < '0' || hhmm[0] > '9' ||
        hhmm[1] < '0' || hhmm[1] > '9' ||
        hhmm[2] != ':' ||
        hhmm[3] < '0' || hhmm[3] > '9' ||
        hhmm[4] < '0' || hhmm[4] > '9' ||
        hhmm[5] != '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    uint8_t hour = (uint8_t)((hhmm[0] - '0') * 10 + (hhmm[1] - '0'));
    uint8_t minute = (uint8_t)((hhmm[3] - '0') * 10 + (hhmm[4] - '0'));
    return dcc_app_every_day_at_kst(app, hour, minute, fn, user_data);
}

dcc_status_t dcc_app_every_day_at_kst(
    dcc_app_t *app,
    uint8_t hour,
    uint8_t minute,
    dcc_app_task_fn fn,
    void *user_data
) {
    return dcc_app_add_schedule(app, DCC_APP_SCHEDULE_DAILY_KST, 0U, hour, minute, fn, user_data);
}

dcc_status_t dcc_app_start_schedules(dcc_app_t *app) {
    if (app == NULL || app->tasks != NULL) {
        return DCC_OK;
    }
    if (app->schedule_count == 0U) {
        return DCC_OK;
    }
    dcc_status_t status = dcc_task_group_create(app->client, &app->tasks);
    if (status != DCC_OK) {
        return status;
    }
    for (size_t i = 0; i < app->schedule_count; ++i) {
        status = dcc_task_group_spawn(app->tasks, dcc_app_schedule_task, app->schedules[i], NULL);
        if (status != DCC_OK) {
            dcc_app_stop_schedules(app);
            return status;
        }
    }
    return DCC_OK;
}

void dcc_app_stop_schedules(dcc_app_t *app) {
    if (app == NULL || app->tasks == NULL) {
        return;
    }
    (void)dcc_task_group_cancel_and_wait(app->tasks, 5000U, NULL);
    (void)dcc_task_group_destroy(app->tasks);
    app->tasks = NULL;
}
