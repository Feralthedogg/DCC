#ifndef DCC_CLUSTER_IDENTIFY_COORDINATOR_INTERNAL_H
#define DCC_CLUSTER_IDENTIFY_COORDINATOR_INTERNAL_H

#include <stdatomic.h>
#include <stdint.h>

#define DCC_CLUSTER_IDENTIFY_BUCKET_CAP 64U

typedef struct dcc_cluster_identify_coordinator {
    atomic_flag lock;
    uint32_t max_concurrency;
    uint32_t remaining;
    uint32_t total;
    uint64_t reset_at_ms;
    uint64_t bucket_next_ms[DCC_CLUSTER_IDENTIFY_BUCKET_CAP];
    atomic_uint_fast64_t waits;
    atomic_uint_fast64_t reservations;
} dcc_cluster_identify_coordinator_t;

#endif
