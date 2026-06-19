#ifndef DCC_JSON_GATEWAY_PAYLOAD_COMMERCE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_COMMERCE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_COMMERCE_RESOURCE_FIELDS \
    int has_entitlement; \
    dcc_entitlement_t entitlement; \
    int has_subscription; \
    dcc_subscription_t subscription; \
    int has_invite; \
    dcc_invite_t invite

#endif
