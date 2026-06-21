#ifndef DCC_HOT_RELOAD_SMOKE_SUPPORT_H
#define DCC_HOT_RELOAD_SMOKE_SUPPORT_H

#if !defined(_WIN32)

#include <dcc/dcc.h>

int dcc_hot_reload_smoke_copy_file(const char *source, const char *target);
int dcc_hot_reload_smoke_dispatch_ready(dcc_client_t *client, const char *session_id);
int dcc_hot_reload_smoke_generation_overflow(void);
int dcc_hot_reload_smoke_settle_poll(void);
int dcc_hot_reload_smoke_watcher(void);

#endif

#endif
