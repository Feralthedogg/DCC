#ifndef DCC_REST_VOICE_STATE_BODY_INTERNAL_H
#define DCC_REST_VOICE_STATE_BODY_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_voice_state_body(
    const dcc_voice_state_params_t *params,
    int include_request_to_speak_timestamp,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
