#include "gateway_smoke_handlers.h"

#include <string.h>

void on_automod_action(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_auto_moderation_action_t *action = dcc_event_auto_moderation_action(event);
    if (data != NULL &&
        action != NULL &&
        dcc_event_type(event) == DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION &&
        data->guild_id == 333 &&
        data->action_type == 1 &&
        data->type == 1 &&
        data->rule_id == 777 &&
        data->trigger_type == 4 &&
        data->user_id == 446 &&
        data->channel_id == 222 &&
        data->message_id == 111 &&
        data->alert_system_message_id == 112 &&
        data->content != NULL &&
        strcmp(data->content, "blocked text") == 0 &&
        data->matched_keyword != NULL &&
        strcmp(data->matched_keyword, "bad") == 0 &&
        data->matched_content != NULL &&
        strcmp(data->matched_content, "bad text") == 0 &&
        action->guild_id == 333 &&
        action->action_type == 1 &&
        action->rule_id == 777 &&
        action->trigger_type == 4 &&
        action->user_id == 446 &&
        action->channel_id == 222 &&
        action->message_id == 111 &&
        action->alert_system_message_id == 112 &&
        action->content != NULL &&
        strcmp(action->content, "blocked text") == 0 &&
        action->matched_keyword != NULL &&
        strcmp(action->matched_keyword, "bad") == 0 &&
        action->matched_content != NULL &&
        strcmp(action->matched_content, "bad text") == 0) {
        dcc_auto_moderation_action_t *copy = dcc_auto_moderation_action_clone(action);
        if (copy != NULL &&
            copy->content != NULL &&
            strcmp(copy->content, "blocked text") == 0 &&
            copy->matched_content != NULL &&
            strcmp(copy->matched_content, "bad text") == 0) {
            state->automod_action_seen = 1;
        }
        dcc_auto_moderation_action_free(copy);
    }
}

void on_automod_rule_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_auto_moderation_rule_t *rule = dcc_event_auto_moderation_rule(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data == NULL || rule == NULL) {
        return;
    }

    if (type == DCC_EVENT_AUTO_MODERATION_RULE_CREATE &&
        data->rule_id == 778 &&
        data->guild_id == 333 &&
        data->name != NULL &&
        strcmp(data->name, "links") == 0 &&
        data->creator_id == 446 &&
        data->event_type == 1 &&
        data->trigger_type == 3 &&
        data->enabled == 1 &&
        data->role_ids != NULL &&
        data->role_ids_count == 1 &&
        data->role_ids[0] == 555 &&
        data->channel_ids != NULL &&
        data->channel_ids_count == 2 &&
        data->channel_ids[0] == 222 &&
        data->channel_ids[1] == 223 &&
        rule->id == 778 &&
        rule->guild_id == 333 &&
        rule->name != NULL &&
        strcmp(rule->name, "links") == 0 &&
        rule->creator_id == 446 &&
        rule->event_type == 1 &&
        rule->trigger_type == 3 &&
        rule->enabled == 1 &&
        rule->role_ids != NULL &&
        rule->role_ids_count == 1 &&
        rule->role_ids[0] == 555 &&
        rule->channel_ids != NULL &&
        rule->channel_ids_count == 2 &&
        rule->channel_ids[0] == 222 &&
        rule->channel_ids[1] == 223) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_auto_moderation_rule_t *rule_copy = dcc_auto_moderation_rule_clone(rule);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "links") == 0 &&
            copy->role_ids != NULL &&
            copy->role_ids_count == 1 &&
            copy->channel_ids != NULL &&
            copy->channel_ids_count == 2 &&
            rule_copy != NULL &&
            rule_copy->name != NULL &&
            strcmp(rule_copy->name, "links") == 0 &&
            rule_copy->role_ids != NULL &&
            rule_copy->role_ids_count == 1 &&
            rule_copy->channel_ids != NULL &&
            rule_copy->channel_ids_count == 2) {
            state->automod_rule_seen = 1;
        }
        dcc_auto_moderation_rule_free(rule_copy);
        dcc_gateway_event_data_free(copy);
    } else if ((type == DCC_EVENT_AUTO_MODERATION_RULE_UPDATE ||
                type == DCC_EVENT_AUTO_MODERATION_RULE_DELETE) &&
               data->rule_id == 778 &&
               data->guild_id == 333 &&
               data->name != NULL &&
               strcmp(data->name, "links") == 0 &&
               data->creator_id == 0 &&
               data->event_type == 0 &&
               data->trigger_type == 0 &&
               data->enabled == (type == DCC_EVENT_AUTO_MODERATION_RULE_UPDATE ? 1U : 0U) &&
               data->role_ids == NULL &&
               data->role_ids_count == 0 &&
               data->channel_ids == NULL &&
               data->channel_ids_count == 0 &&
               rule->id == 778 &&
               rule->guild_id == 333 &&
               rule->name != NULL &&
               strcmp(rule->name, "links") == 0 &&
               rule->creator_id == 0 &&
               rule->event_type == 0 &&
               rule->trigger_type == 0 &&
               rule->enabled == (type == DCC_EVENT_AUTO_MODERATION_RULE_UPDATE ? 1U : 0U) &&
               rule->role_ids == NULL &&
               rule->role_ids_count == 0 &&
               rule->channel_ids == NULL &&
               rule->channel_ids_count == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_auto_moderation_rule_t *rule_copy = dcc_auto_moderation_rule_clone(rule);
        if (copy != NULL &&
            copy->rule_id == 778 &&
            copy->name != NULL &&
            strcmp(copy->name, "links") == 0 &&
            copy->role_ids == NULL &&
            copy->channel_ids == NULL &&
            rule_copy != NULL &&
            rule_copy->id == 778 &&
            rule_copy->name != NULL &&
            strcmp(rule_copy->name, "links") == 0 &&
            rule_copy->role_ids == NULL &&
            rule_copy->channel_ids == NULL) {
            if (type == DCC_EVENT_AUTO_MODERATION_RULE_UPDATE) {
                state->automod_rule_update_seen = 1;
            } else {
                state->automod_rule_delete_seen = 1;
            }
        }
        dcc_auto_moderation_rule_free(rule_copy);
        dcc_gateway_event_data_free(copy);
    }
}
