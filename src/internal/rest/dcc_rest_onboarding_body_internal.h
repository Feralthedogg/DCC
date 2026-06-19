#ifndef DCC_REST_ONBOARDING_BODY_INTERNAL_H
#define DCC_REST_ONBOARDING_BODY_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_onboarding_body(
    const dcc_onboarding_params_t *params,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
