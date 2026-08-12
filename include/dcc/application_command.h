#ifndef DCC_APPLICATION_COMMAND_H
#define DCC_APPLICATION_COMMAND_H

#include <dcc/autocomplete.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION = 1U,
  DCC_APPLICATION_COMMAND_BUILDER_VERSION = 1U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME = UINT64_C(1),
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION = UINT64_C(1) << 1U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON = UINT64_C(1) << 2U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON = UINT64_C(1) << 3U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON = UINT64_C(1) << 4U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES = UINT64_C(1) << 5U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS_JSON = UINT64_C(1) << 6U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS = UINT64_C(1) << 7U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES = UINT64_C(1) << 8U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE = UINT64_C(1) << 9U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE = UINT64_C(1) << 10U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE = UINT64_C(1) << 11U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE = UINT64_C(1) << 12U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE = UINT64_C(1) << 13U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED = UINT64_C(1) << 14U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE = UINT64_C(1) << 15U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS = UINT64_C(1) << 16U,
  DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS = UINT64_C(1) << 17U,

  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME = UINT64_C(1),
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON = UINT64_C(1) << 1U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION = UINT64_C(1) << 2U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON = UINT64_C(1) << 3U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON = UINT64_C(1) << 4U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS = UINT64_C(1) << 5U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON = UINT64_C(1) << 6U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON = UINT64_C(1) << 7U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE = UINT64_C(1) << 8U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS = UINT64_C(1) << 9U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION = UINT64_C(1) << 10U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW = UINT64_C(1) << 11U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS = UINT64_C(1) << 12U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS = UINT64_C(1) << 13U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES = UINT64_C(1) << 14U,
  DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS = UINT64_C(1) << 15U
};

typedef enum dcc_application_command_type {
    DCC_APPLICATION_COMMAND_CHAT_INPUT = 1,
    DCC_APPLICATION_COMMAND_USER = 2,
    DCC_APPLICATION_COMMAND_MESSAGE = 3,
    DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT = 4
} dcc_application_command_type_t;

typedef enum dcc_application_command_option_type {
    DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND = 1,
    DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP = 2,
    DCC_APPLICATION_COMMAND_OPTION_STRING = 3,
    DCC_APPLICATION_COMMAND_OPTION_INTEGER = 4,
    DCC_APPLICATION_COMMAND_OPTION_BOOLEAN = 5,
    DCC_APPLICATION_COMMAND_OPTION_USER = 6,
    DCC_APPLICATION_COMMAND_OPTION_CHANNEL = 7,
    DCC_APPLICATION_COMMAND_OPTION_ROLE = 8,
    DCC_APPLICATION_COMMAND_OPTION_MENTIONABLE = 9,
    DCC_APPLICATION_COMMAND_OPTION_NUMBER = 10,
    DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT = 11
} dcc_application_command_option_type_t;

typedef struct dcc_application_command_option_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    const char *description;
    uint32_t type;
    const char *name_localizations_json;
    const char *description_localizations_json;
    const char *choices_json;
    const dcc_autocomplete_choice_t *choices;
    size_t choices_count;
    const char *options_json;
    const struct dcc_application_command_option_builder *options;
    size_t options_count;
    const uint32_t *channel_types;
    size_t channel_types_count;
    int64_t min_integer_value;
    int64_t max_integer_value;
    double min_number_value;
    double max_number_value;
    uint8_t required;
    uint8_t autocomplete;
    const dcc_localization_t *name_localizations;
    size_t name_localization_count;
    const dcc_localization_t *description_localizations;
    size_t description_localization_count;
} dcc_application_command_option_builder_t;

typedef struct dcc_application_command_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    const char *name_localizations_json;
    const char *description;
    const char *description_localizations_json;
    const char *options_json;
    const dcc_application_command_option_builder_t *options;
    size_t options_count;
    const char *integration_types_json;
    const char *contexts_json;
    uint32_t type;
    uint64_t default_member_permissions;
    uint8_t dm_permission;
    uint8_t nsfw;
    uint8_t default_member_permissions_null;
    const dcc_localization_t *name_localizations;
    size_t name_localization_count;
    const dcc_localization_t *description_localizations;
    size_t description_localization_count;
    const dcc_application_integration_type_t *integration_types;
    size_t integration_type_count;
    const dcc_interaction_context_type_t *contexts;
    size_t context_count;
} dcc_application_command_builder_t;

#define DCC_APPLICATION_COMMAND_OPTION_BUILDER_INIT \
    { \
        sizeof(dcc_application_command_option_builder_t), \
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION, UINT64_C(0), \
        NULL, NULL, 0U, NULL, NULL, NULL, NULL, 0U, NULL, NULL, 0U, \
        NULL, 0U, INT64_C(0), INT64_C(0), 0.0, 0.0, 0U, 0U, \
        NULL, 0U, NULL, 0U \
    }
#define DCC_APPLICATION_COMMAND_BUILDER_INIT \
    { \
        sizeof(dcc_application_command_builder_t), \
        DCC_APPLICATION_COMMAND_BUILDER_VERSION, UINT64_C(0), \
        NULL, NULL, NULL, NULL, NULL, NULL, 0U, NULL, NULL, 0U, \
        UINT64_C(0), 0U, 0U, 0U, NULL, 0U, NULL, 0U, NULL, 0U, NULL, 0U \
    }

DCC_API void dcc_application_command_option_builder_init(
    dcc_application_command_option_builder_t *builder
);
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
DCC_API dcc_status_t dcc_application_command_builder_set_options(
    dcc_application_command_builder_t *builder,
    const dcc_application_command_option_builder_t *options,
    size_t options_count
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
DCC_API dcc_status_t dcc_application_command_option_builder_set_name_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
);
DCC_API dcc_status_t dcc_application_command_option_builder_set_description_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
);
DCC_API dcc_status_t dcc_application_command_builder_set_name_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
);
DCC_API dcc_status_t dcc_application_command_builder_set_description_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
);
DCC_API dcc_status_t dcc_application_command_builder_set_integration_types(
    dcc_application_command_builder_t *builder,
    const dcc_application_integration_type_t *integration_types,
    size_t integration_type_count
);
DCC_API dcc_status_t dcc_application_command_builder_set_contexts(
    dcc_application_command_builder_t *builder,
    const dcc_interaction_context_type_t *contexts,
    size_t context_count
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
DCC_API dcc_status_t dcc_application_command_option_builder_build_array_json(
    const dcc_application_command_option_builder_t *options,
    size_t option_count,
    char **out_json
);
DCC_API void dcc_application_command_builder_json_free(char *json);
DCC_API void dcc_application_command_option_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
