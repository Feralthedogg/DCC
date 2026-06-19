#ifndef DCC_OBJECTS_CLONE_INTERACTIONS_H
#define DCC_OBJECTS_CLONE_INTERACTIONS_H

#include <dcc/objects/interactions.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_interaction_t *dcc_interaction_clone(const dcc_interaction_t *interaction);
DCC_API void dcc_interaction_free(dcc_interaction_t *interaction);
DCC_API const dcc_interaction_option_t *dcc_interaction_option_by_name(
    const dcc_interaction_t *interaction,
    const char *name
);
DCC_API const dcc_interaction_option_t *dcc_interaction_focused_option(
    const dcc_interaction_t *interaction
);
DCC_API const dcc_interaction_option_t *dcc_interaction_option_child_by_name(
    const dcc_interaction_option_t *option,
    const char *name
);
DCC_API const dcc_interaction_option_t *dcc_interaction_subcommand(
    const dcc_interaction_t *interaction
);
DCC_API const dcc_interaction_option_t *dcc_interaction_subcommand_group(
    const dcc_interaction_t *interaction
);
DCC_API const char *dcc_interaction_option_string(const dcc_interaction_t *interaction, const char *name);
DCC_API int dcc_interaction_option_integer(
    const dcc_interaction_t *interaction,
    const char *name,
    int64_t *out
);
DCC_API int dcc_interaction_option_number(
    const dcc_interaction_t *interaction,
    const char *name,
    double *out
);
DCC_API int dcc_interaction_option_boolean(
    const dcc_interaction_t *interaction,
    const char *name,
    uint8_t *out
);
DCC_API int dcc_interaction_option_snowflake(
    const dcc_interaction_t *interaction,
    const char *name,
    dcc_snowflake_t *out
);
DCC_API const dcc_user_t *dcc_interaction_resolved_user(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);
DCC_API const dcc_member_t *dcc_interaction_resolved_member(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t user_id
);
DCC_API const dcc_role_t *dcc_interaction_resolved_role(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);
DCC_API const dcc_channel_t *dcc_interaction_resolved_channel(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);
DCC_API const dcc_message_t *dcc_interaction_resolved_message(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);
DCC_API const dcc_attachment_t *dcc_interaction_resolved_attachment(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);
DCC_API const dcc_interaction_resolved_permission_t *dcc_interaction_resolved_permission(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t user_id
);
DCC_API dcc_snowflake_t dcc_interaction_authorizing_integration_owner(
    const dcc_interaction_t *interaction,
    uint32_t type
);
DCC_API const dcc_entitlement_t *dcc_interaction_entitlement(
    const dcc_interaction_t *interaction,
    dcc_snowflake_t id
);

#ifdef __cplusplus
}
#endif

#endif
