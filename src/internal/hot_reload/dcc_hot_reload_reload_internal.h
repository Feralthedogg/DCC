#ifndef DCC_HOT_RELOAD_RELOAD_INTERNAL_H
#define DCC_HOT_RELOAD_RELOAD_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_file_sig_internal.h"

#include <dcc/hot_reload.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_hot_reload_rollback_result {
    dcc_hot_reload_file_sig_t file_sig;
    uint8_t has_file_sig;
    uint8_t attempted;
} dcc_hot_reload_rollback_result_t;

dcc_status_t dcc_hot_reload_reload(dcc_hot_reload_t *hot_reload);
dcc_hot_reload_rollback_result_t dcc_hot_reload_restore_previous_module_on_disk(
    dcc_hot_reload_t *hot_reload,
    uint64_t next_generation
);

#ifdef __cplusplus
}
#endif

#endif
