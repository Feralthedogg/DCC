#ifndef DCC_REST_ROLE_BODY_INTERNAL_H
#define DCC_REST_ROLE_BODY_INTERNAL_H

#include <dcc/error.h>
#include <dcc/rest/types/roles.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_role_body(const dcc_role_params_t *params, uint8_t include_id, char **out);

#ifdef __cplusplus
}
#endif

#endif
