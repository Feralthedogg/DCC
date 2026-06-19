#ifndef DCC_JSON_GATEWAY_PAYLOAD_SCHEDULED_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_SCHEDULED_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_SCHEDULED_RESOURCE_FIELDS \
    int has_scheduled_event; \
    dcc_scheduled_event_t scheduled_event; \
    char scheduled_event_description[1024]; \
    char scheduled_event_image[256]; \
    char scheduled_event_location[1024]; \
    char scheduled_event_creator_username[128]; \
    char scheduled_event_creator_global_name[128]; \
    char scheduled_event_creator_avatar[128]

#endif
