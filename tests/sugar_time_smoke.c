#include <dcc/sugar.h>

#include <stdint.h>

static int check_duration_macros(void) {
    if (DCC_MS(250U) != UINT64_C(250)) {
        return 1;
    }
    if (DCC_SECONDS(2U) != UINT64_C(2000)) {
        return 1;
    }
    if (DCC_MINUTES(3U) != UINT64_C(180000)) {
        return 1;
    }
    if (DCC_HOURS(4U) != UINT64_C(14400000)) {
        return 1;
    }
    if (DCC_DAYS(2U) != UINT64_C(172800000)) {
        return 1;
    }
    return 0;
}

static int check_checked_duration(void) {
    uint64_t duration_ms = 0U;

    if (DCC_SECONDS_CHECKED(90U, &duration_ms) != DCC_OK ||
        duration_ms != UINT64_C(90000)) {
        return 1;
    }
    if (DCC_MINUTES_CHECKED(5U, &duration_ms) != DCC_OK ||
        duration_ms != UINT64_C(300000)) {
        return 1;
    }
    if (DCC_DURATION_CHECKED(UINT64_MAX, DCC_TIME_MS_PER_SECOND, &duration_ms) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    if (DCC_SECONDS_CHECKED(1U, NULL) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    return 0;
}

static int check_timeout_checked(void) {
    uint32_t timeout_ms = 0U;

    if (DCC_TIMEOUT_SECONDS_CHECKED(30U, &timeout_ms) != DCC_OK ||
        timeout_ms != 30000U) {
        return 1;
    }
    if (DCC_TIMEOUT_MINUTES_CHECKED(1U, &timeout_ms) != DCC_OK ||
        timeout_ms != 60000U) {
        return 1;
    }
    if (DCC_TIMEOUT_CHECKED((uint64_t)UINT32_MAX + 1U, &timeout_ms) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    if (DCC_TIMEOUT_SECONDS_CHECKED(1U, NULL) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    return 0;
}

static int check_sugar_structs(void) {
    dcc_app_cooldown_options_t cooldown = DCC_COOLDOWN_USER(3U, DCC_SECONDS(10U));
    if (cooldown.bucket != DCC_APP_COOLDOWN_USER ||
        cooldown.limit != 3U ||
        cooldown.window_ms != UINT64_C(10000)) {
        return 1;
    }

    dcc_app_options_t options =
        DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL("token", DCC_INTENTS_DEFAULT, DCC_MS(1500U));
    if (options.auto_defer_after_ms != 1500U || options.auto_defer_ephemeral != 1U) {
        return 1;
    }

    dcc_interaction_flow_t flow = DCC_FLOW_AUTO_DEFER(NULL, NULL, DCC_SECONDS(2U));
    if (flow.auto_defer_after_ms != 2000U) {
        return 1;
    }
    return 0;
}

int main(void) {
    if (check_duration_macros() != 0) {
        return 1;
    }
    if (check_checked_duration() != 0) {
        return 1;
    }
    if (check_timeout_checked() != 0) {
        return 1;
    }
    if (check_sugar_structs() != 0) {
        return 1;
    }
    return 0;
}
