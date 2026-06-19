#ifndef DCC_REST_ONBOARDING_OPTION_BUILDERS_INTERNAL_H
#define DCC_REST_ONBOARDING_OPTION_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_append_onboarding_option(
    dcc_rest_buffer_t *body,
    const dcc_onboarding_prompt_option_params_t *option
);

#ifdef __cplusplus
}
#endif

#endif
