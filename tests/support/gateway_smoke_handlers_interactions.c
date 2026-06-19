#include "gateway_smoke_handlers.h"

#include <string.h>

static int interaction_ping_options_ok(const dcc_interaction_t *interaction) {
    int64_t count = 0;
    double ratio = 0.0;
    uint8_t visible = 0;
    dcc_snowflake_t target = 0;
    const char *text = dcc_interaction_option_string(interaction, "text");
    const dcc_interaction_option_t *count_option = dcc_interaction_option_by_name(interaction, "count");
    const dcc_user_t *resolved_user = dcc_interaction_resolved_user(interaction, 446);
    const dcc_member_t *resolved_member = dcc_interaction_resolved_member(interaction, 446);
    const dcc_role_t *resolved_role = dcc_interaction_resolved_role(interaction, 555);
    const dcc_channel_t *resolved_channel = dcc_interaction_resolved_channel(interaction, 222);
    const dcc_message_t *resolved_message = dcc_interaction_resolved_message(interaction, 111);
    const dcc_attachment_t *resolved_attachment = dcc_interaction_resolved_attachment(interaction, 888);
    const dcc_interaction_resolved_permission_t *resolved_permission =
        dcc_interaction_resolved_permission(interaction, 446);
    const dcc_entitlement_t *entitlement = dcc_interaction_entitlement(interaction, 700);
    return interaction != NULL &&
           interaction->message_id == 111 &&
           interaction->app_permissions == 8192 &&
           interaction->attachment_size_limit == 26214400U &&
           interaction->version == 1 &&
           interaction->has_context == 1 &&
           interaction->context == 0 &&
           interaction->locale != NULL &&
           strcmp(interaction->locale, "ko") == 0 &&
           interaction->guild_locale != NULL &&
           strcmp(interaction->guild_locale, "en-US") == 0 &&
           interaction->authorizing_integration_owners_count == 2 &&
           interaction->authorizing_integration_owners_truncated == 0 &&
           dcc_interaction_authorizing_integration_owner(interaction, 0) == 333 &&
           dcc_interaction_authorizing_integration_owner(interaction, 1) == 446 &&
           dcc_interaction_authorizing_integration_owner(interaction, 2) == 0 &&
           interaction->entitlements_count == 1 &&
           interaction->entitlements_truncated == 0 &&
           entitlement != NULL &&
           entitlement->id == 700 &&
           entitlement->sku_id == 123 &&
           entitlement->application_id == 666 &&
           entitlement->user_id == 446 &&
           entitlement->guild_id == 333 &&
           entitlement->type == 8 &&
           entitlement->flags == 2 &&
           entitlement->start_time != NULL &&
           strcmp(entitlement->start_time, "2026-06-15T00:00:00.000000+00:00") == 0 &&
           entitlement->end_time == NULL &&
           interaction->options != NULL &&
           interaction->options_count == 5 &&
           text != NULL &&
           strcmp(text, "hello") == 0 &&
           dcc_interaction_option_integer(interaction, "count", &count) &&
           count == 42 &&
           count_option != NULL &&
           count_option->type == 4 &&
           count_option->value_type == DCC_INTERACTION_OPTION_VALUE_INTEGER &&
           dcc_interaction_option_number(interaction, "ratio", &ratio) &&
           ratio > 1.49 &&
           ratio < 1.51 &&
           dcc_interaction_option_boolean(interaction, "visible", &visible) &&
           visible == 1 &&
           dcc_interaction_option_snowflake(interaction, "target", &target) &&
           target == 446 &&
           dcc_interaction_option_by_name(interaction, "missing") == NULL &&
           interaction->resolved.users_count == 1 &&
           interaction->resolved.members_count == 1 &&
           interaction->resolved.roles_count == 1 &&
           interaction->resolved.channels_count == 1 &&
           interaction->resolved.messages_count == 1 &&
           interaction->resolved.attachments_count == 1 &&
           interaction->resolved.member_permissions_count == 1 &&
           resolved_user != NULL &&
           resolved_user->username != NULL &&
           strcmp(resolved_user->username, "target-user") == 0 &&
           resolved_member != NULL &&
           resolved_member->role_ids != NULL &&
           resolved_member->role_ids_count == 1 &&
           resolved_member->role_ids[0] == 555 &&
           resolved_member->nick != NULL &&
           strcmp(resolved_member->nick, "TargetNick") == 0 &&
           resolved_role != NULL &&
           resolved_role->name != NULL &&
           strcmp(resolved_role->name, "mods") == 0 &&
           resolved_role->permissions == 8 &&
           resolved_channel != NULL &&
           resolved_channel->name != NULL &&
           strcmp(resolved_channel->name, "general") == 0 &&
           resolved_message != NULL &&
           resolved_message->content != NULL &&
           strcmp(resolved_message->content, "hello resolved") == 0 &&
           resolved_message->author.username != NULL &&
           strcmp(resolved_message->author.username, "target-user") == 0 &&
           resolved_attachment != NULL &&
           resolved_attachment->filename != NULL &&
           strcmp(resolved_attachment->filename, "clip.ogg") == 0 &&
           resolved_attachment->content_type != NULL &&
           strcmp(resolved_attachment->content_type, "audio/ogg") == 0 &&
           resolved_attachment->duration_secs > 1.24 &&
           resolved_attachment->duration_secs < 1.26 &&
           resolved_attachment->ephemeral == 1 &&
           resolved_permission != NULL &&
           resolved_permission->permissions == 2048;
}

