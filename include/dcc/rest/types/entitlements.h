#ifndef DCC_REST_TYPES_ENTITLEMENTS_H
#define DCC_REST_TYPES_ENTITLEMENTS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_test_entitlement_params {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_snowflake_t sku_id;
    dcc_snowflake_t owner_id;
    uint32_t owner_type;
} dcc_test_entitlement_params_t;

#ifdef __cplusplus
}
#endif

#endif
