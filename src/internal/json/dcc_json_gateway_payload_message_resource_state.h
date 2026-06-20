#ifndef DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_FIELDS \
    int has_message; \
    dcc_message_t message; \
    dcc_component_v2_t message_components[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP]; \
    dcc_select_option_t message_component_options[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_OPTION_CAP]; \
    dcc_component_v2_media_t message_component_media[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_MEDIA_CAP]; \
    dcc_component_v2_select_default_value_t message_component_default_values[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_DEFAULT_VALUE_CAP]; \
    uint32_t message_component_channel_types[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CHANNEL_TYPE_CAP]; \
    char message_content[8192]; \
    char message_components_json[65536]; \
    char message_component_custom_ids[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_labels[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_descriptions[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_LONG_TEXT_CAP]; \
    char message_component_placeholders[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_contents[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_LONG_TEXT_CAP]; \
    char message_component_urls[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_URL_CAP]; \
    char message_component_values[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_option_labels[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_OPTION_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_option_values[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_OPTION_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_option_descriptions[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_OPTION_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_SHORT_TEXT_CAP]; \
    char message_component_media_urls[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_MEDIA_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_URL_CAP]; \
    char message_component_media_descriptions[DCC_JSON_GATEWAY_MESSAGE_COMPONENT_MEDIA_CAP][DCC_JSON_GATEWAY_MESSAGE_COMPONENT_LONG_TEXT_CAP]; \
    char message_edited_timestamp[64]; \
    char author_username[128]; \
    char author_global_name[128]; \
    char author_avatar[128]; \
    size_t message_component_option_count; \
    size_t message_component_media_count; \
    size_t message_component_default_value_count; \
    size_t message_component_channel_type_count

#endif
