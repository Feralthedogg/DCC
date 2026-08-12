#ifndef DCC_COMMAND_REGISTRY_H
#define DCC_COMMAND_REGISTRY_H

#include <dcc/application_command.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/rest/base.h>
#include <dcc/rest/request.h>
#include <dcc/snowflake.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_command_registry_diff_action {
    DCC_COMMAND_REGISTRY_NOOP = 0,
    DCC_COMMAND_REGISTRY_CREATE,
    DCC_COMMAND_REGISTRY_UPDATE,
    DCC_COMMAND_REGISTRY_DELETE_STALE
} dcc_command_registry_diff_action_t;

typedef struct dcc_command_registry_options {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t guild_id;
    uint8_t delete_stale;
    uint8_t dry_run;
} dcc_command_registry_options_t;

enum {
    DCC_COMMAND_REGISTRY_OPTIONS_VERSION = 1U,
    DCC_COMMAND_REGISTRY_OPERATION_OPTIONS_VERSION = 1U,
    DCC_COMMAND_REGISTRY_OPERATION_RESULT_VERSION = 1U,
    DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_GUILD_ID = UINT64_C(1),
    DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DELETE_STALE = UINT64_C(1) << 1U,
    DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DRY_RUN = UINT64_C(1) << 2U
};

#define DCC_COMMAND_REGISTRY_OPTIONS_INIT                                      \
    { sizeof(dcc_command_registry_options_t), DCC_COMMAND_REGISTRY_OPTIONS_VERSION, \
      UINT64_C(0), UINT64_C(0), 0U, 0U }

typedef struct dcc_command_registry {
    size_t size;
    void *state;
} dcc_command_registry_t;

typedef struct dcc_command_registry_entry {
    size_t size;
    dcc_application_command_builder_t command;
} dcc_command_registry_entry_t;

typedef struct dcc_command_registry_remote_entry {
    size_t size;
    dcc_snowflake_t id;
    const char *name;
    uint32_t type;
    const char *canonical_json;
} dcc_command_registry_remote_entry_t;

typedef struct dcc_command_registry_remote_snapshot {
    size_t size;
    void *state;
    const dcc_command_registry_remote_entry_t *entries;
    size_t entry_count;
} dcc_command_registry_remote_snapshot_t;

typedef struct dcc_command_registry_diff {
    dcc_command_registry_diff_action_t action;
    const char *name;
    uint32_t type;
    dcc_snowflake_t remote_id;
    size_t local_index;
    size_t remote_index;
    const char *local_json;
    const char *remote_json;
} dcc_command_registry_diff_t;

typedef struct dcc_command_registry_plan {
    size_t size;
    void *state;
    const dcc_command_registry_diff_t *diffs;
    size_t diff_count;
    size_t create_count;
    size_t update_count;
    size_t delete_stale_count;
    size_t noop_count;
    dcc_snowflake_t guild_id;
    uint8_t delete_stale;
    uint8_t dry_run;
} dcc_command_registry_plan_t;

typedef struct dcc_command_registry_operation dcc_command_registry_operation_t;

typedef struct dcc_command_registry_operation_result {
    size_t size;
    uint32_t version;
    dcc_status_t status;
    size_t planned_count;
    size_t submitted_count;
    size_t succeeded_count;
    size_t create_completed;
    size_t update_completed;
    size_t delete_stale_completed;
    size_t noop_count;
    size_t failed_plan_index;
    dcc_command_registry_diff_action_t failed_action;
    const dcc_rest_result_t *failed_rest_result;
} dcc_command_registry_operation_result_t;

typedef void (*dcc_command_registry_operation_result_fn)(
    dcc_client_t *client,
    const dcc_command_registry_operation_result_t *result,
    void *user_data
);

typedef struct dcc_command_registry_operation_options {
    size_t size;
    uint32_t version;
    dcc_rest_priority_t priority;
    dcc_rest_auth_mode_t auth_mode;
    const char *auth_token;
    dcc_command_registry_operation_result_fn callback;
    void *user_data;
} dcc_command_registry_operation_options_t;

