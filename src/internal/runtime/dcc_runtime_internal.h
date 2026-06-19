#ifndef DCC_RUNTIME_INTERNAL_H
#define DCC_RUNTIME_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_runtime {
    int initialized;
    uint64_t tasks_spawned;
} dcc_runtime_t;

dcc_status_t dcc_runtime_init(dcc_runtime_t *runtime);
dcc_status_t dcc_runtime_spawn(dcc_runtime_t *runtime, void (*fn)(void *), void *arg);
dcc_status_t dcc_runtime_run(dcc_runtime_t *runtime);
dcc_status_t dcc_runtime_request_stop(dcc_runtime_t *runtime);
void dcc_runtime_shutdown(dcc_runtime_t *runtime);

#ifdef __cplusplus
}
#endif

#endif
