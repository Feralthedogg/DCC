#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_subscription_t *dcc_subscription_clone(const dcc_subscription_t *subscription) {
    if (subscription == NULL) {
        return NULL;
    }

    dcc_subscription_t *copy = (dcc_subscription_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *subscription;
    copy->sku_ids = NULL;
    copy->entitlement_ids = NULL;
    copy->current_period_start = dcc_clone_string_or_null(subscription->current_period_start);
    copy->current_period_end = dcc_clone_string_or_null(subscription->current_period_end);
    copy->canceled_at = dcc_clone_string_or_null(subscription->canceled_at);
    copy->status = dcc_clone_string_or_null(subscription->status);

    if (subscription->sku_ids != NULL && subscription->sku_ids_count > 0) {
        copy->sku_ids = dcc_clone_snowflake_array_or_null(subscription->sku_ids, subscription->sku_ids_count);
        if (copy->sku_ids == NULL) {
            dcc_subscription_free(copy);
            return NULL;
        }
    }

    if (subscription->entitlement_ids != NULL && subscription->entitlement_ids_count > 0) {
        copy->entitlement_ids = dcc_clone_snowflake_array_or_null(
            subscription->entitlement_ids,
            subscription->entitlement_ids_count
        );
        if (copy->entitlement_ids == NULL) {
            dcc_subscription_free(copy);
            return NULL;
        }
    }

    if ((subscription->current_period_start != NULL && copy->current_period_start == NULL) ||
        (subscription->current_period_end != NULL && copy->current_period_end == NULL) ||
        (subscription->canceled_at != NULL && copy->canceled_at == NULL) ||
        (subscription->status != NULL && copy->status == NULL)) {
        dcc_subscription_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_subscription_free(dcc_subscription_t *subscription) {
    if (subscription == NULL) {
        return;
    }
    free((void *)subscription->sku_ids);
    free((void *)subscription->entitlement_ids);
    free((void *)subscription->current_period_start);
    free((void *)subscription->current_period_end);
    free((void *)subscription->canceled_at);
    free((void *)subscription->status);
    free(subscription);
}
