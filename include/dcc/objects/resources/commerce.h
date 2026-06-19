#ifndef DCC_OBJECTS_RESOURCES_COMMERCE_H
#define DCC_OBJECTS_RESOURCES_COMMERCE_H

/* Entitlement and subscription resource objects. */

#include <dcc/objects/base.h>

typedef struct dcc_entitlement {
    dcc_snowflake_t id;
    dcc_snowflake_t sku_id;
    dcc_snowflake_t application_id;
    dcc_snowflake_t subscription_id;
    dcc_snowflake_t promotion_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t guild_id;
    uint64_t flags;
    uint32_t type;
    uint32_t gift_code_flags;
    const char *start_time;
    const char *end_time;
} dcc_entitlement_t;

typedef struct dcc_subscription {
    dcc_snowflake_t id;
    dcc_snowflake_t user_id;
    const dcc_snowflake_t *sku_ids;
    size_t sku_ids_count;
    uint8_t sku_ids_truncated;
    const dcc_snowflake_t *entitlement_ids;
    size_t entitlement_ids_count;
    uint8_t entitlement_ids_truncated;
    const char *current_period_start;
    const char *current_period_end;
    const char *canceled_at;
    const char *status;
} dcc_subscription_t;

#endif
