#include "package_consumer_checks.h"

#include <string.h>

int main(void) {
    const char *version = dcc_version_string();
    const dcc_application_role_connection_metadata_params_t metadata = {
        .size = sizeof(dcc_application_role_connection_metadata_params_t),
        .type = DCC_APPLICATION_ROLE_CONNECTION_METADATA_BOOLEAN_EQUAL,
        .key = "verified",
        .name = "Verified",
        .description = "Verified account"
    };
    const dcc_rest_multipart_field_t metadata_fields[] = {
        { .name = "verified", .value = "1" }
    };
    const dcc_application_role_connection_params_t connection = {
        .size = sizeof(dcc_application_role_connection_params_t),
        .platform_name = "dcc",
        .metadata_fields = metadata_fields,
        .metadata_field_count = sizeof(metadata_fields) / sizeof(metadata_fields[0])
    };
    const dcc_auto_moderation_rule_params_t automod = {
        .size = sizeof(dcc_auto_moderation_rule_params_t),
        .name = "links",
        .event_type = DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND,
        .trigger_type = DCC_AUTO_MODERATION_TRIGGER_SPAM,
        .trigger_metadata = {
            .size = sizeof(dcc_auto_moderation_metadata_params_t)
        },
        .enabled = 1
    };
    const dcc_channel_forum_tag_params_t tag = {
        .size = sizeof(dcc_channel_forum_tag_params_t),
        .name = "news",
        .emoji_name = "star"
    };
    const dcc_channel_params_t channel = {
        .size = sizeof(dcc_channel_params_t),
        .guild_id = 123,
        .type = DCC_CHANNEL_FORUM,
        .name = "general",
        .default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .available_tags = &tag,
        .available_tag_count = 1U,
        .default_forum_layout = DCC_CHANNEL_FORUM_LAYOUT_LIST_VIEW,
        .require_tag = 1
    };
    const dcc_thread_params_t thread = {
        .size = sizeof(dcc_thread_params_t),
        .channel_id = 123,
        .name = "thread",
        .auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .type = DCC_CHANNEL_PUBLIC_THREAD
    };
    const dcc_scheduled_event_params_t scheduled_event = {
        .size = sizeof(dcc_scheduled_event_params_t),
        .guild_id = 123,
        .name = "event",
        .scheduled_start_time = "2026-06-16T00:00:00.000000+00:00",
        .location = "Seoul",
        .privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,
        .status = DCC_SCHEDULED_EVENT_STATUS_SCHEDULED,
        .entity_type = DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL
    };
    const dcc_template_params_t guild_template = {
        .size = sizeof(dcc_template_params_t),
        .guild_id = 123,
        .name = "template",
        .description = "template desc"
    };
    const dcc_guild_params_t guild = {
        .size = sizeof(dcc_guild_params_t),
        .name = "guild",
        .afk_timeout = DCC_GUILD_AFK_300,
        .default_message_notifications = DCC_GUILD_NOTIFY_ONLY_MENTIONS,
        .explicit_content_filter = DCC_GUILD_EXPLICIT_CONTENT_ALL_MEMBERS,
        .mfa_level = DCC_GUILD_MFA_ELEVATED
    };
    const dcc_onboarding_params_t onboarding = {
        .size = sizeof(dcc_onboarding_params_t),
        .guild_id = 123,
        .enabled = 1,
        .mode = DCC_ONBOARDING_MODE_ADVANCED
    };
    const dcc_snowflake_t member_roles[] = {456};
    const dcc_guild_member_params_t member = {
        .size = sizeof(dcc_guild_member_params_t),
        .guild_id = 123,
        .user_id = 456,
        .flags = DCC_GUILD_MEMBER_UPDATE_NICK | DCC_GUILD_MEMBER_UPDATE_ROLES,
        .nick = "member",
        .roles = member_roles,
        .role_count = sizeof(member_roles) / sizeof(member_roles[0])
    };
    const dcc_current_guild_member_params_t current_member = {
        .size = sizeof(dcc_current_guild_member_params_t),
        .guild_id = 123,
        .nick = "self"
    };
    const dcc_current_user_params_t current_user = {
        .size = sizeof(dcc_current_user_params_t),
        .nickname = "bot"
    };
    const dcc_invite_params_t invite = {
        .size = sizeof(dcc_invite_params_t),
        .channel_id = 123,
        .target_type = DCC_INVITE_TARGET_STREAM
    };
    const dcc_webhook_params_t webhook = {
        .size = sizeof(dcc_webhook_params_t),
        .channel_id = 123,
        .name = "relay"
    };
    const dcc_channel_position_t channel_position = {
        .channel_id = 123,
        .position = 1,
        .lock_permissions = 1
    };
    const dcc_channel_positions_params_t channel_positions = {
        .size = sizeof(dcc_channel_positions_params_t),
        .guild_id = 123,
        .positions = &channel_position,
        .position_count = 1U
    };
    const dcc_dm_channel_params_t dm = {
        .size = sizeof(dcc_dm_channel_params_t),
        .user_id = 456
    };
    const dcc_voice_state_params_t voice_state = {
        .size = sizeof(dcc_voice_state_params_t),
        .guild_id = 123,
        .channel_id = 789
    };
    const dcc_message_reaction_params_t reaction = {
        .size = sizeof(dcc_message_reaction_params_t),
        .channel_id = 123,
        .message_id = 456,
        .reaction = "wave:789"
    };
    const dcc_message_flags_params_t message_flags = {
        .size = sizeof(dcc_message_flags_params_t),
        .channel_id = 123,
        .message_id = 456,
        .flags = 4
    };
    dcc_message_builder_t direct_message_builder;
    dcc_message_builder_init(&direct_message_builder);
    if (dcc_message_builder_set_content(&direct_message_builder, "direct") != DCC_OK) {
        return 1;
    }
    const dcc_poll_answer_t poll_answers[] = {
        { .media = { .text = "yes" } },
        { .media = { .text = "no" } }
    };
    const dcc_poll_builder_t poll_builder = {
        .size = sizeof(poll_builder),
        .question = { .text = "vote" },
        .answers = poll_answers,
        .answer_count = sizeof(poll_answers) / sizeof(poll_answers[0]),
        .duration_hours = 24,
        .has_duration = 1
    };
    const char sticker_data[] = "png";
    const dcc_guild_sticker_params_t sticker = {
        .size = sizeof(dcc_guild_sticker_params_t),
        .guild_id = 123,
        .name = "sticker",
        .description = "sticker desc",
        .tags = "sticker",
        .filename = "sticker.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data) - 1U
    };
    const dcc_application_command_permission_t command_permissions[] = {
        {
            .id = 789,
            .type = 1,
            .permission = 1
        }
    };
    const dcc_guild_command_permissions_params_t command_permissions_params = {
        .size = sizeof(dcc_guild_command_permissions_params_t),
        .application_id = 123,
        .guild_id = 456,
        .command_id = 789,
        .permissions = command_permissions,
        .permission_count = sizeof(command_permissions) / sizeof(command_permissions[0])
    };
    const dcc_guild_command_permissions_update_params_t command_permission_update = {
        .command_id = 789,
        .permissions = command_permissions,
        .permission_count = sizeof(command_permissions) / sizeof(command_permissions[0])
    };
    const dcc_bulk_guild_command_permissions_params_t bulk_command_permissions = {
        .size = sizeof(dcc_bulk_guild_command_permissions_params_t),
        .application_id = 123,
        .guild_id = 456,
        .commands = &command_permission_update,
        .command_count = 1U
    };
    const dcc_guild_ban_params_t ban = {
        .size = sizeof(dcc_guild_ban_params_t),
        .guild_id = 123,
        .user_id = 456,
        .delete_message_seconds = 60
    };
    const dcc_snowflake_t prune_roles[] = {789};
    const dcc_guild_prune_params_t prune = {
        .size = sizeof(dcc_guild_prune_params_t),
        .guild_id = 123,
        .days = 7,
        .include_roles = prune_roles,
        .include_role_count = sizeof(prune_roles) / sizeof(prune_roles[0]),
        .compute_prune_count = 1
    };
    const dcc_test_entitlement_params_t test_entitlement = {
        .size = sizeof(dcc_test_entitlement_params_t),
        .application_id = 123,
        .sku_id = 456,
        .owner_id = 789,
        .owner_type = 2
    };
    const dcc_guild_member_role_params_t member_role = {
        .size = sizeof(dcc_guild_member_role_params_t),
        .guild_id = 123,
        .user_id = 456,
        .role_id = 789
    };
    const dcc_thread_member_params_t thread_member = {
        .size = sizeof(dcc_thread_member_params_t),
        .thread_id = 123,
        .user_id = 456
    };
    dcc_package_metadata_update_fn metadata_update = dcc_rest_update_application_role_connection_metadata_params;
    dcc_package_connection_update_fn connection_update = dcc_rest_update_current_user_application_role_connection_params;
    dcc_package_automod_create_fn automod_create = dcc_rest_create_auto_moderation_rule_params;
    dcc_package_channel_create_fn channel_create = dcc_rest_create_guild_channel_params;
    dcc_package_thread_create_fn thread_create = dcc_rest_create_thread_params;
    dcc_package_scheduled_event_create_fn scheduled_event_create = dcc_rest_create_guild_scheduled_event_params;
    dcc_package_template_create_fn template_create = dcc_rest_create_guild_template_params;
    dcc_package_guild_create_fn guild_create = dcc_rest_create_guild_params;
    dcc_package_onboarding_modify_fn onboarding_modify = dcc_rest_modify_guild_onboarding_params;
    dcc_package_member_modify_fn member_modify = dcc_rest_modify_guild_member_params;
    dcc_package_current_member_modify_fn current_member_modify = dcc_rest_modify_current_guild_member_params;
    dcc_package_current_user_modify_fn current_user_modify = dcc_rest_modify_current_user_params;
    dcc_package_invite_create_fn invite_create = dcc_rest_create_channel_invite_params;
    dcc_package_webhook_create_fn webhook_create = dcc_rest_create_webhook_params;
    dcc_package_channel_positions_fn channel_positions_modify = dcc_rest_modify_guild_channel_positions_params;
    dcc_package_dm_create_fn dm_create = dcc_rest_create_dm_channel_params;
    dcc_package_message_reaction_fn reaction_add = dcc_rest_add_message_reaction_params;
    dcc_package_message_flags_fn message_flags_edit = dcc_rest_edit_message_flags_params;
    dcc_package_message_pin_fn legacy_pin_message = dcc_rest_legacy_pin_message;
    dcc_package_message_pin_fn legacy_unpin_message = dcc_rest_legacy_unpin_message;
    dcc_package_channel_pins_fn legacy_channel_pins = dcc_rest_get_legacy_channel_pins;
    dcc_package_direct_message_builder_fn direct_message_create = dcc_rest_create_direct_message_builder;
    dcc_package_application_command_builder_fn application_command_create =
        dcc_rest_create_application_command_builder;
    dcc_package_application_command_bulk_builder_fn application_command_bulk =
        dcc_rest_bulk_overwrite_application_commands_builder;
    dcc_package_application_command_delete_all_fn application_command_delete_all =
        dcc_rest_delete_all_application_commands;
    dcc_package_application_command_options_init_fn application_command_options_init =
        dcc_application_command_registration_options_init;
    dcc_package_application_command_options_set_global_fn application_command_options_set_global =
        dcc_application_command_registration_options_set_global;
    dcc_package_application_command_options_set_guild_fn application_command_options_set_guild =
        dcc_application_command_registration_options_set_guild;
    dcc_package_message_builder_set_poll_fn message_builder_set_poll = dcc_message_builder_set_poll;
    dcc_package_message_builder_set_components_v2_fn message_builder_set_components_v2 =
        dcc_message_builder_set_components_v2;
    dcc_package_message_builder_set_components_v2_json_fn message_builder_set_components_v2_json =
        dcc_message_builder_set_components_v2_json;
    dcc_package_component_v2_builder_init_fn component_v2_init = dcc_component_v2_builder_init;
    dcc_package_component_v2_builder_init_text_display_fn component_v2_init_text_display =
        dcc_component_v2_builder_init_text_display;
    dcc_package_component_v2_builder_init_button_fn component_v2_init_button =
        dcc_component_v2_builder_init_button;
    dcc_package_component_v2_builder_init_container_fn component_v2_init_container =
        dcc_component_v2_builder_init_container;
    dcc_package_component_v2_builder_set_content_fn component_v2_set_content =
        dcc_component_v2_builder_set_content;
    dcc_package_component_v2_builder_set_custom_id_fn component_v2_set_custom_id =
        dcc_component_v2_builder_set_custom_id;
    dcc_package_component_v2_builder_set_default_fn component_v2_set_default =
        dcc_component_v2_builder_set_default;
    dcc_package_component_v2_builder_set_default_values_fn component_v2_set_default_values =
        dcc_component_v2_builder_set_default_values;
    dcc_package_component_v2_builder_set_channel_types_fn component_v2_set_channel_types =
        dcc_component_v2_builder_set_channel_types;
    dcc_package_component_v2_builder_build_json_fn component_v2_build_json =
        dcc_component_v2_builder_build_json;
    dcc_package_component_v2_builder_build_array_json_fn component_v2_build_array_json =
        dcc_component_v2_builder_build_array_json;
    dcc_package_component_v2_builder_json_free_fn component_v2_json_free =
        dcc_component_v2_builder_json_free;
    dcc_package_message_component_at_fn message_component_at = dcc_message_component_at;
    dcc_package_message_component_root_fn message_component_root = dcc_message_component_root;
    dcc_package_message_component_by_custom_id_fn message_component_by_custom_id =
        dcc_message_component_by_custom_id;
    dcc_package_component_v2_child_fn component_v2_child = dcc_component_v2_child;
    dcc_package_component_v2_accessory_fn component_v2_accessory = dcc_component_v2_accessory;
    dcc_package_component_v2_labeled_component_fn component_v2_labeled_component =
        dcc_component_v2_labeled_component;
    dcc_package_message_poll_builder_build_json_fn message_poll_build_json =
        dcc_message_poll_builder_build_json;
    dcc_package_message_poll_builder_json_free_fn message_poll_json_free =
        dcc_message_poll_builder_json_free;
    dcc_package_webhook_message_builder_fn webhook_message_modify = dcc_rest_modify_webhook_message_builder;
    dcc_package_interaction_message_builder_fn original_response_edit =
        dcc_rest_interaction_original_response_edit_builder;
    dcc_package_interaction_message_builder_fn followup_create = dcc_rest_interaction_followup_create_builder;
    dcc_package_interaction_followup_edit_builder_fn followup_edit = dcc_rest_interaction_followup_edit_builder;
    dcc_package_interaction_response_message_builder_fn interaction_response_create =
        dcc_rest_interaction_response_create_message_builder;
    dcc_package_interaction_response_type_fn interaction_response_type =
        dcc_rest_interaction_response_create_type;
    dcc_package_interaction_response_named_fn interaction_response_pong =
        dcc_rest_interaction_response_create_pong;
    dcc_package_interaction_response_named_fn interaction_response_defer =
        dcc_rest_interaction_response_create_deferred_message;
    dcc_package_interaction_response_named_fn interaction_response_defer_update =
        dcc_rest_interaction_response_create_deferred_update;
    dcc_package_interaction_response_named_fn interaction_response_premium =
        dcc_rest_interaction_response_create_premium_required;
    dcc_package_interaction_response_named_from_interaction_fn interaction_response_pong_interaction =
        dcc_rest_interaction_response_create_pong_from_interaction;
    dcc_package_interaction_response_named_from_interaction_fn interaction_response_defer_interaction =
        dcc_rest_interaction_response_create_deferred_message_from_interaction;
    dcc_package_interaction_response_named_from_interaction_fn interaction_response_defer_update_interaction =
        dcc_rest_interaction_response_create_deferred_update_from_interaction;
    dcc_package_interaction_response_named_from_interaction_fn interaction_response_premium_interaction =
        dcc_rest_interaction_response_create_premium_required_from_interaction;
    dcc_package_sticker_create_fn sticker_create = dcc_rest_create_guild_sticker_params;
    dcc_package_command_permissions_fn command_permissions_edit =
        dcc_rest_edit_guild_command_permissions_params;
    dcc_package_bulk_command_permissions_fn bulk_command_permissions_edit =
        dcc_rest_bulk_edit_guild_command_permissions_params;
    dcc_package_guild_ban_fn ban_create = dcc_rest_create_guild_ban_params;
    dcc_package_guild_prune_fn prune_begin = dcc_rest_begin_guild_prune_params;
    dcc_package_test_entitlement_fn test_entitlement_create = dcc_rest_create_test_entitlement_params;
    dcc_package_member_role_fn member_role_add = dcc_rest_add_guild_member_role_params;
    dcc_package_thread_member_fn thread_member_add = dcc_rest_add_thread_member_params;
    dcc_package_task_group_create_fn task_group_create = dcc_task_group_create;
    dcc_package_task_group_spawn_fn task_group_spawn = dcc_task_group_spawn;
    dcc_package_task_group_wait_fn task_group_wait = dcc_task_group_wait;
    dcc_package_task_group_wait_any_fn task_group_wait_any = dcc_task_group_wait_any;
    dcc_package_task_group_wait_result_fn task_group_wait_result = dcc_task_group_wait_result;
    dcc_package_task_group_wait_any_and_cancel_fn task_group_wait_any_and_cancel =
        dcc_task_group_wait_any_and_cancel;
    dcc_package_task_group_cancel_and_wait_fn task_group_cancel_and_wait = dcc_task_group_cancel_and_wait;
    dcc_package_task_group_cancel_when_fn task_group_cancel_when = dcc_task_group_cancel_when;
    dcc_package_task_group_cancel_fn task_group_cancel = dcc_task_group_cancel;
    dcc_package_task_group_destroy_fn task_group_destroy = dcc_task_group_destroy;
    char *poll_json = NULL;
    int poll_json_ok = message_builder_set_poll != NULL &&
                       message_poll_build_json != NULL &&
                       message_poll_json_free != NULL &&
                       message_builder_set_poll(&direct_message_builder, &poll_builder) == DCC_OK &&
                       message_poll_build_json(&poll_builder, &poll_json) == DCC_OK &&
                       poll_json != NULL;
    if (message_poll_json_free != NULL) {
        message_poll_json_free(poll_json);
    }
    dcc_component_v2_builder_t package_v2_component;
    char *component_v2_json = NULL;
    char *component_v2_select_json = NULL;
    char *component_v2_array_json = NULL;
    char *message_v2_json = NULL;
    dcc_message_builder_t package_v2_message;
    int component_v2_ok = component_v2_init != NULL &&
                          component_v2_init_text_display != NULL &&
                          component_v2_init_button != NULL &&
                          component_v2_init_container != NULL &&
                          component_v2_set_content != NULL &&
                          component_v2_set_custom_id != NULL &&
                          component_v2_set_default != NULL &&
                          component_v2_set_default_values != NULL &&
                          component_v2_set_channel_types != NULL &&
                          component_v2_build_json != NULL &&
                          component_v2_build_array_json != NULL &&
                          component_v2_json_free != NULL &&
                          message_builder_set_components_v2 != NULL &&
                          message_builder_set_components_v2_json != NULL;
    if (component_v2_ok) {
        dcc_message_builder_init(&package_v2_message);
        dcc_component_v2_builder_t package_v2_select;
        dcc_component_v2_builder_t package_v2_checkbox;
        dcc_component_v2_builder_t package_v2_button;
        dcc_component_v2_builder_t package_v2_container;
        const dcc_component_v2_select_default_value_t package_v2_defaults[] = {
            {
                .id = 456,
                .type = DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL
            }
        };
        const uint32_t package_v2_channel_types[] = {0U, 5U};
        component_v2_init(&package_v2_select, DCC_COMPONENT_V2_CHANNEL_SELECT);
        component_v2_init(&package_v2_checkbox, DCC_COMPONENT_V2_CHECKBOX);
        component_v2_ok =
            component_v2_init_text_display(&package_v2_component, "package component v2") == DCC_OK &&
            component_v2_init_button(
                &package_v2_button,
                DCC_BUTTON_PRIMARY,
                "Refresh",
                "package.refresh"
            ) == DCC_OK &&
            component_v2_init_container(&package_v2_container, &package_v2_component, 1U) == DCC_OK &&
            component_v2_set_custom_id(&package_v2_select, "package.channel") == DCC_OK &&
            component_v2_set_custom_id(&package_v2_checkbox, "package.accept") == DCC_OK &&
            dcc_component_v2_builder_set_label(&package_v2_checkbox, "Accept") == DCC_OK &&
            component_v2_set_default(&package_v2_checkbox, 1U) == DCC_OK &&
            component_v2_set_default_values(
                &package_v2_select,
                package_v2_defaults,
                sizeof(package_v2_defaults) / sizeof(package_v2_defaults[0])
            ) == DCC_OK &&
            component_v2_set_channel_types(
                &package_v2_select,
                package_v2_channel_types,
                sizeof(package_v2_channel_types) / sizeof(package_v2_channel_types[0])
            ) == DCC_OK &&
            component_v2_build_json(&package_v2_component, &component_v2_json) == DCC_OK &&
            component_v2_build_json(&package_v2_select, &component_v2_select_json) == DCC_OK &&
            component_v2_build_array_json(&package_v2_component, 1U, &component_v2_array_json) ==
                DCC_OK &&
            message_builder_set_components_v2(&package_v2_message, &package_v2_component, 1U) ==
                DCC_OK &&
            dcc_message_builder_build_json(&package_v2_message, &message_v2_json) == DCC_OK &&
            package_v2_message.has_flags != 0U &&
            (package_v2_message.flags & DCC_MESSAGE_FLAG_IS_COMPONENTS_V2) != 0U;
    }
    dcc_component_v2_t package_message_components[3] = {
        {
            .type = DCC_COMPONENT_V2_SECTION,
            .child_indices = {1U},
            .child_count = 1U,
            .accessory_index = 2U,
            .has_accessory = 1U
        },
        {
            .type = DCC_COMPONENT_V2_TEXT_DISPLAY,
            .content = "body",
            .parent_index = 0U,
            .has_parent = 1U
        },
        {
            .type = DCC_COMPONENT_V2_BUTTON,
            .custom_id = "package.refresh",
            .label = "Refresh",
            .parent_index = 0U,
            .has_parent = 1U
        }
    };
    dcc_message_t package_component_message = {
        .components = package_message_components,
        .components_count = 3U,
        .component_root_indices = {0U},
        .component_root_count = 1U
    };
    const dcc_component_v2_t *package_root =
        message_component_root != NULL ? message_component_root(&package_component_message, 0U) : NULL;
    const dcc_component_v2_t *package_child =
        component_v2_child != NULL ? component_v2_child(&package_component_message, package_root, 0U) : NULL;
    const dcc_component_v2_t *package_accessory =
        component_v2_accessory != NULL ? component_v2_accessory(&package_component_message, package_root) : NULL;
    int component_v2_accessors_ok =
        message_component_at != NULL &&
        message_component_root != NULL &&
        message_component_by_custom_id != NULL &&
        component_v2_child != NULL &&
        component_v2_accessory != NULL &&
        component_v2_labeled_component != NULL &&
        message_component_at(&package_component_message, 1U) == &package_message_components[1] &&
        package_root == &package_message_components[0] &&
        package_child == &package_message_components[1] &&
        package_accessory == &package_message_components[2] &&
        message_component_by_custom_id(&package_component_message, "package.refresh") ==
            &package_message_components[2] &&
        component_v2_labeled_component(&package_component_message, package_root) == NULL;
    if (component_v2_json_free != NULL) {
        component_v2_json_free(component_v2_json);
        component_v2_json_free(component_v2_select_json);
        component_v2_json_free(component_v2_array_json);
    }
    dcc_message_builder_json_free(message_v2_json);
    dcc_application_command_registration_options_t package_command_options;
    int command_options_ok = application_command_options_init != NULL &&
                             application_command_options_set_global != NULL &&
                             application_command_options_set_guild != NULL;
    if (command_options_ok) {
        application_command_options_init(&package_command_options);
        command_options_ok = package_command_options.size == sizeof(package_command_options) &&
                             package_command_options.guild_id == 0 &&
                             application_command_options_set_global(NULL) == DCC_ERR_INVALID_ARG &&
                             application_command_options_set_guild(NULL, 123) == DCC_ERR_INVALID_ARG &&
                             application_command_options_set_guild(&package_command_options, 0) ==
                                 DCC_ERR_INVALID_ARG &&
                             application_command_options_set_guild(&package_command_options, 123) == DCC_OK &&
                             package_command_options.guild_id == 123 &&
                             application_command_options_set_global(&package_command_options) == DCC_OK &&
                             package_command_options.guild_id == 0;
    }


    return version != NULL &&
                   strcmp(version, DCC_VERSION_STRING) == 0 &&
                   metadata.type == DCC_APPLICATION_ROLE_CONNECTION_METADATA_BOOLEAN_EQUAL &&
                   connection.metadata_field_count == 1U &&
                   automod.trigger_type == DCC_AUTO_MODERATION_TRIGGER_SPAM &&
                   channel.type == DCC_CHANNEL_FORUM &&
                   tag.size == sizeof(dcc_channel_forum_tag_params_t) &&
                   thread.type == DCC_CHANNEL_PUBLIC_THREAD &&
                   scheduled_event.entity_type == DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL &&
                   guild_template.description != NULL &&
                   guild.afk_timeout == DCC_GUILD_AFK_300 &&
                   onboarding.mode == DCC_ONBOARDING_MODE_ADVANCED &&
                   member.role_count == 1U &&
                   current_member.nick != NULL &&
                   current_user.nickname != NULL &&
                   invite.target_type == DCC_INVITE_TARGET_STREAM &&
                   webhook.name != NULL &&
                   channel_positions.position_count == 1U &&
                   dm.user_id == 456U &&
                   voice_state.channel_id == 789U &&
                   reaction.message_id == 456U &&
                   message_flags.flags == 4U &&
                   direct_message_builder.has_content != 0U &&
                   direct_message_builder.poll == &poll_builder &&
                   poll_json_ok != 0 &&
                   component_v2_ok != 0 &&
                   component_v2_accessors_ok != 0 &&
                   sticker.data_len == 3U &&
                   command_permissions_params.permission_count == 1U &&
                   bulk_command_permissions.command_count == 1U &&
                   ban.delete_message_seconds == 60U &&
                   prune.include_role_count == 1U &&
                   test_entitlement.owner_type == 2U &&
                   member_role.role_id == 789U &&
                   thread_member.user_id == 456U &&
                   metadata_update != NULL &&
                   connection_update != NULL &&
                   automod_create != NULL &&
                   channel_create != NULL &&
                   thread_create != NULL &&
                   scheduled_event_create != NULL &&
                   template_create != NULL &&
                   guild_create != NULL &&
                   onboarding_modify != NULL &&
                   member_modify != NULL &&
                   current_member_modify != NULL &&
                   current_user_modify != NULL &&
                   invite_create != NULL &&
                   webhook_create != NULL &&
                   channel_positions_modify != NULL &&
                   dm_create != NULL &&
                   dcc_package_consumer_check_voice_api() != 0 &&
                   reaction_add != NULL &&
                   message_flags_edit != NULL &&
                   legacy_pin_message != NULL &&
                   legacy_unpin_message != NULL &&
                   legacy_channel_pins != NULL &&
                   direct_message_create != NULL &&
                   application_command_create != NULL &&
                   application_command_bulk != NULL &&
                   application_command_delete_all != NULL &&
                   application_command_options_init != NULL &&
                   application_command_options_set_global != NULL &&
                   application_command_options_set_guild != NULL &&
                   command_options_ok != 0 &&
                   message_builder_set_poll != NULL &&
                   message_builder_set_components_v2 != NULL &&
                   message_builder_set_components_v2_json != NULL &&
                   component_v2_init != NULL &&
                   component_v2_init_text_display != NULL &&
                   component_v2_init_button != NULL &&
                   component_v2_init_container != NULL &&
                   component_v2_set_content != NULL &&
                   component_v2_set_custom_id != NULL &&
                   component_v2_set_default_values != NULL &&
                   component_v2_set_channel_types != NULL &&
                   component_v2_build_json != NULL &&
                   component_v2_build_array_json != NULL &&
                   component_v2_json_free != NULL &&
                   message_component_at != NULL &&
                   message_component_root != NULL &&
                   message_component_by_custom_id != NULL &&
                   component_v2_child != NULL &&
                   component_v2_accessory != NULL &&
                   component_v2_labeled_component != NULL &&
                   message_poll_build_json != NULL &&
                   message_poll_json_free != NULL &&
                   webhook_message_modify != NULL &&
                   original_response_edit != NULL &&
                   followup_create != NULL &&
                   followup_edit != NULL &&
                   interaction_response_create != NULL &&
                   interaction_response_type != NULL &&
                   interaction_response_pong != NULL &&
                   interaction_response_defer != NULL &&
                   interaction_response_defer_update != NULL &&
                   interaction_response_premium != NULL &&
                   interaction_response_pong_interaction != NULL &&
                   interaction_response_defer_interaction != NULL &&
                   interaction_response_defer_update_interaction != NULL &&
                   interaction_response_premium_interaction != NULL &&
                   interaction_response_type(
                       NULL,
                       123,
                       "token",
                       (dcc_interaction_response_type_t)0,
                       NULL,
                       NULL
                   ) == DCC_ERR_INVALID_ARG &&
                   sticker_create != NULL &&
                   command_permissions_edit != NULL &&
                   bulk_command_permissions_edit != NULL &&
                   ban_create != NULL &&
                   prune_begin != NULL &&
                   test_entitlement_create != NULL &&
                   member_role_add != NULL &&
                   thread_member_add != NULL &&
                   task_group_create != NULL &&
                   task_group_spawn != NULL &&
                   task_group_wait != NULL &&
                   task_group_wait_any != NULL &&
                   task_group_wait_result != NULL &&
                   task_group_wait_any_and_cancel != NULL &&
                   task_group_cancel_and_wait != NULL &&
                   task_group_cancel_when != NULL &&
                   task_group_cancel != NULL &&
                   task_group_destroy != NULL &&
                   dcc_package_consumer_check_app_api() != 0 &&
                   dcc_package_consumer_check_app_message_api() != 0 &&
                   dcc_package_consumer_check_cluster_api() != 0 &&
                   dcc_package_consumer_check_interaction_server_api() != 0 &&
                   dcc_package_consumer_check_hot_reload_api() != 0 &&
                   dcc_package_consumer_check_official_surface_api() != 0 &&
                   dcc_package_consumer_check_interaction_api() != 0 &&
                   dcc_package_consumer_check_wait_api() != 0
               ? 0
               : 1;
}
