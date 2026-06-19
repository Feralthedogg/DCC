#ifndef DCC_CORE_INTERNAL_H
#define DCC_CORE_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_set_error(dcc_client_t *client, const char *message);
char *dcc_strdup(const char *value);
void dcc_emit_log(dcc_client_t *client, dcc_log_level_t level, const char *message);
void dcc_emit_rate_limited(dcc_client_t *client, const dcc_rate_limited_event_t *limited);

#ifdef __cplusplus
}
#endif

#endif