static int interaction_autocomplete_option_ok(const dcc_interaction_t *interaction) {
    const dcc_interaction_option_t *focused = dcc_interaction_focused_option(interaction);
    const char *query = dcc_interaction_option_string(interaction, "query");
    return interaction != NULL &&
           interaction->options != NULL &&
           interaction->options_count == 1 &&
           focused != NULL &&
           focused == interaction->focused_option &&
           focused->focused == 1 &&
           focused->name != NULL &&
           strcmp(focused->name, "query") == 0 &&
           focused->type == 3 &&
           focused->value_type == DCC_INTERACTION_OPTION_VALUE_STRING &&
           query != NULL &&
           strcmp(query, "r") == 0;
}

void on_interaction_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction != NULL &&
        interaction->id == 555 &&
        interaction->application_id == 666 &&
        interaction->guild_id == 333 &&
        interaction->channel_id == 222 &&
        interaction->command_id == 777 &&
        interaction->type == 2 &&
        interaction->token != NULL &&
        strcmp(interaction->token, "interaction-token") == 0 &&
        interaction->name != NULL &&
        strcmp(interaction->name, "ping") == 0 &&
        interaction_ping_options_ok(interaction)) {
        dcc_interaction_t *copy = dcc_interaction_clone(interaction);
        if (copy != NULL &&
            copy->token != NULL &&
            strcmp(copy->token, "interaction-token") == 0 &&
            copy->locale != interaction->locale &&
            copy->authorizing_integration_owners != interaction->authorizing_integration_owners &&
            copy->entitlements != interaction->entitlements &&
            interaction_ping_options_ok(copy)) {
            state->interaction_seen = 1;
        }
        dcc_interaction_free(copy);
    }
}

void on_interaction_subtype(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL || dcc_event_data(event) == NULL) {
        return;
    }

    dcc_interaction_t *copy = dcc_interaction_clone(interaction);
    int copy_ok = copy != NULL &&
        copy->token != NULL &&
        copy->type == interaction->type &&
        copy->command_type == interaction->command_type &&
        copy->component_type == interaction->component_type &&
        copy->values_count == interaction->values_count;

    switch (dcc_event_type(event)) {
        case DCC_EVENT_SLASH_COMMAND:
            if (copy_ok &&
                interaction->id == 555 &&
                interaction->command_id == 777 &&
                interaction->type == 2 &&
                interaction->command_type == 1 &&
                interaction->name != NULL &&
                strcmp(interaction->name, "ping") == 0 &&
                interaction_ping_options_ok(interaction) &&
                interaction_ping_options_ok(copy)) {
                state->slash_command_seen = 1;
            }
            break;
        case DCC_EVENT_BUTTON_CLICK:
            if (copy_ok &&
                interaction->id == 556 &&
                interaction->type == 3 &&
                interaction->component_type == 2 &&
                interaction->custom_id != NULL &&
                strcmp(interaction->custom_id, "button-primary") == 0) {
                state->button_click_seen = 1;
            }
            break;
        case DCC_EVENT_AUTOCOMPLETE:
            if (copy_ok &&
                interaction->id == 558 &&
                interaction->type == 4 &&
                interaction->command_id == 780 &&
                interaction->command_type == 1 &&
                interaction->name != NULL &&
                strcmp(interaction->name, "complete") == 0 &&
                interaction_autocomplete_option_ok(interaction) &&
                interaction_autocomplete_option_ok(copy)) {
                state->autocomplete_seen = 1;
            }
            break;
        case DCC_EVENT_SELECT_CLICK:
            if (copy_ok &&
                interaction->id == 557 &&
                interaction->type == 3 &&
                interaction->component_type == 3 &&
                interaction->custom_id != NULL &&
                strcmp(interaction->custom_id, "select-colour") == 0 &&
                interaction->values != NULL &&
                interaction->values_count == 2 &&
                interaction->values[0] != NULL &&
                interaction->values[1] != NULL &&
                strcmp(interaction->values[0], "red") == 0 &&
                strcmp(interaction->values[1], "blue") == 0 &&
                copy->values != NULL &&
                copy->values[0] != NULL &&
                copy->values[1] != NULL &&
                strcmp(copy->values[0], "red") == 0 &&
                strcmp(copy->values[1], "blue") == 0) {
                state->select_click_seen = 1;
            }
            break;
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
            if (copy_ok &&
                interaction->id == 561 &&
                interaction->type == 2 &&
                interaction->command_type == 3 &&
                interaction->target_id == 111 &&
                interaction->name != NULL &&
                strcmp(interaction->name, "inspect_message") == 0) {
                state->message_context_menu_seen = 1;
            }
            break;
        case DCC_EVENT_USER_CONTEXT_MENU:
            if (copy_ok &&
                interaction->id == 560 &&
                interaction->type == 2 &&
                interaction->command_type == 2 &&
                interaction->target_id == 446 &&
                interaction->name != NULL &&
                strcmp(interaction->name, "inspect_user") == 0) {
                state->user_context_menu_seen = 1;
            }
            break;
        case DCC_EVENT_FORM_SUBMIT:
            if (copy_ok &&
                interaction->id == 559 &&
                interaction->type == 5 &&
                interaction->custom_id != NULL &&
                strcmp(interaction->custom_id, "modal-submit") == 0) {
                state->form_submit_seen = 1;
            }
            break;
        default:
            break;
    }
    dcc_interaction_free(copy);
}

void on_named_interaction_route(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    int *seen = (int *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (seen != NULL && interaction != NULL && dcc_event_data(event) != NULL) {
        *seen = 1;
    }
}
