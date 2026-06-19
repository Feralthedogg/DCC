#ifndef DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_RESOURCE_FIELDS \
    dcc_role_t interaction_resolved_roles[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    char interaction_resolved_role_names[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    dcc_channel_t interaction_resolved_channels[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    char interaction_resolved_channel_names[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_channel_topics[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][1024]; \
    dcc_message_t interaction_resolved_messages[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    char interaction_resolved_message_contents[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP] \
                                              [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_MESSAGE_CONTENT_CAP]; \
    char interaction_resolved_message_timestamps[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][64]; \
    char interaction_resolved_message_edited_timestamps[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][64]; \
    char interaction_resolved_message_author_usernames[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_message_author_global_names[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_message_author_avatars[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    dcc_attachment_t interaction_resolved_attachments[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    char interaction_resolved_attachment_filenames[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][256]; \
    char interaction_resolved_attachment_descriptions[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][1024]; \
    char interaction_resolved_attachment_urls[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP] \
                                             [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_ATTACHMENT_URL_CAP]; \
    char interaction_resolved_attachment_proxy_urls[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP] \
                                                   [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_ATTACHMENT_URL_CAP]; \
    char interaction_resolved_attachment_content_types[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_attachment_waveforms[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP] \
                                                  [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_ATTACHMENT_WAVEFORM_CAP]

#endif
