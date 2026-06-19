#ifndef DCC_JSON_GATEWAY_PAYLOAD_BASE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_BASE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_BASE_RESOURCE_FIELDS \
    int has_channel; \
    dcc_channel_t channel; \
    int has_role; \
    dcc_role_t role; \
    int has_member; \
    dcc_member_t member

#endif
