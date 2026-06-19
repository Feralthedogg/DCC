#ifndef DCC_HOT_RELOAD_TEST_STATE_H
#define DCC_HOT_RELOAD_TEST_STATE_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct hot_reload_test_state {
    atomic_uint init_v1;
    atomic_uint deinit_v1;
    atomic_uint ready_v1;
    atomic_uint init_v2;
    atomic_uint deinit_v2;
    atomic_uint ready_v2;
    atomic_uint shared_init_v1;
    atomic_uint shared_seen_v2;
    atomic_uint shared_ready_after_v1;
    atomic_uint shared_ready_after_v2;
} hot_reload_test_state_t;

typedef struct hot_reload_shared_state {
    uint32_t marker;
    uint32_t ready_count;
    hot_reload_test_state_t *test_state;
} hot_reload_shared_state_t;

#define HOT_RELOAD_SHARED_MARKER UINT32_C(0xDCC51001)

#endif
