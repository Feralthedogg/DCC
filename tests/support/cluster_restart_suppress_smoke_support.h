#ifndef DCC_TEST_CLUSTER_RESTART_SUPPRESS_SMOKE_SUPPORT_H
#define DCC_TEST_CLUSTER_RESTART_SUPPRESS_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#include <stdatomic.h>

typedef struct suppress_gateway_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint identifies;
    atomic_uint bad;
} suppress_gateway_server_t;

typedef struct suppress_monitor {
    dcc_cluster_t *cluster;
    atomic_uint suppressed_seen;
    atomic_uint timeout_seen;
    atomic_uint bad;
} suppress_monitor_t;

int suppress_start_listener(int *out_fd, unsigned short *out_port);
void *suppress_gateway_server_main(void *arg);
void *suppress_monitor_main(void *arg);
void suppress_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data);

#endif
