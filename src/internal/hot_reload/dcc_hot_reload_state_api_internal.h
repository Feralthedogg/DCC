#ifndef DCC_HOT_RELOAD_STATE_API_INTERNAL_H
#define DCC_HOT_RELOAD_STATE_API_INTERNAL_H

#include <dcc/hot_reload.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *dcc_hot_reload_state_reserve(dcc_hot_reload_t *hot_reload, size_t size, size_t alignment);

#ifdef __cplusplus
}
#endif

#endif
