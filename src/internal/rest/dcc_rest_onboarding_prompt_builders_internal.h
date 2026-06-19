#ifndef DCC_REST_ONBOARDING_PROMPT_BUILDERS_INTERNAL_H
#define DCC_REST_ONBOARDING_PROMPT_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_append_onboarding_prompt(
    dcc_rest_buffer_t *body,
    const dcc_onboarding_prompt_params_t *prompt
);

#ifdef __cplusplus
}
#endif

#endif
