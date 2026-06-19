#ifndef DCC_REST_CHANNEL_BODY_INTERNAL_H
#define DCC_REST_CHANNEL_BODY_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_rest_channel_archive_duration_valid(dcc_channel_auto_archive_duration_t duration);
dcc_status_t dcc_rest_build_channel_body(
    const dcc_channel_params_t *params,
    uint8_t include_id,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
