#include "gateway_smoke_handlers.h"

#include <string.h>

void on_guild_delete(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_t *guild = dcc_event_guild(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    if (guild != NULL &&
        data != NULL &&
        guild->id == 333 &&
        data->id == 333 &&
        guild->name == NULL &&
        guild->unavailable == 1) {
        dcc_guild_t *copy = dcc_guild_clone(guild);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->id == 333 &&
            copy->name == NULL &&
            copy->unavailable == 1 &&
            data_copy != NULL &&
            data_copy->id == 333) {
            state->guild_delete_seen = 1;
        }
        dcc_gateway_event_data_free(data_copy);
        dcc_guild_free(copy);
    }
}

void on_guild_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_t *guild = dcc_event_guild(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    if (guild != NULL &&
        data != NULL &&
        dcc_event_type(event) == DCC_EVENT_GUILD_UPDATE &&
        guild->id == 333 &&
        guild->name != NULL &&
        strcmp(guild->name, "DCC Guild Updated") == 0 &&
        guild->owner_id == 447 &&
        guild->description != NULL &&
        strcmp(guild->description, "updated guild description") == 0 &&
        guild->icon != NULL &&
        strcmp(guild->icon, "guild-updated-icon") == 0 &&
        guild->banner != NULL &&
        strcmp(guild->banner, "guild-updated-banner") == 0 &&
        guild->application_id == 667 &&
        guild->afk_channel_id == 324 &&
        guild->system_channel_id == 325 &&
        guild->rules_channel_id == 326 &&
        guild->public_updates_channel_id == 327 &&
        guild->widget_channel_id == 328 &&
        guild->safety_alerts_channel_id == 329 &&
        guild->vanity_url_code != NULL &&
        strcmp(guild->vanity_url_code, "dcc-updated") == 0 &&
        guild->preferred_locale != NULL &&
        strcmp(guild->preferred_locale, "ko") == 0 &&
        guild->splash != NULL &&
        strcmp(guild->splash, "guild-updated-splash") == 0 &&
        guild->discovery_splash != NULL &&
        strcmp(guild->discovery_splash, "guild-updated-discovery") == 0 &&
        guild->flags == 32 &&
        guild->afk_timeout == 600 &&
        guild->max_presences == 6000 &&
        guild->max_members == 260000 &&
        guild->premium_subscription_count == 13 &&
        guild->max_video_channel_users == 50 &&
        guild->default_message_notifications == 0 &&
        guild->premium_tier == 3 &&
        guild->verification_level == 4 &&
        guild->explicit_content_filter == 1 &&
        guild->mfa_level == 1 &&
        guild->nsfw_level == 2 &&
        guild->widget_enabled == 0 &&
        guild->premium_progress_bar_enabled == 1 &&
        guild->member_count == 43 &&
        guild->large == 1 &&
        data->id == 333 &&
        data->name != NULL &&
        strcmp(data->name, "DCC Guild Updated") == 0 &&
        data->owner_id == 447 &&
        data->description != NULL &&
        strcmp(data->description, "updated guild description") == 0 &&
        data->icon != NULL &&
        strcmp(data->icon, "guild-updated-icon") == 0 &&
        data->banner != NULL &&
        strcmp(data->banner, "guild-updated-banner") == 0 &&
        data->application_id == 667 &&
        data->afk_channel_id == 324 &&
        data->system_channel_id == 325 &&
        data->rules_channel_id == 326 &&
        data->public_updates_channel_id == 327 &&
        data->widget_channel_id == 328 &&
        data->safety_alerts_channel_id == 329 &&
        data->vanity_url_code != NULL &&
        strcmp(data->vanity_url_code, "dcc-updated") == 0 &&
        data->preferred_locale != NULL &&
        strcmp(data->preferred_locale, "ko") == 0 &&
        data->splash != NULL &&
        strcmp(data->splash, "guild-updated-splash") == 0 &&
        data->discovery_splash != NULL &&
        strcmp(data->discovery_splash, "guild-updated-discovery") == 0 &&
        data->flags == 32 &&
        data->afk_timeout == 600 &&
        data->max_presences == 6000 &&
        data->max_members == 260000 &&
        data->premium_subscription_count == 13 &&
        data->max_video_channel_users == 50 &&
        data->default_message_notifications == 0 &&
        data->premium_tier == 3 &&
        data->verification_level == 4 &&
        data->explicit_content_filter == 1 &&
        data->mfa_level == 1 &&
        data->nsfw_level == 2 &&
        data->widget_enabled == 0 &&
        data->premium_progress_bar_enabled == 1) {
        dcc_guild_t *copy = dcc_guild_clone(guild);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "DCC Guild Updated") == 0 &&
            copy->description != NULL &&
            strcmp(copy->description, "updated guild description") == 0 &&
            copy->icon != NULL &&
            strcmp(copy->icon, "guild-updated-icon") == 0 &&
            copy->banner != NULL &&
            strcmp(copy->banner, "guild-updated-banner") == 0 &&
            copy->vanity_url_code != NULL &&
            strcmp(copy->vanity_url_code, "dcc-updated") == 0 &&
            copy->preferred_locale != NULL &&
            strcmp(copy->preferred_locale, "ko") == 0 &&
            data_copy != NULL &&
            data_copy->name != NULL &&
            strcmp(data_copy->name, "DCC Guild Updated") == 0 &&
            data_copy->description != NULL &&
            strcmp(data_copy->description, "updated guild description") == 0 &&
            data_copy->vanity_url_code != NULL &&
            strcmp(data_copy->vanity_url_code, "dcc-updated") == 0) {
            state->guild_update_seen = 1;
        }
        dcc_gateway_event_data_free(data_copy);
        dcc_guild_free(copy);
    }
}
