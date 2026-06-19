#ifndef DCC_REST_WELCOME_SCREEN_INTERNAL_H
#define DCC_REST_WELCOME_SCREEN_INTERNAL_H

#include <dcc/dcc.h>
#include <dcc/rest/types/guilds.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_welcome_screen_body(
    const dcc_welcome_screen_params_t *params,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
