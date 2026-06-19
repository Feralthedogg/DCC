#ifndef DCC_HOT_RELOAD_RUNTIME_INTERNAL_H
#define DCC_HOT_RELOAD_RUNTIME_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void dcc_hot_reload_copy_text(char *out, size_t out_size, const char *text) {
    if (out == NULL || out_size == 0U) {
        return;
    }
    if (text == NULL) {
        out[0] = '\0';
        return;
    }
    (void)snprintf(out, out_size, "%s", text);
}

void dcc_hot_reload_set_error(dcc_hot_reload_t *hot_reload, dcc_status_t status, const char *message);
void dcc_hot_reload_log(dcc_hot_reload_t *hot_reload, dcc_log_level_t level, const char *message);
uint64_t dcc_hot_reload_now_ms(void);

#ifdef __cplusplus
}
#endif

#endif
