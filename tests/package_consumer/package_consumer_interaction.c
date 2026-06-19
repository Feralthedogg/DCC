#include "package_consumer_checks.h"

#include <stdint.h>
#include <string.h>

int dcc_package_consumer_check_interaction_api(void) {
    dcc_package_on_interaction_name_fn on_interaction_name = dcc_client_on_interaction_name;
    dcc_package_on_interaction_name_fn on_interaction_custom_id = dcc_client_on_interaction_custom_id;
    dcc_package_on_interaction_short_fn on_slash_command = dcc_client_on_slash_command;
    dcc_package_on_interaction_short_fn on_autocomplete = dcc_client_on_autocomplete;
    dcc_package_on_interaction_short_fn on_user_context_menu = dcc_client_on_user_context_menu;
    dcc_package_on_interaction_short_fn on_message_context_menu = dcc_client_on_message_context_menu;
    dcc_package_on_interaction_short_fn on_button = dcc_client_on_button;
    dcc_package_on_interaction_short_fn on_select = dcc_client_on_select;
    dcc_package_on_interaction_short_fn on_form_submit = dcc_client_on_form_submit;
    dcc_package_off_interaction_fn off_interaction = dcc_client_off_interaction;
    dcc_package_interaction_option_by_name_fn interaction_option_by_name = dcc_interaction_option_by_name;
    dcc_package_interaction_focused_option_fn interaction_focused_option = dcc_interaction_focused_option;
    dcc_package_interaction_option_child_by_name_fn interaction_option_child_by_name =
        dcc_interaction_option_child_by_name;
    dcc_package_interaction_subcommand_fn interaction_subcommand = dcc_interaction_subcommand;
    dcc_package_interaction_subcommand_fn interaction_subcommand_group = dcc_interaction_subcommand_group;
    dcc_package_interaction_option_string_fn interaction_option_string = dcc_interaction_option_string;
    dcc_package_interaction_option_integer_fn interaction_option_integer = dcc_interaction_option_integer;
    dcc_package_interaction_option_number_fn interaction_option_number = dcc_interaction_option_number;
    dcc_package_interaction_option_boolean_fn interaction_option_boolean = dcc_interaction_option_boolean;
    dcc_package_interaction_option_snowflake_fn interaction_option_snowflake =
        dcc_interaction_option_snowflake;
    dcc_package_interaction_resolved_user_fn interaction_resolved_user =
        dcc_interaction_resolved_user;
    dcc_package_interaction_resolved_member_fn interaction_resolved_member =
        dcc_interaction_resolved_member;
    dcc_package_interaction_resolved_role_fn interaction_resolved_role =
        dcc_interaction_resolved_role;
    dcc_package_interaction_resolved_channel_fn interaction_resolved_channel =
        dcc_interaction_resolved_channel;
    dcc_package_interaction_resolved_message_fn interaction_resolved_message =
        dcc_interaction_resolved_message;
    dcc_package_interaction_resolved_attachment_fn interaction_resolved_attachment =
        dcc_interaction_resolved_attachment;
    dcc_package_interaction_resolved_permission_fn interaction_resolved_permission =
        dcc_interaction_resolved_permission;
    dcc_package_interaction_authorizing_integration_owner_fn interaction_authorizing_owner =
        dcc_interaction_authorizing_integration_owner;
    dcc_package_interaction_entitlement_fn interaction_entitlement = dcc_interaction_entitlement;
    dcc_package_attachment_clone_fn attachment_clone = dcc_attachment_clone;
    dcc_package_attachment_free_fn attachment_free = dcc_attachment_free;

    const dcc_snowflake_t package_resolved_member_roles[] = {55};
    dcc_user_t package_resolved_users[1] = {
        { .id = 42, .username = "user" }
    };
    dcc_member_t package_resolved_members[1] = {
        {
            .guild_id = 1,
            .user = { .id = 42, .username = "member-user" },
            .role_ids = package_resolved_member_roles,
            .role_ids_count = 1
        }
    };
    dcc_role_t package_resolved_roles[1] = {
        { .id = 55, .guild_id = 1, .name = "role" }
    };
    dcc_channel_t package_resolved_channels[1] = {
        { .id = 66, .guild_id = 1, .name = "channel" }
    };
    dcc_message_t package_resolved_messages[1] = {
        { .id = 77, .channel_id = 66, .guild_id = 1, .content = "message" }
    };
    dcc_attachment_t package_resolved_attachments[1] = {
        {
            .id = 88,
            .size = 1024,
            .filename = "file.txt",
            .url = "https://cdn.example/file.txt",
            .content_type = "text/plain"
        }
    };
    dcc_interaction_resolved_permission_t package_resolved_permissions[1] = {
        { .user_id = 42, .permissions = 2048 }
    };
    dcc_interaction_authorizing_integration_owner_t package_authorizing_owners[2] = {
        { .type = 0, .owner_id = 1 },
        { .type = 1, .owner_id = 42 }
    };
    dcc_entitlement_t package_interaction_entitlements[1] = {
        {
            .id = 700,
            .sku_id = 123,
            .application_id = 666,
            .user_id = 42,
            .guild_id = 1,
            .type = 8,
            .flags = 2,
            .start_time = "2026-06-15T00:00:00.000000+00:00"
        }
    };
    dcc_attachment_t *package_attachment_copy = attachment_clone != NULL
        ? attachment_clone(&package_resolved_attachments[0])
        : NULL;
    dcc_interaction_option_t package_interaction_options[5] = {
        {
            .name = "name",
            .type = 3,
            .value_type = DCC_INTERACTION_OPTION_VALUE_STRING,
            .string_value = "value"
        },
        {
            .name = "count",
            .type = 4,
            .value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER,
            .integer_value = 7
        },
        {
            .name = "ratio",
            .type = 10,
            .value_type = DCC_INTERACTION_OPTION_VALUE_NUMBER,
            .number_value = 2.5
        },
        {
            .name = "visible",
            .type = 5,
            .value_type = DCC_INTERACTION_OPTION_VALUE_BOOLEAN,
            .boolean_value = 1
        },
        {
            .name = "target",
            .type = 6,
            .value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE,
            .snowflake_value = 42,
            .focused = 1
        }
    };
    dcc_interaction_t package_interaction = {
        .message_id = 77,
        .app_permissions = 8192,
        .attachment_size_limit = DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT,
        .version = 1,
        .context = 0,
        .has_context = 1,
        .locale = "ko",
        .guild_locale = "en-US",
        .options = package_interaction_options,
        .options_count = 5,
        .authorizing_integration_owners = package_authorizing_owners,
        .authorizing_integration_owners_count = 2,
        .entitlements = package_interaction_entitlements,
        .entitlements_count = 1,
        .focused_option = &package_interaction_options[4],
        .resolved = {
            .users = package_resolved_users,
            .users_count = 1,
            .members = package_resolved_members,
            .members_count = 1,
            .roles = package_resolved_roles,
            .roles_count = 1,
            .channels = package_resolved_channels,
            .channels_count = 1,
            .messages = package_resolved_messages,
            .messages_count = 1,
            .attachments = package_resolved_attachments,
            .attachments_count = 1,
            .member_permissions = package_resolved_permissions,
            .member_permissions_count = 1
        }
    };
    dcc_interaction_option_t package_nested_leaf_options[1] = {
        {
            .name = "reason",
            .type = 3,
            .value_type = DCC_INTERACTION_OPTION_VALUE_STRING,
            .string_value = "because"
        }
    };
    dcc_interaction_option_t package_nested_subcommands[1] = {
        {
            .name = "ban",
            .type = 1,
            .options = package_nested_leaf_options,
            .options_count = 1
        }
    };
    dcc_interaction_option_t package_nested_groups[1] = {
        {
            .name = "admin",
            .type = 2,
            .options = package_nested_subcommands,
            .options_count = 1
        }
    };
    dcc_interaction_t package_nested_interaction = {
        .options = package_nested_groups,
        .options_count = 1
    };
    int64_t package_option_integer = 0;
    double package_option_number = 0.0;
    uint8_t package_option_boolean = 0;
    dcc_snowflake_t package_option_snowflake = 0;

    return on_interaction_name != NULL &&
                   on_interaction_custom_id != NULL &&
                   on_slash_command != NULL &&
                   on_autocomplete != NULL &&
                   on_user_context_menu != NULL &&
                   on_message_context_menu != NULL &&
                   on_button != NULL &&
                   on_select != NULL &&
                   on_form_submit != NULL &&
                   off_interaction != NULL &&
                   on_interaction_name(NULL, DCC_EVENT_SLASH_COMMAND, "ping", NULL, NULL, NULL) ==
                       DCC_ERR_INVALID_ARG &&
                   on_interaction_custom_id(NULL, DCC_EVENT_BUTTON_CLICK, "x", NULL, NULL, NULL) ==
                       DCC_ERR_INVALID_ARG &&
                   off_interaction(NULL, DCC_EVENT_SLASH_COMMAND, 1) == DCC_ERR_INVALID_ARG &&
                   interaction_option_by_name != NULL &&
                   interaction_focused_option != NULL &&
                   interaction_option_child_by_name != NULL &&
                   interaction_subcommand != NULL &&
                   interaction_subcommand_group != NULL &&
                   interaction_option_string != NULL &&
                   interaction_option_integer != NULL &&
                   interaction_option_number != NULL &&
                   interaction_option_boolean != NULL &&
                   interaction_option_snowflake != NULL &&
                   interaction_resolved_user != NULL &&
                   interaction_resolved_member != NULL &&
                   interaction_resolved_role != NULL &&
                   interaction_resolved_channel != NULL &&
                   interaction_resolved_message != NULL &&
                   interaction_resolved_attachment != NULL &&
                   interaction_resolved_permission != NULL &&
                   interaction_authorizing_owner != NULL &&
                   interaction_entitlement != NULL &&
                   attachment_clone != NULL &&
                   attachment_free != NULL &&
                   DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT == 10485760U &&
                   package_attachment_copy != NULL &&
                   package_attachment_copy->filename != NULL &&
                   strcmp(package_attachment_copy->filename, "file.txt") == 0 &&
                   (attachment_free(package_attachment_copy), 1) &&
                   interaction_option_by_name(&package_interaction, "count") ==
                       &package_interaction_options[1] &&
                   interaction_focused_option(&package_interaction) == &package_interaction_options[4] &&
                   interaction_option_by_name(&package_nested_interaction, "reason") ==
                       &package_nested_leaf_options[0] &&
                   interaction_option_child_by_name(
                       &package_nested_groups[0],
                       "ban"
                   ) == &package_nested_subcommands[0] &&
                   interaction_subcommand(&package_nested_interaction) == &package_nested_subcommands[0] &&
                   interaction_subcommand_group(&package_nested_interaction) == &package_nested_groups[0] &&
                   interaction_option_string(&package_nested_interaction, "reason") != NULL &&
                   strcmp(
                       interaction_option_string(&package_nested_interaction, "reason"),
                       "because"
                   ) == 0 &&
                   interaction_option_string(&package_interaction, "name") != NULL &&
                   strcmp(interaction_option_string(&package_interaction, "name"), "value") == 0 &&
                   interaction_option_integer(
                       &package_interaction,
                       "count",
                       &package_option_integer
                   ) == 1 &&
                   package_option_integer == 7 &&
                   interaction_option_number(
                       &package_interaction,
                       "ratio",
                       &package_option_number
                   ) == 1 &&
                   package_option_number > 2.49 &&
                   package_option_number < 2.51 &&
                   interaction_option_boolean(
                       &package_interaction,
                       "visible",
                       &package_option_boolean
                   ) == 1 &&
                   package_option_boolean == 1 &&
                   interaction_option_snowflake(
                       &package_interaction,
                       "target",
                       &package_option_snowflake
                   ) == 1 &&
                   package_option_snowflake == 42 &&
                   interaction_option_string(&package_interaction, "missing") == NULL &&
                   interaction_resolved_user(&package_interaction, 42) == &package_resolved_users[0] &&
                   interaction_resolved_member(&package_interaction, 42) == &package_resolved_members[0] &&
                   interaction_resolved_role(&package_interaction, 55) == &package_resolved_roles[0] &&
                   interaction_resolved_channel(&package_interaction, 66) ==
                       &package_resolved_channels[0] &&
                   interaction_resolved_message(&package_interaction, 77) ==
                       &package_resolved_messages[0] &&
                   interaction_resolved_attachment(&package_interaction, 88) ==
                       &package_resolved_attachments[0] &&
                   interaction_resolved_permission(&package_interaction, 42) ==
                       &package_resolved_permissions[0] &&
                   interaction_resolved_permission(&package_interaction, 42)->permissions == 2048 &&
                   package_interaction.message_id == 77 &&
                   package_interaction.app_permissions == 8192 &&
                   package_interaction.attachment_size_limit ==
                       DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT &&
                   package_interaction.version == 1 &&
                   package_interaction.has_context == 1 &&
                   package_interaction.context == 0 &&
                   package_interaction.locale != NULL &&
                   strcmp(package_interaction.locale, "ko") == 0 &&
                   interaction_authorizing_owner(&package_interaction, 0) == 1 &&
                   interaction_authorizing_owner(&package_interaction, 1) == 42 &&
                   interaction_authorizing_owner(&package_interaction, 2) == 0 &&
                   interaction_entitlement(&package_interaction, 700) ==
                       &package_interaction_entitlements[0] &&
                   interaction_entitlement(&package_interaction, 700)->flags == 2 &&
                   interaction_entitlement(&package_interaction, 999) == NULL &&
                   interaction_resolved_user(&package_interaction, 99) == NULL &&
                   interaction_resolved_attachment(&package_interaction, 99) == NULL &&
                   interaction_resolved_permission(&package_interaction, 99) == NULL
               ? 1
               : 0;
}
