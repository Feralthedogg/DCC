#include "support/http_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <signal.h>
#include <stdio.h>

#define RUN_SMOKE(function_) \
    do { \
        if ((function_)() != 0) { \
            fprintf(stderr, "%s failed\n", #function_); \
            return 1; \
        } \
    } while (0)

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);
    RUN_SMOKE(run_rest_task7_initializer_smoke);
    RUN_SMOKE(run_internal_http_smoke);
    RUN_SMOKE(run_internal_http_chunked_smoke);
    /* Exercise admission rollback before the independently tracked event-wait TSan path. */
    RUN_SMOKE(run_public_rest_async_spawn_admission_smoke);
    RUN_SMOKE(run_public_event_wait_smoke);
    RUN_SMOKE(run_public_rest_smoke);
    RUN_SMOKE(run_public_rest_raw_smoke);
    RUN_SMOKE(run_public_rest_overload_smoke);
    RUN_SMOKE(run_public_rest_multipart_smoke);
    RUN_SMOKE(run_public_rest_message_builder_smoke);
    RUN_SMOKE(run_public_rest_application_command_builder_smoke);
    RUN_SMOKE(run_public_command_registry_smoke);
    RUN_SMOKE(run_public_rest_async_smoke);
    RUN_SMOKE(run_public_rest_future_smoke);
    RUN_SMOKE(run_public_rest_async_priority_smoke);
    RUN_SMOKE(run_public_rest_async_route_serial_smoke);
    RUN_SMOKE(run_public_rest_async_control_smoke);
    RUN_SMOKE(run_public_rest_async_active_cancel_smoke);
    RUN_SMOKE(run_public_rest_async_retry_queue_smoke);
    RUN_SMOKE(run_public_rest_app_send_thread_smoke);
    RUN_SMOKE(run_public_rest_rate_limit_smoke);
    RUN_SMOKE(run_public_rest_direct_message_smoke);
    RUN_SMOKE(run_public_rest_gap_smoke);
    RUN_SMOKE(run_public_rest_official_surface_smoke);
    RUN_SMOKE(run_public_rest_wrapper_smoke);
    RUN_SMOKE(run_public_rest_bucket_smoke);
    return 0;
}

#undef RUN_SMOKE

#endif
