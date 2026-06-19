#ifndef DCC_COMMAND_REGISTRY_H
#define DCC_COMMAND_REGISTRY_H

#include <dcc/application_command.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/rest/base.h>
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
    dcc_snowflake_t guild_id;
    uint8_t delete_stale;
    uint8_t dry_run;
} dcc_command_registry_options_t;

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
    const dcc_command_registry_options_t *options,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_command_registry_apply(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
