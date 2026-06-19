#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

void dcc_gateway_event_data_free_strings(dcc_gateway_event_data_t *data) {
    if (data == NULL) {
        return;
    }
    dcc_free_user_fields(&data->user);
    free((void *)data->name);
    free((void *)data->description);
    free((void *)data->topic);
    free((void *)data->type_text);
    free((void *)data->desktop_status);
    free((void *)data->mobile_status);
    free((void *)data->web_status);
    free((void *)data->code);
    free((void *)data->status);
    free((void *)data->token);
    free((void *)data->endpoint);
    free((void *)data->emoji_name);
    free((void *)data->reason);
    free((void *)data->content);
    free((void *)data->matched_keyword);
    free((void *)data->matched_content);
    free((void *)data->timestamp_text);
    free((void *)data->join_timestamp);
    free((void *)data->created_at);
    free((void *)data->expires_at);
    free((void *)data->start_time);
    free((void *)data->end_time);
    free((void *)data->current_period_start);
    free((void *)data->current_period_end);
    free((void *)data->canceled_at);
    free((void *)data->joined_at);
    free((void *)data->premium_since);
    free((void *)data->communication_disabled_until);
    free((void *)data->nick);
    free((void *)data->icon);
    free((void *)data->avatar);
    free((void *)data->banner);
    free((void *)data->vanity_url_code);
    free((void *)data->preferred_locale);
    free((void *)data->splash);
    free((void *)data->discovery_splash);
    free((void *)data->nonce);
}
