#ifndef DCC_JSON_GATEWAY_PAYLOAD_ACTIVITY_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_ACTIVITY_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_ACTIVITY_COLLECTION_FIELDS \
    dcc_gateway_activity_t gateway_activities[DCC_JSON_GATEWAY_ACTIVITY_CAP]; \
    char gateway_activity_names[DCC_JSON_GATEWAY_ACTIVITY_CAP][128]; \
    char gateway_activity_states[DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_activity_details[DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_activity_urls[DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_activity_emoji_names[DCC_JSON_GATEWAY_ACTIVITY_CAP][128]

#endif
