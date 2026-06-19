#include <dcc/dcc.h>

#include <stdio.h>
#include <string.h>

int main(void) {
    dcc_hot_reload_canary_options_t opts;
    dcc_hot_reload_canary_options_init(&opts);
    if (opts.canary_percent != 5U ||
        opts.promote_after_ms != 30000U ||
        opts.max_error_rate != 0.05 ||
        opts.max_consecutive_failures != 3U ||
        dcc_hot_reload_canary_options_validate(&opts) != DCC_OK) {
        fprintf(stderr, "unexpected canary defaults\n");
        return 1;
    }

    dcc_hot_reload_canary_state_t state;
    if (dcc_hot_reload_canary_start(&state, &opts, 10U, 1000U) != DCC_OK ||
        !dcc_hot_reload_canary_should_route(&state, 4U) ||
        dcc_hot_reload_canary_should_route(&state, 5U) ||
        dcc_hot_reload_canary_record(&state, 1U) != DCC_OK ||
        dcc_hot_reload_canary_decide(&state, 30999U) != DCC_HOT_RELOAD_CANARY_KEEP ||
        dcc_hot_reload_canary_decide(&state, 31000U) != DCC_HOT_RELOAD_CANARY_PROMOTE ||
        state.rollback_reason != DCC_HOT_RELOAD_CANARY_ROLLBACK_NONE) {
        fprintf(stderr, "unexpected canary promote path\n");
        return 1;
    }

    opts.canary_percent = 100U;
    opts.max_error_rate = 0.20;
    opts.max_consecutive_failures = 10U;
    if (dcc_hot_reload_canary_start(&state, &opts, 11U, 0U) != DCC_OK ||
        dcc_hot_reload_canary_record(&state, 1U) != DCC_OK ||
        dcc_hot_reload_canary_record(&state, 1U) != DCC_OK ||
        dcc_hot_reload_canary_record(&state, 0U) != DCC_OK ||
        dcc_hot_reload_canary_decide(&state, 1U) != DCC_HOT_RELOAD_CANARY_ROLLBACK ||
        state.rollback_reason != DCC_HOT_RELOAD_CANARY_ROLLBACK_ERROR_RATE) {
        fprintf(stderr, "unexpected canary error-rate rollback\n");
        return 1;
    }

    opts.max_error_rate = 1.0;
    opts.max_consecutive_failures = 3U;
    if (dcc_hot_reload_canary_start(&state, &opts, 12U, 0U) != DCC_OK ||
        dcc_hot_reload_canary_record(&state, 0U) != DCC_OK ||
        dcc_hot_reload_canary_record(&state, 0U) != DCC_OK ||
        dcc_hot_reload_canary_decide(&state, 1U) != DCC_HOT_RELOAD_CANARY_KEEP ||
        dcc_hot_reload_canary_record(&state, 0U) != DCC_OK ||
        dcc_hot_reload_canary_decide(&state, 1U) != DCC_HOT_RELOAD_CANARY_ROLLBACK ||
        state.rollback_reason != DCC_HOT_RELOAD_CANARY_ROLLBACK_CONSECUTIVE_FAILURES ||
        dcc_hot_reload_canary_options_validate(NULL) != DCC_ERR_INVALID_ARG ||
        strcmp(dcc_hot_reload_canary_decision_string(DCC_HOT_RELOAD_CANARY_PROMOTE), "promote") != 0 ||
        strcmp(
            dcc_hot_reload_canary_rollback_reason_string(DCC_HOT_RELOAD_CANARY_ROLLBACK_ERROR_RATE),
            "error_rate"
        ) != 0) {
        fprintf(stderr, "unexpected canary consecutive rollback\n");
        return 1;
    }
    return 0;
}
