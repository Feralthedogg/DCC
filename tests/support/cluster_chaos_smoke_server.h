#ifndef DCC_TEST_CLUSTER_CHAOS_SMOKE_SERVER_H
#define DCC_TEST_CLUSTER_CHAOS_SMOKE_SERVER_H

#if !defined(_WIN32)

#include <pthread.h>
#include <stdatomic.h>

#define CLUSTER_CHAOS_SHARDS 2U
#define CLUSTER_CHAOS_CONNECTIONS (CLUSTER_CHAOS_SHARDS * 3U)

typedef struct cluster_chaos_server {
    int fd;
    unsigned short port;
    pthread_t handlers[CLUSTER_CHAOS_CONNECTIONS];
    atomic_uint handler_count;
    atomic_uint accepts;
    atomic_uint identifies;
    atomic_uint resumes;
    atomic_uint close_4000;
    atomic_uint close_4009;
    atomic_uint final_ready;
    atomic_uint bad;
    atomic_uint shard_phase[CLUSTER_CHAOS_SHARDS];
} cluster_chaos_server_t;

int dcc_test_cluster_chaos_start_gateway_server(cluster_chaos_server_t *server, pthread_t *thread);

#endif

#endif
