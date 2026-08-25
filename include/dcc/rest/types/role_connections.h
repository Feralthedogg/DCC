#ifndef DCC_REST_TYPES_ROLE_CONNECTIONS_H
#define DCC_REST_TYPES_ROLE_CONNECTIONS_H

#include <dcc/application_types.h>
#include <dcc/rest/base.h>
#include <dcc/rest/types/string_map.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_application_role_connection_metadata_type {
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_LESS_THAN_OR_EQUAL = 1,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_GREATER_THAN_OR_EQUAL = 2,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_EQUAL = 3,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_NOT_EQUAL = 4,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_DATETIME_LESS_THAN_OR_EQUAL = 5,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_DATETIME_GREATER_THAN_OR_EQUAL = 6,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_BOOLEAN_EQUAL = 7,
  DCC_APPLICATION_ROLE_CONNECTION_METADATA_BOOLEAN_NOT_EQUAL = 8
} dcc_application_role_connection_metadata_type_t;

typedef struct dcc_application_role_connection_metadata_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_application_role_connection_metadata_type_t type;
  const char *key;
  const char *name;
  const dcc_localization_t *name_localizations;
  size_t name_localization_count;
  const char *description;
  const dcc_localization_t *description_localizations;
  size_t description_localization_count;
} dcc_application_role_connection_metadata_params_t;

typedef struct dcc_application_role_connection_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *platform_name;
  const char *platform_username;
  const dcc_rest_string_map_t *metadata;
} dcc_application_role_connection_params_t;

#define DCC_APPLICATION_ROLE_CONNECTION_METADATA_PARAMS_VERSION 1U
#define DCC_APPLICATION_ROLE_CONNECTION_PARAMS_VERSION 1U
enum {
  DCC_ROLE_CONNECTION_METADATA_PRESENT_NAME_LOCALIZATIONS = UINT64_C(1),
  DCC_ROLE_CONNECTION_METADATA_PRESENT_DESCRIPTION_LOCALIZATIONS = UINT64_C(1)
                                                                   << 1U
};
enum {
  DCC_ROLE_CONNECTION_PRESENT_PLATFORM_NAME = UINT64_C(1),
  DCC_ROLE_CONNECTION_PRESENT_PLATFORM_USERNAME = UINT64_C(1) << 1U,
  DCC_ROLE_CONNECTION_PRESENT_METADATA = UINT64_C(1) << 2U
};
#define DCC_APPLICATION_ROLE_CONNECTION_METADATA_PARAMS_INIT                   \
  {sizeof(dcc_application_role_connection_metadata_params_t),                  \
   DCC_APPLICATION_ROLE_CONNECTION_METADATA_PARAMS_VERSION,                    \
   UINT64_C(0),                                                                \
   0,                                                                          \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   NULL,                                                                       \
   NULL,                                                                       \
   0U}
#define DCC_APPLICATION_ROLE_CONNECTION_PARAMS_INIT                            \
  {sizeof(dcc_application_role_connection_params_t),                           \
   DCC_APPLICATION_ROLE_CONNECTION_PARAMS_VERSION,                             \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL}

#ifdef __cplusplus
}
#endif

#endif