#define DCC_COMMAND_REGISTRY_OPERATION_OPTIONS_INIT                            \
    { sizeof(dcc_command_registry_operation_options_t),                        \
      DCC_COMMAND_REGISTRY_OPERATION_OPTIONS_VERSION,                          \
      DCC_REST_PRIORITY_NORMAL, DCC_REST_AUTH_DEFAULT, NULL, NULL, NULL }

#define DCC_COMMAND_REGISTRY_OPERATION_RESULT_INIT                             \
    { sizeof(dcc_command_registry_operation_result_t),                         \
      DCC_COMMAND_REGISTRY_OPERATION_RESULT_VERSION, DCC_OK, 0U, 0U, 0U, 0U, \
      0U, 0U, 0U, SIZE_MAX, DCC_COMMAND_REGISTRY_NOOP, NULL }

DCC_API void dcc_command_registry_options_init(dcc_command_registry_options_t *options);
DCC_API dcc_status_t dcc_command_registry_options_set_global(dcc_command_registry_options_t *options);
DCC_API dcc_status_t dcc_command_registry_options_set_guild(
    dcc_command_registry_options_t *options,
    dcc_snowflake_t guild_id
);
DCC_API dcc_status_t dcc_command_registry_options_set_delete_stale(
    dcc_command_registry_options_t *options,
    uint8_t delete_stale
);
DCC_API dcc_status_t dcc_command_registry_options_set_dry_run(
    dcc_command_registry_options_t *options,
    uint8_t dry_run
);

DCC_API void dcc_command_registry_init(dcc_command_registry_t *registry);
DCC_API void dcc_command_registry_deinit(dcc_command_registry_t *registry);
DCC_API dcc_status_t dcc_command_registry_clear(dcc_command_registry_t *registry);
DCC_API dcc_status_t dcc_command_registry_add_builder(
    dcc_command_registry_t *registry,
    const dcc_application_command_builder_t *command
);
DCC_API size_t dcc_command_registry_count(const dcc_command_registry_t *registry);

DCC_API dcc_status_t dcc_command_registry_build_plan(
    const dcc_command_registry_t *registry,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    dcc_command_registry_plan_t *out
);
DCC_API dcc_status_t dcc_command_registry_remote_snapshot_parse_json(
    const char *json,
    size_t json_len,
    dcc_command_registry_remote_snapshot_t *out
);
DCC_API void dcc_command_registry_remote_snapshot_deinit(
    dcc_command_registry_remote_snapshot_t *snapshot
);
DCC_API void dcc_command_registry_plan_deinit(dcc_command_registry_plan_t *plan);
DCC_API const char *dcc_command_registry_action_string(dcc_command_registry_diff_action_t action);
DCC_API dcc_status_t dcc_command_registry_plan_format(
    const dcc_command_registry_plan_t *plan,
    char *buf,
    size_t buf_len,
    size_t *out_len
);

DCC_API dcc_status_t dcc_command_registry_fetch_remote(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *registry_options,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);
DCC_API dcc_status_t dcc_command_registry_apply(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_plan_t *plan,
    const dcc_command_registry_operation_options_t *operation_options,
    dcc_command_registry_operation_t **out_operation
);
DCC_API dcc_status_t dcc_command_registry_operation_wait(
    dcc_command_registry_operation_t *operation,
    uint32_t timeout_ms,
    const dcc_command_registry_operation_result_t **out_result
);
DCC_API dcc_status_t dcc_command_registry_operation_cancel(
    dcc_command_registry_operation_t *operation
);
DCC_API uint8_t dcc_command_registry_operation_completed(
    const dcc_command_registry_operation_t *operation
);
DCC_API void dcc_command_registry_operation_destroy(
    dcc_command_registry_operation_t *operation
);

#ifdef __cplusplus
}
#endif

#endif
