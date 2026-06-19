#ifndef DCC_HOT_RELOAD_OPTIONS_INTERNAL_H
#define DCC_HOT_RELOAD_OPTIONS_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t dcc_hot_reload_options_has_field(
    const dcc_hot_reload_options_t *options,
    size_t offset,
    size_t field_size
);
dcc_status_t dcc_hot_reload_apply_worker_options(
    dcc_hot_reload_t *hot_reload,
    const dcc_hot_reload_options_t *options
);
dcc_status_t dcc_hot_reload_apply_create_options(
    dcc_hot_reload_t *hot_reload,
    const char *library_path,
    const dcc_hot_reload_options_t *options
);

#ifdef __cplusplus
}
#endif

#endif
