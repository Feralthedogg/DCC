#include "internal/objects/dcc_objects_internal.h"

#define DCC_GATEWAY_CLONE_STRING_FIELD(field)       \
    do {                                            \
        copy->field = dcc_clone_string_or_null(data->field); \
        if (data->field != NULL && copy->field == NULL) {    \
            return -1;                              \
        }                                           \
    } while (0)

int dcc_gateway_event_data_clone_strings(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
) {
    if (copy == NULL || data == NULL) {
        return -1;
    }

    if (dcc_clone_user_fields(&copy->user, &data->user) != 0) {
        return -1;
    }
    DCC_GATEWAY_CLONE_STRING_FIELD(name);
    DCC_GATEWAY_CLONE_STRING_FIELD(description);
    DCC_GATEWAY_CLONE_STRING_FIELD(topic);
    DCC_GATEWAY_CLONE_STRING_FIELD(type_text);
    DCC_GATEWAY_CLONE_STRING_FIELD(desktop_status);
    DCC_GATEWAY_CLONE_STRING_FIELD(mobile_status);
    DCC_GATEWAY_CLONE_STRING_FIELD(web_status);
    DCC_GATEWAY_CLONE_STRING_FIELD(code);
    DCC_GATEWAY_CLONE_STRING_FIELD(status);
    DCC_GATEWAY_CLONE_STRING_FIELD(token);
    DCC_GATEWAY_CLONE_STRING_FIELD(endpoint);
    DCC_GATEWAY_CLONE_STRING_FIELD(emoji_name);
    DCC_GATEWAY_CLONE_STRING_FIELD(reason);
    DCC_GATEWAY_CLONE_STRING_FIELD(content);
    DCC_GATEWAY_CLONE_STRING_FIELD(matched_keyword);
    DCC_GATEWAY_CLONE_STRING_FIELD(matched_content);
    DCC_GATEWAY_CLONE_STRING_FIELD(timestamp_text);
    DCC_GATEWAY_CLONE_STRING_FIELD(join_timestamp);
    DCC_GATEWAY_CLONE_STRING_FIELD(created_at);
    DCC_GATEWAY_CLONE_STRING_FIELD(expires_at);
    DCC_GATEWAY_CLONE_STRING_FIELD(start_time);
    DCC_GATEWAY_CLONE_STRING_FIELD(end_time);
    DCC_GATEWAY_CLONE_STRING_FIELD(current_period_start);
    DCC_GATEWAY_CLONE_STRING_FIELD(current_period_end);
    DCC_GATEWAY_CLONE_STRING_FIELD(canceled_at);
    DCC_GATEWAY_CLONE_STRING_FIELD(joined_at);
    DCC_GATEWAY_CLONE_STRING_FIELD(premium_since);
    DCC_GATEWAY_CLONE_STRING_FIELD(communication_disabled_until);
    DCC_GATEWAY_CLONE_STRING_FIELD(nick);
    DCC_GATEWAY_CLONE_STRING_FIELD(icon);
    DCC_GATEWAY_CLONE_STRING_FIELD(avatar);
    DCC_GATEWAY_CLONE_STRING_FIELD(banner);
    DCC_GATEWAY_CLONE_STRING_FIELD(vanity_url_code);
    DCC_GATEWAY_CLONE_STRING_FIELD(preferred_locale);
    DCC_GATEWAY_CLONE_STRING_FIELD(splash);
    DCC_GATEWAY_CLONE_STRING_FIELD(discovery_splash);
    DCC_GATEWAY_CLONE_STRING_FIELD(nonce);
    return 0;
}

#undef DCC_GATEWAY_CLONE_STRING_FIELD
