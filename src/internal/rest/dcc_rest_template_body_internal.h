#ifndef DCC_REST_TEMPLATE_BODY_INTERNAL_H
#define DCC_REST_TEMPLATE_BODY_INTERNAL_H

#include <dcc/rest/types/templates.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_rest_template_body_mode {
    DCC_REST_TEMPLATE_BODY_CREATE_GUILD = 0,
    DCC_REST_TEMPLATE_BODY_TEMPLATE
} dcc_rest_template_body_mode_t;

dcc_status_t dcc_rest_build_template_body(
    const dcc_template_params_t *params,
    dcc_rest_template_body_mode_t mode,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
