#ifndef DCC_REST_TYPES_ROLE_CONNECTIONS_H
#define DCC_REST_TYPES_ROLE_CONNECTIONS_H

#include <dcc/rest/base.h>

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
    dcc_application_role_connection_metadata_type_t type;
    const char *key;
    const char *name;
    const char *name_localizations_json;
    const char *description;
    const char *description_localizations_json;
} dcc_application_role_connection_metadata_params_t;

typedef struct dcc_application_role_connection_params {
    size_t size;
    const char *platform_name;
    const char *platform_username;
    const dcc_rest_multipart_field_t *metadata_fields;
    size_t metadata_field_count;
} dcc_application_role_connection_params_t;

#ifdef __cplusplus
}
#endif

#endif
