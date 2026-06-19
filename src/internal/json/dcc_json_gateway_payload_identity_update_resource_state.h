#ifndef DCC_JSON_GATEWAY_PAYLOAD_IDENTITY_UPDATE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_IDENTITY_UPDATE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_IDENTITY_UPDATE_RESOURCE_FIELDS \
    int has_integration; \
    dcc_integration_t integration; \
    int has_presence; \
    dcc_presence_t presence; \
    int has_user; \
    dcc_user_t user; \
    int has_application_command_permissions_update; \
    dcc_application_command_permissions_update_t application_command_permissions_update; \
    int has_guild_audit_log_entry; \
    dcc_guild_audit_log_entry_t guild_audit_log_entry; \
    int has_guild_ban; \
    dcc_guild_ban_t guild_ban

#endif
