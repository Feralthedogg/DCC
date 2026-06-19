#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_commerce_resources(dcc_json_gateway_payload_t *out) {
    if (out->entitlement.id == 0 && out->gateway.entitlement_id != 0) {
        out->entitlement.id = out->gateway.entitlement_id;
    }
    if (out->entitlement.sku_id == 0 && out->gateway.sku_id != 0) {
        out->entitlement.sku_id = out->gateway.sku_id;
    }
    if (out->entitlement.application_id == 0 && out->gateway.application_id != 0) {
        out->entitlement.application_id = out->gateway.application_id;
    }
    if (out->entitlement.user_id == 0 && out->gateway.user_id != 0) {
        out->entitlement.user_id = out->gateway.user_id;
    }
    if (out->entitlement.guild_id == 0 && out->gateway.guild_id != 0) {
        out->entitlement.guild_id = out->gateway.guild_id;
    }
    if (out->subscription.id == 0 && out->gateway.id != 0) {
        out->subscription.id = out->gateway.id;
    }
    if (out->subscription.user_id == 0 && out->gateway.user_id != 0) {
        out->subscription.user_id = out->gateway.user_id;
    }
}
