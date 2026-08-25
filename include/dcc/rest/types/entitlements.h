#ifndef DCC_REST_TYPES_ENTITLEMENTS_H
#define DCC_REST_TYPES_ENTITLEMENTS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_test_entitlement_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t sku_id;
  dcc_snowflake_t owner_id;
  uint32_t owner_type;
  dcc_snowflake_t application_id; /* frozen App compatibility route suffix */
} dcc_test_entitlement_params_t;

typedef enum dcc_test_entitlement_owner_type {
  DCC_TEST_ENTITLEMENT_OWNER_GUILD = 1,
  DCC_TEST_ENTITLEMENT_OWNER_USER = 2
} dcc_test_entitlement_owner_type_t;

#define DCC_TEST_ENTITLEMENT_PARAMS_VERSION 1U
#define DCC_TEST_ENTITLEMENT_PARAMS_INIT                                       \
  {sizeof(dcc_test_entitlement_params_t),                                      \
   DCC_TEST_ENTITLEMENT_PARAMS_VERSION,                                        \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U}
static inline void
dcc_test_entitlement_params_init(dcc_test_entitlement_params_t *value) {
  if (value != NULL) {
    const dcc_test_entitlement_params_t initialized =
        DCC_TEST_ENTITLEMENT_PARAMS_INIT;
    *value = initialized;
  }
}

typedef struct dcc_rest_entitlement_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t user_id;
  const dcc_snowflake_t *sku_ids;
  size_t sku_id_count;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
  uint8_t limit;
  dcc_snowflake_t guild_id;
  uint8_t exclude_ended;
  uint8_t exclude_deleted;
} dcc_rest_entitlement_query_t;

#define DCC_REST_ENTITLEMENT_QUERY_VERSION 1U
enum {
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_USER_ID = UINT64_C(1),
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_SKU_IDS = UINT64_C(1) << 1U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_BEFORE = UINT64_C(1) << 2U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_AFTER = UINT64_C(1) << 3U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_LIMIT = UINT64_C(1) << 4U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_GUILD_ID = UINT64_C(1) << 5U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_EXCLUDE_ENDED = UINT64_C(1) << 6U,
  DCC_REST_ENTITLEMENT_QUERY_PRESENT_EXCLUDE_DELETED = UINT64_C(1) << 7U
};
#define DCC_REST_ENTITLEMENT_QUERY_INIT                                        \
  {sizeof(dcc_rest_entitlement_query_t),                                       \
   DCC_REST_ENTITLEMENT_QUERY_VERSION,                                         \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   NULL,                                                                       \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U}
static inline void
dcc_rest_entitlement_query_init(dcc_rest_entitlement_query_t *value) {
  if (value != NULL) {
    const dcc_rest_entitlement_query_t initialized =
        DCC_REST_ENTITLEMENT_QUERY_INIT;
    *value = initialized;
  }
}

#ifdef __cplusplus
}
#endif

#endif
