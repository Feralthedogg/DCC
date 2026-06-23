#ifndef DCC_EVENT_WAIT_CONVENIENCE_SMOKE_SUPPORT_H
#define DCC_EVENT_WAIT_CONVENIENCE_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

typedef struct dcc_event_wait_thread_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_interaction_t *interaction;
} dcc_event_wait_thread_state_t;

typedef struct dcc_client_wait_thread_state {
    dcc_client_t *client;
    dcc_status_t status;
} dcc_client_wait_thread_state_t;

void *event_wait_convenience_gateway_ready_wait_thread_main(void *arg);
void *event_wait_convenience_gateway_ready_or_close_wait_thread_main(void *arg);
void *event_wait_convenience_gateway_admission_wait_thread_main(void *arg);
void *event_wait_convenience_interaction_or_close_wait_thread_main(void *arg);
void *event_wait_convenience_interaction_admission_wait_thread_main(void *arg);
void *event_wait_convenience_interaction_or_close_owned_wait_thread_main(void *arg);
void *event_wait_convenience_interaction_admission_owned_wait_thread_main(void *arg);
void *event_wait_convenience_client_wait_thread_main(void *arg);
int event_wait_convenience_dispatch_gateway_ready(dcc_client_t *client);
int event_wait_convenience_dispatch_slash_command(dcc_client_t *client);
int event_wait_convenience_dispatch_socket_close(dcc_client_t *client);
int event_wait_convenience_check_invalid_contracts(dcc_client_t *client);

#endif
