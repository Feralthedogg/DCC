#ifndef DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TEXT_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TEXT_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TEXT_FIELDS \
    char gateway_user_username[128]; \
    char gateway_user_global_name[128]; \
    char gateway_user_avatar[128]; \
    char gateway_role_name[128]; \
    char gateway_topic[1024]; \
    char gateway_type_text[128]; \
    char gateway_desktop_status[32]; \
    char gateway_mobile_status[32]; \
    char gateway_web_status[32]; \
    char gateway_code[128]; \
    char gateway_status[256]; \
    char gateway_endpoint[512]; \
    char gateway_emoji_name[128]; \
    char gateway_reason[256]; \
    char gateway_content[8192]; \
    char gateway_matched_keyword[256]; \
    char gateway_matched_content[1024]

#endif
