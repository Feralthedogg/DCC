#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_REST_MESSAGES_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_REST_MESSAGES_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_message_reaction_fn)(
        dcc_client_t *,
        const dcc_message_reaction_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_message_flags_fn)(
        dcc_client_t *,
        const dcc_message_flags_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_direct_message_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_message_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_application_command_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_application_command_registration_options_t *,
        const dcc_application_command_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_application_command_bulk_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_application_command_registration_options_t *,
        const dcc_application_command_builder_t *,
        size_t,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_application_command_delete_all_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_application_command_registration_options_t *,
        dcc_rest_cb,
        void *
    );
typedef void (*dcc_package_application_command_options_init_fn)(
        dcc_application_command_registration_options_t *
    );
typedef dcc_status_t (*dcc_package_application_command_options_set_global_fn)(
        dcc_application_command_registration_options_t *
    );
typedef dcc_status_t (*dcc_package_application_command_options_set_guild_fn)(
        dcc_application_command_registration_options_t *,
        dcc_snowflake_t
    );
typedef dcc_status_t (*dcc_package_message_builder_set_poll_fn)(
        dcc_message_builder_t *,
        const dcc_poll_builder_t *
    );
typedef dcc_status_t (*dcc_package_message_builder_set_components_v2_fn)(
        dcc_message_builder_t *,
        const dcc_component_v2_builder_t *,
        size_t
    );
typedef dcc_status_t (*dcc_package_message_builder_set_components_v2_json_fn)(
        dcc_message_builder_t *,
        const char *
    );
typedef void (*dcc_package_component_v2_builder_init_fn)(
        dcc_component_v2_builder_t *,
        dcc_component_v2_type_t
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_init_text_display_fn)(
        dcc_component_v2_builder_t *,
        const char *
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_init_button_fn)(
        dcc_component_v2_builder_t *,
        dcc_button_style_t,
        const char *,
        const char *
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_init_container_fn)(
        dcc_component_v2_builder_t *,
        const dcc_component_v2_builder_t *,
        size_t
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_set_content_fn)(
        dcc_component_v2_builder_t *,
        const char *
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_set_custom_id_fn)(
        dcc_component_v2_builder_t *,
        const char *
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_set_default_fn)(
        dcc_component_v2_builder_t *,
        uint8_t
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_set_default_values_fn)(
        dcc_component_v2_builder_t *,
        const dcc_component_v2_select_default_value_t *,
        size_t
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_set_channel_types_fn)(
        dcc_component_v2_builder_t *,
        const uint32_t *,
        size_t
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_build_json_fn)(
        const dcc_component_v2_builder_t *,
        char **
    );
typedef dcc_status_t (*dcc_package_component_v2_builder_build_array_json_fn)(
        const dcc_component_v2_builder_t *,
        size_t,
        char **
    );
typedef void (*dcc_package_component_v2_builder_json_free_fn)(char *);
typedef const dcc_component_v2_t *(*dcc_package_message_component_at_fn)(
        const dcc_message_t *,
        size_t
    );
typedef const dcc_component_v2_t *(*dcc_package_message_component_root_fn)(
        const dcc_message_t *,
        size_t
    );
typedef const dcc_component_v2_t *(*dcc_package_message_component_by_custom_id_fn)(
        const dcc_message_t *,
        const char *
    );
typedef const dcc_component_v2_t *(*dcc_package_component_v2_child_fn)(
        const dcc_message_t *,
        const dcc_component_v2_t *,
        size_t
    );
typedef const dcc_component_v2_t *(*dcc_package_component_v2_accessory_fn)(
        const dcc_message_t *,
        const dcc_component_v2_t *
    );
typedef const dcc_component_v2_t *(*dcc_package_component_v2_labeled_component_fn)(
        const dcc_message_t *,
        const dcc_component_v2_t *
    );
typedef dcc_status_t (*dcc_package_message_poll_builder_build_json_fn)(
        const dcc_poll_builder_t *,
        char **
    );
typedef void (*dcc_package_message_poll_builder_json_free_fn)(char *);
typedef dcc_status_t (*dcc_package_webhook_message_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        dcc_snowflake_t,
        const char *,
        const dcc_message_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_message_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        const dcc_message_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_followup_edit_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        dcc_snowflake_t,
        const dcc_message_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_response_message_builder_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        dcc_interaction_response_type_t,
        const dcc_message_builder_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_response_type_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        dcc_interaction_response_type_t,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_response_named_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const char *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_interaction_response_named_from_interaction_fn)(
        dcc_client_t *,
        const dcc_interaction_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_sticker_create_fn)(
        dcc_client_t *,
        const dcc_guild_sticker_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_command_permissions_fn)(
        dcc_client_t *,
        const dcc_guild_command_permissions_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_bulk_command_permissions_fn)(
        dcc_client_t *,
        const dcc_bulk_guild_command_permissions_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_guild_ban_fn)(
        dcc_client_t *,
        const dcc_guild_ban_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_guild_prune_fn)(
        dcc_client_t *,
        const dcc_guild_prune_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_test_entitlement_fn)(
        dcc_client_t *,
        const dcc_test_entitlement_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_member_role_fn)(
        dcc_client_t *,
        const dcc_guild_member_role_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_thread_member_fn)(
        dcc_client_t *,
        const dcc_thread_member_params_t *,
        dcc_rest_cb,
        void *
    );

#endif
