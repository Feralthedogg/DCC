#include "support/http_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <signal.h>

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);
    if (run_internal_http_smoke() != 0) {
        return 1;
    }
    if (run_internal_http_chunked_smoke() != 0) {
        return 1;
    }
    if (run_public_event_wait_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_raw_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_overload_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_multipart_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_message_builder_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_application_command_builder_smoke() != 0) {
        return 1;
    }
    if (run_public_command_registry_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_future_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_priority_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_route_serial_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_control_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_active_cancel_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_async_retry_queue_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_app_send_thread_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_rate_limit_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_direct_message_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_gap_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_official_surface_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_wrapper_smoke() != 0) {
        return 1;
    }
    if (run_public_rest_bucket_smoke() != 0) {
        return 1;
    }
    return 0;
}

#endif
