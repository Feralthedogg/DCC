#ifndef DCC_SUGAR_TIME_H
#define DCC_SUGAR_TIME_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_TIME_MS_PER_SECOND UINT64_C(1000)
#define DCC_TIME_MS_PER_MINUTE UINT64_C(60000)
#define DCC_TIME_MS_PER_HOUR UINT64_C(3600000)
#define DCC_TIME_MS_PER_DAY UINT64_C(86400000)

#define DCC_DURATION_MILLISECONDS(value_) ((uint64_t)(value_))
#define DCC_DURATION_SECONDS(value_) ((uint64_t)(value_) * DCC_TIME_MS_PER_SECOND)
#define DCC_DURATION_MINUTES(value_) ((uint64_t)(value_) * DCC_TIME_MS_PER_MINUTE)
#define DCC_DURATION_HOURS(value_) ((uint64_t)(value_) * DCC_TIME_MS_PER_HOUR)
#define DCC_DURATION_DAYS(value_) ((uint64_t)(value_) * DCC_TIME_MS_PER_DAY)

#define DCC_MS(value_) DCC_DURATION_MILLISECONDS(value_)
#define DCC_SECONDS(value_) DCC_DURATION_SECONDS(value_)
#define DCC_MINUTES(value_) DCC_DURATION_MINUTES(value_)
#define DCC_HOURS(value_) DCC_DURATION_HOURS(value_)
#define DCC_DAYS(value_) DCC_DURATION_DAYS(value_)

#define DCC_TIMEOUT_FOREVER UINT32_C(0)
#define DCC_TIMEOUT_NONE DCC_TIMEOUT_FOREVER

static inline dcc_status_t dcc_sugar_duration_ms_checked(
    uint64_t value,
    uint64_t unit_ms,
    uint64_t *out_ms
) {
    if (out_ms == NULL || unit_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    if (value > UINT64_MAX / unit_ms) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_ms = value * unit_ms;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_timeout_ms_checked(
    uint64_t duration_ms,
    uint32_t *out_timeout_ms
) {
    if (out_timeout_ms == NULL || duration_ms > (uint64_t)UINT32_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_timeout_ms = (uint32_t)duration_ms;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_duration_timeout_ms_checked(
    uint64_t value,
    uint64_t unit_ms,
    uint32_t *out_timeout_ms
) {
    uint64_t duration_ms = 0U;
    dcc_status_t status = dcc_sugar_duration_ms_checked(value, unit_ms, &duration_ms);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_sugar_timeout_ms_checked(duration_ms, out_timeout_ms);
}

#define DCC_DURATION_CHECKED(value_, unit_ms_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), (unit_ms_), (out_ms_))
#define DCC_MILLISECONDS_CHECKED(value_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), UINT64_C(1), (out_ms_))
#define DCC_SECONDS_CHECKED(value_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), DCC_TIME_MS_PER_SECOND, (out_ms_))
#define DCC_MINUTES_CHECKED(value_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), DCC_TIME_MS_PER_MINUTE, (out_ms_))
#define DCC_HOURS_CHECKED(value_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), DCC_TIME_MS_PER_HOUR, (out_ms_))
#define DCC_DAYS_CHECKED(value_, out_ms_) \
    dcc_sugar_duration_ms_checked((value_), DCC_TIME_MS_PER_DAY, (out_ms_))

#define DCC_TIMEOUT_CHECKED(duration_ms_, out_timeout_ms_) \
    dcc_sugar_timeout_ms_checked((duration_ms_), (out_timeout_ms_))
#define DCC_TIMEOUT_MILLISECONDS_CHECKED(value_, out_timeout_ms_) \
    dcc_sugar_duration_timeout_ms_checked((value_), UINT64_C(1), (out_timeout_ms_))
#define DCC_TIMEOUT_SECONDS_CHECKED(value_, out_timeout_ms_) \
    dcc_sugar_duration_timeout_ms_checked((value_), DCC_TIME_MS_PER_SECOND, (out_timeout_ms_))
#define DCC_TIMEOUT_MINUTES_CHECKED(value_, out_timeout_ms_) \
    dcc_sugar_duration_timeout_ms_checked((value_), DCC_TIME_MS_PER_MINUTE, (out_timeout_ms_))
#define DCC_TIMEOUT_HOURS_CHECKED(value_, out_timeout_ms_) \
    dcc_sugar_duration_timeout_ms_checked((value_), DCC_TIME_MS_PER_HOUR, (out_timeout_ms_))
#define DCC_TIMEOUT_DAYS_CHECKED(value_, out_timeout_ms_) \
    dcc_sugar_duration_timeout_ms_checked((value_), DCC_TIME_MS_PER_DAY, (out_timeout_ms_))

#endif
