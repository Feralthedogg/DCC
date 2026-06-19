#ifndef DCC_REST_ENTITLEMENT_BODY_INTERNAL_H
#define DCC_REST_ENTITLEMENT_BODY_INTERNAL_H

#include <dcc/dcc.h>
#include <dcc/rest/types/entitlements.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_test_entitlement_body(
    const dcc_test_entitlement_params_t *params,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
