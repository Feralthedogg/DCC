#include "internal/objects/dcc_objects_internal.h"

void dcc_gateway_event_data_clear_owned_refs(dcc_gateway_event_data_t *data) {
    if (data == NULL) {
        return;
    }

    data->user.username = NULL;
    data->user.global_name = NULL;
    data->user.avatar = NULL;
    data->ids = NULL;
    data->channel_ids = NULL;
    data->removed_member_ids = NULL;
    data->member_ids = NULL;
    data->added_members = NULL;
    data->members = NULL;
    data->role_ids = NULL;
    data->not_found_ids = NULL;
    data->sku_ids = NULL;
    data->entitlement_ids = NULL;
    data->activities = NULL;
    data->name = NULL;
    data->description = NULL;
    data->topic = NULL;
    data->type_text = NULL;
    data->desktop_status = NULL;
    data->mobile_status = NULL;
    data->web_status = NULL;
    data->code = NULL;
    data->status = NULL;
    data->token = NULL;
    data->endpoint = NULL;
    data->emoji_name = NULL;
    data->reason = NULL;
    data->content = NULL;
    data->matched_keyword = NULL;
    data->matched_content = NULL;
    data->timestamp_text = NULL;
    data->join_timestamp = NULL;
    data->created_at = NULL;
    data->expires_at = NULL;
    data->start_time = NULL;
    data->end_time = NULL;
    data->current_period_start = NULL;
    data->current_period_end = NULL;
    data->canceled_at = NULL;
    data->joined_at = NULL;
    data->premium_since = NULL;
    data->communication_disabled_until = NULL;
    data->nick = NULL;
    data->icon = NULL;
    data->avatar = NULL;
    data->banner = NULL;
    data->vanity_url_code = NULL;
    data->preferred_locale = NULL;
    data->splash = NULL;
    data->discovery_splash = NULL;
    data->nonce = NULL;
}
