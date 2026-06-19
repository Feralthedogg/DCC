#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_auto_moderation_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
) {
    if (dcc_json_event_prefix_eq(
            event_name,
            sizeof(out->event_name),
            "AUTO_MODERATION_RULE_",
            sizeof("AUTO_MODERATION_RULE_") - 1
        ) &&
        out->gateway.rule_id == 0) {
        out->gateway.rule_id = out->gateway.id;
        if (out->auto_moderation_rule.id == 0) {
            out->auto_moderation_rule.id = out->gateway.id;
        }
    }
}

void dcc_json_gateway_apply_entitlement_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
) {
    if (dcc_json_event_prefix_eq(
            event_name,
            sizeof(out->event_name),
            "ENTITLEMENT_",
            sizeof("ENTITLEMENT_") - 1
        ) &&
        out->gateway.entitlement_id == 0) {
        out->gateway.entitlement_id = out->gateway.id;
        if (out->entitlement.id == 0) {
            out->entitlement.id = out->gateway.id;
        }
    }
}
