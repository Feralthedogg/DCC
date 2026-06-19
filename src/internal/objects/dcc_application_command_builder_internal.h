#ifndef DCC_APPLICATION_COMMAND_BUILDER_INTERNAL_H
#define DCC_APPLICATION_COMMAND_BUILDER_INTERNAL_H

#include <dcc/application_command.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_application_command_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_application_command_json_buffer_t;

void dcc_command_json_buffer_deinit(dcc_application_command_json_buffer_t *buffer);
dcc_status_t dcc_command_json_append(
    dcc_application_command_json_buffer_t *buffer,
    const void *data,
    size_t len
);
dcc_status_t dcc_command_json_append_cstr(
    dcc_application_command_json_buffer_t *buffer,
    const char *value
);
dcc_status_t dcc_command_json_append_u32(
    dcc_application_command_json_buffer_t *buffer,
    uint32_t value
);
dcc_status_t dcc_command_json_append_u64_string(
    dcc_application_command_json_buffer_t *buffer,
    uint64_t value
);
dcc_status_t dcc_command_json_append_escaped_string(
    dcc_application_command_json_buffer_t *buffer,
    const char *value
);
dcc_status_t dcc_command_json_member_prefix(
    dcc_application_command_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_command_json_append_string_member(
    dcc_application_command_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_command_json_append_raw_member(
    dcc_application_command_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_application_command_builder_append_identity_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_application_command_builder_append_payload_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_application_command_builder_append_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif
