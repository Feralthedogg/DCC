#ifndef DCC_TESTS_SUPPORT_APP_SMOKE_INTERACTIONS_H
#define DCC_TESTS_SUPPORT_APP_SMOKE_INTERACTIONS_H

#include <dcc/dcc.h>

#include <stdint.h>
#include <string.h>

typedef struct app_smoke_interactions {
    dcc_event_t resumed_event;
    dcc_event_t ready_event;
    dcc_event_t message_event;
    dcc_event_t message_update_event;

    dcc_interaction_option_t slash_options[9];
    dcc_user_t slash_resolved_users[1];
    dcc_member_t slash_resolved_members[1];
    dcc_role_t slash_resolved_roles[1];
    dcc_channel_t slash_resolved_channels[1];
    dcc_message_t slash_resolved_messages[1];
    dcc_attachment_t slash_resolved_attachments[1];
    dcc_interaction_t slash;
    dcc_interaction_option_t typed_options[2];
    dcc_interaction_t typed_slash;
    dcc_interaction_option_t typed_subcommand_values[2];
    dcc_interaction_option_t typed_subcommand_child[1];
    dcc_interaction_option_t typed_subcommand_group[1];
    dcc_interaction_t typed_subcommand;
    dcc_interaction_option_t subcommand_options[1];
    dcc_interaction_option_t group_children[1];
    dcc_interaction_option_t admin_options[1];
    dcc_interaction_t admin;
    dcc_interaction_option_t autocomplete_options[1];
    dcc_interaction_t autocomplete;
    dcc_interaction_option_t typed_autocomplete_options[1];
    dcc_interaction_t typed_autocomplete;
    dcc_interaction_option_t typed_subcommand_autocomplete_values[2];
    dcc_interaction_option_t typed_subcommand_autocomplete_child[1];
    dcc_interaction_option_t typed_subcommand_autocomplete_group[1];
    dcc_interaction_t typed_subcommand_autocomplete;

    dcc_interaction_t button;
    dcc_interaction_t denied_button;
    dcc_interaction_t cooldown_button;
    dcc_interaction_t check_button;
    dcc_interaction_t rejected_check_button;
    dcc_interaction_t dm_only_button;
    dcc_interaction_t nsfw_button;
    dcc_interaction_t nsfw_rejected_button;
    dcc_interaction_t channel_type_button;
    dcc_interaction_t channel_type_rejected_button;
    dcc_interaction_t role_button;
    dcc_interaction_t role_rejected_button;
    dcc_interaction_t typed_button;
    const char *typed_select_values[2];
    dcc_interaction_t typed_select;
    dcc_interaction_t extension_slash;
    dcc_interaction_t extension_button;
    dcc_interaction_t view_button;
    dcc_interaction_t module_button;
    dcc_interaction_t group_button;
    dcc_interaction_t group_select;
    dcc_interaction_t group_view_button;

    const char *tag_values[2];
    dcc_interaction_form_field_t fields[3];
    dcc_interaction_t modal;
    dcc_interaction_form_field_t typed_modal_fields[2];
    dcc_interaction_t typed_modal;
    dcc_interaction_t group_modal;
    char session_custom_id[DCC_COMPONENT_SESSION_CUSTOM_ID_MAX + 1U];
    dcc_interaction_t session_button;
} app_smoke_interactions_t;

static void app_smoke_init_base_command(dcc_interaction_t *interaction) {
    memset(interaction, 0, sizeof(*interaction));
    interaction->id = 100U;
    interaction->application_id = 200U;
    interaction->guild_id = 300U;
    interaction->channel_id = 400U;
    interaction->command_id = 500U;
    interaction->user_id = 803U;
    interaction->member_permissions = DCC_PERMISSION_MANAGE_MESSAGES;
    interaction->has_member_permissions = 1U;
    interaction->type = 2U;
    interaction->command_type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
}

static void app_smoke_init_base_button(dcc_interaction_t *interaction) {
    memset(interaction, 0, sizeof(*interaction));
    interaction->type = 3U;
    interaction->component_type = 2U;
    interaction->guild_id = 300U;
    interaction->user_id = 803U;
    interaction->member_permissions = DCC_PERMISSION_MANAGE_MESSAGES;
    interaction->has_member_permissions = 1U;
    interaction->message_id = 600U;
    interaction->custom_id = "page:next";
}

static int app_smoke_interactions_init(
    app_smoke_interactions_t *fx,
    dcc_component_session_t *session
) {
    memset(fx, 0, sizeof(*fx));

    fx->resumed_event.type = DCC_EVENT_RESUMED;
    fx->ready_event.type = DCC_EVENT_READY;
    fx->ready_event.data.ready.shard_id = 1U;
    fx->ready_event.data.ready.shard_count = 2U;
    fx->ready_event.data.ready.session_id = "ready-session";
    fx->message_event.type = DCC_EVENT_MESSAGE_CREATE;
    fx->message_event.data.message.id = 700U;
    fx->message_event.data.message.content = "hello app";
    fx->message_update_event.type = DCC_EVENT_MESSAGE_UPDATE;
    fx->message_update_event.data.message.id = 701U;
    fx->message_update_event.data.message.content = "hello extension";

    fx->slash_options[0].name = "name";
    fx->slash_options[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->slash_options[0].string_value = "feral";
    fx->slash_options[1].name = "count";
    fx->slash_options[1].value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER;
    fx->slash_options[1].integer_value = 7;
    fx->slash_options[2].name = "flag";
    fx->slash_options[2].value_type = DCC_INTERACTION_OPTION_VALUE_BOOLEAN;
    fx->slash_options[2].boolean_value = 1U;
    fx->slash_options[3].name = "user";
    fx->slash_options[3].value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
    fx->slash_options[3].snowflake_value = 42U;
    fx->slash_options[4].name = "ratio";
    fx->slash_options[4].value_type = DCC_INTERACTION_OPTION_VALUE_NUMBER;
    fx->slash_options[4].number_value = 0.25;
    fx->slash_options[5].name = "role";
    fx->slash_options[5].value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
    fx->slash_options[5].snowflake_value = 910U;
    fx->slash_options[6].name = "channel";
    fx->slash_options[6].value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
    fx->slash_options[6].snowflake_value = 401U;
    fx->slash_options[7].name = "message";
    fx->slash_options[7].value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
    fx->slash_options[7].snowflake_value = 700U;
    fx->slash_options[8].name = "attachment";
    fx->slash_options[8].value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
    fx->slash_options[8].snowflake_value = 88U;
    fx->slash_resolved_users[0] = (dcc_user_t){
        .id = 42U,
        .username = "option-user",
        .global_name = "Option User"
    };
    fx->slash_resolved_members[0] = (dcc_member_t){
        .guild_id = 300U,
        .user = { .id = 42U, .username = "option-member" },
        .nick = "Option Member"
    };
    fx->slash_resolved_roles[0] = (dcc_role_t){
        .id = 910U,
        .guild_id = 300U,
        .name = "Mod"
    };
    fx->slash_resolved_channels[0] = (dcc_channel_t){
        .id = 401U,
        .guild_id = 300U,
        .type = DCC_CHANNEL_TEXT,
        .name = "general"
    };
    fx->slash_resolved_messages[0] = (dcc_message_t){
        .id = 700U,
        .channel_id = 401U,
        .guild_id = 300U,
        .content = "resolved message"
    };
    fx->slash_resolved_attachments[0] = (dcc_attachment_t){
        .id = 88U,
        .filename = "resolved.txt"
    };
    app_smoke_init_base_command(&fx->slash);
    fx->slash.token = "slash-token";
    fx->slash.name = "hello";
    fx->slash.options = fx->slash_options;
    fx->slash.options_count = DCC_ARRAY_LEN(fx->slash_options);
    fx->slash.target_id = 42U;
    fx->slash.resolved.users = fx->slash_resolved_users;
    fx->slash.resolved.users_count = DCC_ARRAY_LEN(fx->slash_resolved_users);
    fx->slash.resolved.members = fx->slash_resolved_members;
    fx->slash.resolved.members_count = DCC_ARRAY_LEN(fx->slash_resolved_members);
    fx->slash.resolved.roles = fx->slash_resolved_roles;
    fx->slash.resolved.roles_count = DCC_ARRAY_LEN(fx->slash_resolved_roles);
    fx->slash.resolved.channels = fx->slash_resolved_channels;
    fx->slash.resolved.channels_count = DCC_ARRAY_LEN(fx->slash_resolved_channels);
    fx->slash.resolved.messages = fx->slash_resolved_messages;
    fx->slash.resolved.messages_count = DCC_ARRAY_LEN(fx->slash_resolved_messages);
    fx->slash.resolved.attachments = fx->slash_resolved_attachments;
    fx->slash.resolved.attachments_count = DCC_ARRAY_LEN(fx->slash_resolved_attachments);

    fx->typed_options[0].name = "name";
    fx->typed_options[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->typed_options[0].string_value = "typed-feral";
    fx->typed_options[1].name = "limit";
    fx->typed_options[1].value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER;
    fx->typed_options[1].integer_value = 12;
    fx->typed_slash = fx->slash;
    fx->typed_slash.id = 104U;
    fx->typed_slash.command_id = 503U;
    fx->typed_slash.name = "typed";
    fx->typed_slash.options = fx->typed_options;
    fx->typed_slash.options_count = DCC_ARRAY_LEN(fx->typed_options);

    fx->typed_subcommand_values[0].name = "name";
    fx->typed_subcommand_values[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->typed_subcommand_values[0].string_value = "typed-sub-feral";
    fx->typed_subcommand_values[1].name = "limit";
    fx->typed_subcommand_values[1].value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER;
    fx->typed_subcommand_values[1].integer_value = 15;
    fx->typed_subcommand_child[0].name = "ban";
    fx->typed_subcommand_child[0].type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND;
    fx->typed_subcommand_child[0].options = fx->typed_subcommand_values;
    fx->typed_subcommand_child[0].options_count = DCC_ARRAY_LEN(fx->typed_subcommand_values);
    fx->typed_subcommand_group[0].name = "moderation";
    fx->typed_subcommand_group[0].type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP;
    fx->typed_subcommand_group[0].options = fx->typed_subcommand_child;
    fx->typed_subcommand_group[0].options_count = DCC_ARRAY_LEN(fx->typed_subcommand_child);
    fx->typed_subcommand = fx->slash;
    fx->typed_subcommand.id = 105U;
    fx->typed_subcommand.command_id = 504U;
    fx->typed_subcommand.name = "typed_admin";
    fx->typed_subcommand.options = fx->typed_subcommand_group;
    fx->typed_subcommand.options_count = DCC_ARRAY_LEN(fx->typed_subcommand_group);

    fx->subcommand_options[0].name = "reason";
    fx->subcommand_options[0].type = DCC_APPLICATION_COMMAND_OPTION_STRING;
    fx->subcommand_options[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->subcommand_options[0].string_value = "spam";
    fx->group_children[0].name = "ban";
    fx->group_children[0].type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND;
    fx->group_children[0].options = fx->subcommand_options;
    fx->group_children[0].options_count = DCC_ARRAY_LEN(fx->subcommand_options);
    fx->admin_options[0].name = "moderation";
    fx->admin_options[0].type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP;
    fx->admin_options[0].options = fx->group_children;
    fx->admin_options[0].options_count = DCC_ARRAY_LEN(fx->group_children);
    app_smoke_init_base_command(&fx->admin);
    fx->admin.id = 102U;
    fx->admin.command_id = 501U;
    fx->admin.token = "admin-token";
    fx->admin.name = "admin";
    fx->admin.options = fx->admin_options;
    fx->admin.options_count = DCC_ARRAY_LEN(fx->admin_options);

    fx->autocomplete_options[0].name = "name";
    fx->autocomplete_options[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->autocomplete_options[0].string_value = "fe";
    fx->autocomplete_options[0].focused = 1U;
    app_smoke_init_base_command(&fx->autocomplete);
    fx->autocomplete.id = 101U;
    fx->autocomplete.type = 4U;
    fx->autocomplete.token = "autocomplete-token";
    fx->autocomplete.name = "hello";
    fx->autocomplete.options = fx->autocomplete_options;
    fx->autocomplete.options_count = DCC_ARRAY_LEN(fx->autocomplete_options);

    fx->typed_autocomplete_options[0].name = "name";
    fx->typed_autocomplete_options[0].value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->typed_autocomplete_options[0].string_value = "typed-auto-fe";
    fx->typed_autocomplete_options[0].focused = 1U;
    fx->typed_autocomplete = fx->autocomplete;
    fx->typed_autocomplete.id = 106U;
    fx->typed_autocomplete.command_id = 505U;
    fx->typed_autocomplete.name = "typed_search";
    fx->typed_autocomplete.options = fx->typed_autocomplete_options;
    fx->typed_autocomplete.options_count = DCC_ARRAY_LEN(fx->typed_autocomplete_options);

    fx->typed_subcommand_autocomplete_values[0].name = "name";
    fx->typed_subcommand_autocomplete_values[0].value_type =
        DCC_INTERACTION_OPTION_VALUE_STRING;
    fx->typed_subcommand_autocomplete_values[0].string_value = "typed-sub-auto-fe";
    fx->typed_subcommand_autocomplete_values[0].focused = 1U;
    fx->typed_subcommand_autocomplete_values[1].name = "limit";
    fx->typed_subcommand_autocomplete_values[1].value_type =
        DCC_INTERACTION_OPTION_VALUE_INTEGER;
    fx->typed_subcommand_autocomplete_values[1].integer_value = 18;
    fx->typed_subcommand_autocomplete_child[0].name = "ban";
    fx->typed_subcommand_autocomplete_child[0].type =
        DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND;
    fx->typed_subcommand_autocomplete_child[0].options =
        fx->typed_subcommand_autocomplete_values;
    fx->typed_subcommand_autocomplete_child[0].options_count =
        DCC_ARRAY_LEN(fx->typed_subcommand_autocomplete_values);
    fx->typed_subcommand_autocomplete_group[0].name = "moderation";
    fx->typed_subcommand_autocomplete_group[0].type =
        DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP;
    fx->typed_subcommand_autocomplete_group[0].options =
        fx->typed_subcommand_autocomplete_child;
    fx->typed_subcommand_autocomplete_group[0].options_count =
        DCC_ARRAY_LEN(fx->typed_subcommand_autocomplete_child);
    fx->typed_subcommand_autocomplete = fx->autocomplete;
    fx->typed_subcommand_autocomplete.id = 107U;
    fx->typed_subcommand_autocomplete.command_id = 506U;
    fx->typed_subcommand_autocomplete.name = "typed_admin";
    fx->typed_subcommand_autocomplete.options = fx->typed_subcommand_autocomplete_group;
    fx->typed_subcommand_autocomplete.options_count =
        DCC_ARRAY_LEN(fx->typed_subcommand_autocomplete_group);

    app_smoke_init_base_button(&fx->button);
    fx->denied_button = fx->button;
    fx->denied_button.user_id = 404U;
    fx->cooldown_button = fx->button;
    fx->cooldown_button.message_id = 609U;
    fx->cooldown_button.custom_id = "cooldown:test";
    fx->check_button = fx->button;
    fx->check_button.message_id = 610U;
    fx->check_button.custom_id = "check:ok";
    fx->rejected_check_button = fx->button;
    fx->rejected_check_button.message_id = 611U;
    fx->rejected_check_button.custom_id = "check:deny";
    fx->dm_only_button = fx->button;
    fx->dm_only_button.message_id = 612U;
    fx->dm_only_button.custom_id = "dm:only";
    fx->nsfw_button = fx->button;
    fx->nsfw_button.message_id = 613U;
    fx->nsfw_button.channel_id = 401U;
    fx->nsfw_button.custom_id = "nsfw:ok";
    fx->nsfw_rejected_button = fx->button;
    fx->nsfw_rejected_button.message_id = 614U;
    fx->nsfw_rejected_button.channel_id = 402U;
    fx->nsfw_rejected_button.custom_id = "nsfw:deny";
    fx->channel_type_button = fx->button;
    fx->channel_type_button.message_id = 615U;
    fx->channel_type_button.channel_id = 401U;
    fx->channel_type_button.custom_id = "channel:text";
    fx->channel_type_rejected_button = fx->button;
    fx->channel_type_rejected_button.message_id = 616U;
    fx->channel_type_rejected_button.channel_id = 403U;
    fx->channel_type_rejected_button.custom_id = "channel:voice";
    fx->role_button = fx->button;
    fx->role_button.message_id = 617U;
    fx->role_button.channel_id = 401U;
    fx->role_button.custom_id = "role:ok";
    fx->role_rejected_button = fx->button;
    fx->role_rejected_button.message_id = 618U;
    fx->role_rejected_button.channel_id = 401U;
    fx->role_rejected_button.custom_id = "role:deny";
    fx->typed_button = fx->button;
    fx->typed_button.message_id = 607U;
    fx->typed_button.custom_id = "typed.button:confirm";
    fx->typed_select_values[0] = "alpha";
    fx->typed_select_values[1] = "beta";
    fx->typed_select = fx->button;
    fx->typed_select.component_type = 3U;
    fx->typed_select.message_id = 608U;
    fx->typed_select.custom_id = "typed.select";
    fx->typed_select.values = fx->typed_select_values;
    fx->typed_select.values_count = DCC_ARRAY_LEN(fx->typed_select_values);
    fx->extension_slash = fx->slash;
    fx->extension_slash.id = 103U;
    fx->extension_slash.command_id = 502U;
    fx->extension_slash.name = "exthello";
    fx->extension_slash.options = NULL;
    fx->extension_slash.options_count = 0U;
    fx->extension_button = fx->button;
    fx->extension_button.message_id = 606U;
    fx->extension_button.custom_id = "extension:open";

    app_smoke_init_base_button(&fx->view_button);
    fx->view_button.message_id = 601U;
    fx->view_button.custom_id = "view:confirm";
    app_smoke_init_base_button(&fx->module_button);
    fx->module_button.message_id = 602U;
    fx->module_button.custom_id = "module:ping";
    app_smoke_init_base_button(&fx->group_button);
    fx->group_button.message_id = 603U;
    fx->group_button.custom_id = "settings:open";
    app_smoke_init_base_button(&fx->group_select);
    fx->group_select.component_type = 3U;
    fx->group_select.message_id = 604U;
    fx->group_select.custom_id = "settings:mode";
    app_smoke_init_base_button(&fx->group_view_button);
    fx->group_view_button.message_id = 605U;
    fx->group_view_button.custom_id = "settings:view";

    fx->tag_values[0] = "alpha";
    fx->tag_values[1] = "beta";
    fx->fields[0].custom_id = "birthday";
    fx->fields[0].value_type = DCC_INTERACTION_FORM_VALUE_STRING;
    fx->fields[0].value = "20070101";
    fx->fields[1].custom_id = "public";
    fx->fields[1].value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN;
    fx->fields[1].boolean_value = 1U;
    fx->fields[2].custom_id = "tags";
    fx->fields[2].value_type = DCC_INTERACTION_FORM_VALUE_VALUES;
    fx->fields[2].values = fx->tag_values;
    fx->fields[2].values_count = DCC_ARRAY_LEN(fx->tag_values);
    memset(&fx->modal, 0, sizeof(fx->modal));
    fx->modal.type = 5U;
    fx->modal.guild_id = 300U;
    fx->modal.user_id = 803U;
    fx->modal.member_permissions = DCC_PERMISSION_MANAGE_MESSAGES;
    fx->modal.has_member_permissions = 1U;
    fx->modal.custom_id = "birthday.modal";
    fx->modal.form_fields = fx->fields;
    fx->modal.form_fields_count = DCC_ARRAY_LEN(fx->fields);

    fx->typed_modal_fields[0].custom_id = "birthday";
    fx->typed_modal_fields[0].value_type = DCC_INTERACTION_FORM_VALUE_STRING;
    fx->typed_modal_fields[0].value = "20080102";
    fx->typed_modal_fields[1].custom_id = "public";
    fx->typed_modal_fields[1].value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN;
    fx->typed_modal_fields[1].boolean_value = 0U;
    fx->typed_modal = fx->modal;
    fx->typed_modal.custom_id = "typed.birthday";
    fx->typed_modal.form_fields = fx->typed_modal_fields;
    fx->typed_modal.form_fields_count = DCC_ARRAY_LEN(fx->typed_modal_fields);
    fx->group_modal = fx->modal;
    fx->group_modal.custom_id = "settings:edit";

    if (dcc_component_session_custom_id(
            session,
            "next",
            fx->session_custom_id,
            sizeof(fx->session_custom_id)
        ) != DCC_OK) {
        return 0;
    }
    app_smoke_init_base_button(&fx->session_button);
    fx->session_button.channel_id = 400U;
    fx->session_button.custom_id = fx->session_custom_id;
    return 1;
}

#endif
