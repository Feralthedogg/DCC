#ifndef DCC_HOT_RELOAD_STATE_H
#define DCC_HOT_RELOAD_STATE_H

#include <dcc/hot_reload/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API const char *dcc_hot_reload_path(const dcc_hot_reload_t *hot_reload);
DCC_API uint64_t dcc_hot_reload_generation(const dcc_hot_reload_t *hot_reload);
DCC_API void *dcc_hot_reload_state(dcc_hot_reload_t *hot_reload, size_t size);
DCC_API size_t dcc_hot_reload_state_size(const dcc_hot_reload_t *hot_reload);
DCC_API dcc_status_t dcc_hot_reload_last_status(const dcc_hot_reload_t *hot_reload);
DCC_API const char *dcc_hot_reload_last_error(const dcc_hot_reload_t *hot_reload);
DCC_API uint8_t dcc_hot_reload_running(const dcc_hot_reload_t *hot_reload);

#ifdef __cplusplus
}
#endif

#endif
