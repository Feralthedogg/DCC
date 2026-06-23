#ifndef DCC_SUGAR_DECORATORS_TASKS_H
#define DCC_SUGAR_DECORATORS_TASKS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_TASK_MS(handler_, interval_ms_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_MS_DATA(handler_, interval_ms_, user_data_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_SECONDS(handler_, interval_seconds_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_SECONDS, \
        .interval = (interval_seconds_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_MINUTES(handler_, interval_minutes_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MINUTES, \
        .interval = (interval_minutes_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_HOURS(handler_, interval_hours_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_HOURS, \
        .interval = (interval_hours_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_KST(handler_, hhmm_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_DAILY_KST(handler_, hhmm_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_DAILY_KST_DATA(handler_, hhmm_, user_data_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#endif /* DCC_SUGAR_DECORATORS_TASKS_H */
