#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_REST_RESOURCES_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_REST_RESOURCES_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_metadata_update_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_application_role_connection_metadata_params_t *,
        size_t,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_connection_update_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_application_role_connection_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_automod_create_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        const dcc_auto_moderation_rule_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_channel_create_fn)(
        dcc_client_t *,
        const dcc_channel_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_thread_create_fn)(
        dcc_client_t *,
        const dcc_thread_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_scheduled_event_create_fn)(
        dcc_client_t *,
        const dcc_scheduled_event_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_template_create_fn)(
        dcc_client_t *,
        const dcc_template_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_guild_create_fn)(
        dcc_client_t *,
        const dcc_guild_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_onboarding_modify_fn)(
        dcc_client_t *,
        const dcc_onboarding_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_member_modify_fn)(
        dcc_client_t *,
        const dcc_guild_member_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_current_member_modify_fn)(
        dcc_client_t *,
        const dcc_current_guild_member_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_current_user_modify_fn)(
        dcc_client_t *,
        const dcc_current_user_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_invite_create_fn)(
        dcc_client_t *,
        const dcc_invite_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_webhook_create_fn)(
        dcc_client_t *,
        const dcc_webhook_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_channel_positions_fn)(
        dcc_client_t *,
        const dcc_channel_positions_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_dm_create_fn)(
        dcc_client_t *,
        const dcc_dm_channel_params_t *,
        dcc_rest_cb,
        void *
    );
typedef dcc_status_t (*dcc_package_voice_state_fn)(
        dcc_client_t *,
        const dcc_voice_state_params_t *,
        dcc_rest_cb,
        void *
    );

#endif
