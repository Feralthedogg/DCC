#ifndef DCC_APPLICATION_COMMAND_H
#define DCC_APPLICATION_COMMAND_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_application_command_type {
    DCC_APPLICATION_COMMAND_CHAT_INPUT = 1,
    DCC_APPLICATION_COMMAND_USER = 2,
    DCC_APPLICATION_COMMAND_MESSAGE = 3,
    DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT = 4
} dcc_application_command_type_t;

typedef struct dcc_application_command_builder {
    const char *name;
    const char *name_localizations_json;
    const char *description;
    const char *description_localizations_json;
    const char *options_json;
    const char *integration_types_json;
    const char *contexts_json;
    uint32_t type;
    uint64_t default_member_permissions;
    uint8_t dm_permission;
    uint8_t nsfw;
    uint8_t has_name;
    uint8_t has_description;
    uint8_t has_type;
    uint8_t has_default_member_permissions;
    uint8_t default_member_permissions_null;
    uint8_t has_dm_permission;
    uint8_t has_nsfw;
} dcc_application_command_builder_t;

DCC_API void dcc_application_command_builder_init(dcc_application_command_builder_t *builder);
DCC_API dcc_status_t dcc_application_command_builder_set_name(
    dcc_application_command_builder_t *builder,
    const char *name
);
DCC_API dcc_status_t dcc_application_command_builder_set_name_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *name_localizations_json
);
DCC_API dcc_status_t dcc_application_command_builder_set_description(
    dcc_application_command_builder_t *builder,
    const char *description
);
DCC_API dcc_status_t dcc_application_command_builder_set_description_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *description_localizations_json
);
DCC_API dcc_status_t dcc_application_command_builder_set_type(
    dcc_application_command_builder_t *builder,
    dcc_application_command_type_t type
);
DCC_API dcc_status_t dcc_application_command_builder_set_options_json(
    dcc_application_command_builder_t *builder,
    const char *options_json
);
DCC_API dcc_status_t dcc_application_command_builder_set_default_member_permissions(
    dcc_application_command_builder_t *builder,
    uint64_t permissions
);
DCC_API dcc_status_t dcc_application_command_builder_set_default_member_permissions_null(
    dcc_application_command_builder_t *builder
);
DCC_API dcc_status_t dcc_application_command_builder_set_dm_permission(
    dcc_application_command_builder_t *builder,
    uint8_t dm_permission
);
DCC_API dcc_status_t dcc_application_command_builder_set_nsfw(
    dcc_application_command_builder_t *builder,
    uint8_t nsfw
);
DCC_API dcc_status_t dcc_application_command_builder_set_integration_types_json(
    dcc_application_command_builder_t *builder,
    const char *integration_types_json
);
DCC_API dcc_status_t dcc_application_command_builder_set_contexts_json(
    dcc_application_command_builder_t *builder,
    const char *contexts_json
);
DCC_API dcc_status_t dcc_application_command_builder_build_json(
    const dcc_application_command_builder_t *builder,
    char **out_json
);
DCC_API dcc_status_t dcc_application_command_builder_build_array_json(
    const dcc_application_command_builder_t *builders,
    size_t builder_count,
    char **out_json
);
DCC_API void dcc_application_command_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
