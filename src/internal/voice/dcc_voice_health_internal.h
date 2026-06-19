#ifndef DCC_VOICE_HEALTH_INTERNAL_H
#define DCC_VOICE_HEALTH_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_voice_health_t dcc_voice_health_classify(
    const dcc_voice_session_info_t *session,
    const dcc_voice_client_stats_t *stats,
    dcc_voice_reconnect_action_t action
);

const char *dcc_voice_health_reason(
    const dcc_voice_session_info_t *session,
    const dcc_voice_client_stats_t *stats,
    dcc_voice_health_t health,
    dcc_voice_reconnect_action_t action
);

#ifdef __cplusplus
}
#endif

#endif
