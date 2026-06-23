#ifndef DCC_SUGAR_APP_EXTENSION_TASKS_H
#define DCC_SUGAR_APP_EXTENSION_TASKS_H

#include <dcc/sugar/app_extension/base.h>

#define DCC_EXTENSION_TASK_MS(interval_ms_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_MS(interval_ms_, handler_) \
    DCC_EXTENSION_TASK_MS((interval_ms_), (handler_), NULL)

#define DCC_EVERY_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_EXTENSION_TASK_MS((interval_ms_), (handler_), (user_data_))

#define DCC_TASK_LOOP_MS(interval_ms_, handler_) \
    DCC_EVERY_MS((interval_ms_), (handler_))

#define DCC_TASK_LOOP_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_EVERY_MS_DATA((interval_ms_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_SECONDS(interval_seconds_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_SECONDS, \
        .interval = (interval_seconds_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_SECONDS(interval_seconds_, handler_) \
    DCC_EXTENSION_TASK_SECONDS((interval_seconds_), (handler_), NULL)

#define DCC_EVERY_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_EXTENSION_TASK_SECONDS((interval_seconds_), (handler_), (user_data_))

#define DCC_TASK_LOOP_SECONDS(interval_seconds_, handler_) \
    DCC_EVERY_SECONDS((interval_seconds_), (handler_))

#define DCC_TASK_LOOP_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_EVERY_SECONDS_DATA((interval_seconds_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_MINUTES(interval_minutes_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MINUTES, \
        .interval = (interval_minutes_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_MINUTES(interval_minutes_, handler_) \
    DCC_EXTENSION_TASK_MINUTES((interval_minutes_), (handler_), NULL)

#define DCC_EVERY_MINUTES_DATA(interval_minutes_, handler_, user_data_) \
    DCC_EXTENSION_TASK_MINUTES((interval_minutes_), (handler_), (user_data_))

#define DCC_TASK_LOOP_MINUTES(interval_minutes_, handler_) \
    DCC_EVERY_MINUTES((interval_minutes_), (handler_))

#define DCC_TASK_LOOP_MINUTES_DATA(interval_minutes_, handler_, user_data_) \
    DCC_EVERY_MINUTES_DATA((interval_minutes_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_HOURS(interval_hours_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_HOURS, \
        .interval = (interval_hours_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_HOURS(interval_hours_, handler_) \
    DCC_EXTENSION_TASK_HOURS((interval_hours_), (handler_), NULL)

#define DCC_EVERY_HOURS_DATA(interval_hours_, handler_, user_data_) \
    DCC_EXTENSION_TASK_HOURS((interval_hours_), (handler_), (user_data_))

#define DCC_TASK_LOOP_HOURS(interval_hours_, handler_) \
    DCC_EVERY_HOURS((interval_hours_), (handler_))

#define DCC_TASK_LOOP_HOURS_DATA(interval_hours_, handler_, user_data_) \
    DCC_EVERY_HOURS_DATA((interval_hours_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_KST(hhmm_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_KST(hhmm_, handler_) \
    DCC_EXTENSION_TASK_KST((hhmm_), (handler_), NULL)

#define DCC_EVERY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_EXTENSION_TASK_KST((hhmm_), (handler_), (user_data_))

#define DCC_TASK_LOOP_KST(hhmm_, handler_) \
    DCC_EVERY_KST((hhmm_), (handler_))

#define DCC_TASK_LOOP_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_EVERY_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_TASK_DAILY_KST(hhmm_, handler_) \
    DCC_TASK_LOOP_KST((hhmm_), (handler_))

#define DCC_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_TASK_LOOP_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_DAY_AT_KST(hour_, minute_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hour = (hour_), \
        .minute = (minute_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_DAY_AT_KST(hour_, minute_, handler_) \
    DCC_EXTENSION_TASK_DAY_AT_KST((hour_), (minute_), (handler_), NULL)

#define DCC_EVERY_DAY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_EXTENSION_TASK_DAY_AT_KST((hour_), (minute_), (handler_), (user_data_))

#define DCC_TASK_LOOP_DAY_AT_KST(hour_, minute_, handler_) \
    DCC_EVERY_DAY_AT_KST((hour_), (minute_), (handler_))

#define DCC_TASK_LOOP_DAY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_EVERY_DAY_AT_KST_DATA((hour_), (minute_), (handler_), (user_data_))

#define DCC_TASK_DAILY_AT_KST(hour_, minute_, handler_) \
    DCC_TASK_LOOP_DAY_AT_KST((hour_), (minute_), (handler_))

#define DCC_TASK_DAILY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_TASK_LOOP_DAY_AT_KST_DATA((hour_), (minute_), (handler_), (user_data_))


#endif /* DCC_SUGAR_APP_EXTENSION_TASKS_H */
