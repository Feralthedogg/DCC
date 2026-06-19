#ifndef DCC_HOT_RELOAD_ISOLATED_SUPPORT_H
#define DCC_HOT_RELOAD_ISOLATED_SUPPORT_H

#if !defined(_WIN32)

#include <dcc/dcc.h>

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

typedef struct dcc_hot_reload_test_server {
    int fd;
    uint16_t port;
    char request[5][4096];
    size_t request_len[5];
    size_t request_count;
    size_t max_requests;
} dcc_hot_reload_test_server_t;

typedef struct dcc_hot_reload_test_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} dcc_hot_reload_test_wait_state_t;

void *dcc_hot_reload_test_wait_main(void *arg);
void *dcc_hot_reload_test_server_main(void *arg);
int dcc_hot_reload_test_server_start(dcc_hot_reload_test_server_t *server);
int dcc_hot_reload_test_copy_file(const char *source, const char *target);
int dcc_hot_reload_test_write_marker_file(const char *target, const char *marker);
int dcc_hot_reload_test_dispatch_raw_slash(dcc_client_t *client, uint64_t sequence);

dcc_status_t dcc_hot_reload_isolated_bad_candidate_smoke(void);
dcc_status_t dcc_hot_reload_isolated_exit_candidate_smoke(void);
dcc_status_t dcc_hot_reload_isolated_timeout_candidate_smoke(void);
dcc_status_t dcc_hot_reload_isolated_invalid_ready_worker_smoke(void);
dcc_status_t dcc_hot_reload_isolated_malformed_event_result_smoke(void);
dcc_status_t dcc_hot_reload_isolated_hung_event_result_smoke(void);
dcc_status_t dcc_hot_reload_isolated_idle_promotion_smoke(void);
int dcc_hot_reload_isolated_last_good_retry_smoke(void);

#endif

#endif
