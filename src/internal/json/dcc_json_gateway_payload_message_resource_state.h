#ifndef DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_FIELDS \
    int has_message; \
    dcc_message_t message; \
    char message_content[8192]; \
    char message_edited_timestamp[64]; \
    char author_username[128]; \
    char author_global_name[128]; \
    char author_avatar[128]

#endif
