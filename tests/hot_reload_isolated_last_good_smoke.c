#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "support/hot_reload_isolated_support.h"

int main(void) {
    dcc_status_t bad_candidate_status = dcc_hot_reload_isolated_bad_candidate_smoke();
    if (bad_candidate_status != DCC_OK) {
        return 1;
    }

    dcc_status_t exit_candidate_status = dcc_hot_reload_isolated_exit_candidate_smoke();
    if (exit_candidate_status != DCC_OK) {
        return 1;
    }

    dcc_status_t timeout_candidate_status = dcc_hot_reload_isolated_timeout_candidate_smoke();
    if (timeout_candidate_status != DCC_OK) {
        return 1;
    }

    dcc_status_t invalid_ready_status = dcc_hot_reload_isolated_invalid_ready_worker_smoke();
    if (invalid_ready_status != DCC_OK) {
        return 1;
    }

    dcc_status_t malformed_event_status = dcc_hot_reload_isolated_malformed_event_result_smoke();
    if (malformed_event_status != DCC_OK) {
        return 1;
    }

    dcc_status_t hung_event_status = dcc_hot_reload_isolated_hung_event_result_smoke();
    if (hung_event_status != DCC_OK) {
        return 1;
    }

    dcc_status_t idle_status = dcc_hot_reload_isolated_idle_promotion_smoke();
    if (idle_status != DCC_OK) {
        return 1;
    }

    return dcc_hot_reload_isolated_last_good_retry_smoke();
}

#endif
