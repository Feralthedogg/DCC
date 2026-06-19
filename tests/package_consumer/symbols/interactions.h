#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_INTERACTIONS_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_INTERACTIONS_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_on_interaction_name_fn)(
        dcc_client_t *,
        dcc_event_type_t,
        const char *,
        dcc_event_cb,
        void *,
        dcc_listener_id_t *
    );
typedef dcc_status_t (*dcc_package_on_interaction_short_fn)(
        dcc_client_t *,
        const char *,
        dcc_event_cb,
        void *,
        dcc_listener_id_t *
    );
typedef dcc_status_t (*dcc_package_off_interaction_fn)(dcc_client_t *, dcc_event_type_t, dcc_listener_id_t);
typedef const dcc_interaction_option_t *(*dcc_package_interaction_option_by_name_fn)(
        const dcc_interaction_t *,
        const char *
    );
typedef const dcc_interaction_option_t *(*dcc_package_interaction_focused_option_fn)(
        const dcc_interaction_t *
    );
typedef const dcc_interaction_option_t *(*dcc_package_interaction_option_child_by_name_fn)(
        const dcc_interaction_option_t *,
        const char *
    );
typedef const dcc_interaction_option_t *(*dcc_package_interaction_subcommand_fn)(
        const dcc_interaction_t *
    );
typedef const char *(*dcc_package_interaction_option_string_fn)(
        const dcc_interaction_t *,
        const char *
    );
typedef int (*dcc_package_interaction_option_integer_fn)(
        const dcc_interaction_t *,
        const char *,
        int64_t *
    );
typedef int (*dcc_package_interaction_option_number_fn)(
        const dcc_interaction_t *,
        const char *,
        double *
    );
typedef int (*dcc_package_interaction_option_boolean_fn)(
        const dcc_interaction_t *,
        const char *,
        uint8_t *
    );
typedef int (*dcc_package_interaction_option_snowflake_fn)(
        const dcc_interaction_t *,
        const char *,
        dcc_snowflake_t *
    );
typedef const dcc_user_t *(*dcc_package_interaction_resolved_user_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_member_t *(*dcc_package_interaction_resolved_member_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_role_t *(*dcc_package_interaction_resolved_role_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_channel_t *(*dcc_package_interaction_resolved_channel_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_message_t *(*dcc_package_interaction_resolved_message_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_attachment_t *(*dcc_package_interaction_resolved_attachment_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef const dcc_interaction_resolved_permission_t *(
        *dcc_package_interaction_resolved_permission_fn
    )(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef dcc_snowflake_t (*dcc_package_interaction_authorizing_integration_owner_fn)(
        const dcc_interaction_t *,
        uint32_t
    );
typedef const dcc_entitlement_t *(*dcc_package_interaction_entitlement_fn)(
        const dcc_interaction_t *,
        dcc_snowflake_t
    );
typedef dcc_attachment_t *(*dcc_package_attachment_clone_fn)(const dcc_attachment_t *);
typedef void (*dcc_package_attachment_free_fn)(dcc_attachment_t *);

#endif
