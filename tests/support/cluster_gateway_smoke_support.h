#ifndef DCC_TEST_CLUSTER_GATEWAY_SMOKE_SUPPORT_H
#define DCC_TEST_CLUSTER_GATEWAY_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#if !defined(_WIN32)

#include <pthread.h>
#include <stdatomic.h>

#define CLUSTER_GATEWAY_SHARDS 2U
#define CLUSTER_GATEWAY_CONNECTIONS (CLUSTER_GATEWAY_SHARDS * 2U)

typedef struct cluster_gateway_server {
    int fd;
    unsigned short port;
    pthread_t handlers[CLUSTER_GATEWAY_CONNECTIONS];
    atomic_uint identify_count;
    atomic_uint rolling_identify_count;
    atomic_uint bad;
    atomic_uint initial_seen[CLUSTER_GATEWAY_SHARDS];
    atomic_uint rolling_seen[CLUSTER_GATEWAY_SHARDS];
} cluster_gateway_server_t;

typedef struct cluster_api_server {
    int fd;
    unsigned short port;
    unsigned short gateway_port;
    atomic_uint requests;
} cluster_api_server_t;

int dcc_test_cluster_gateway_start_gateway_server(cluster_gateway_server_t *server, pthread_t *thread);
int dcc_test_cluster_gateway_start_api_server(
    cluster_api_server_t *server,
    pthread_t *thread,
    unsigned short gateway_port
);

#endif

#endif
