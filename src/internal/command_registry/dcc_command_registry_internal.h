#ifndef DCC_COMMAND_REGISTRY_INTERNAL_H
#define DCC_COMMAND_REGISTRY_INTERNAL_H

#include <dcc/command_registry.h>

#include "internal/json/dcc_json.h"
#include "internal/objects/dcc_application_command_builder_internal.h"

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_command_registry_state {
    dcc_command_registry_entry_t *entries;
    size_t entry_count;
    size_t entry_cap;
} dcc_command_registry_state_t;

typedef struct dcc_command_registry_plan_state {
    dcc_command_registry_diff_t *diffs;
    size_t diff_count;
    size_t diff_cap;
} dcc_command_registry_plan_state_t;

typedef struct dcc_command_registry_remote_snapshot_state {
    dcc_command_registry_remote_entry_t *entries;
    size_t entry_count;
} dcc_command_registry_remote_snapshot_state_t;

dcc_command_registry_state_t *dcc_command_registry_state_get(
    dcc_command_registry_t *registry,
    int create
);
const dcc_command_registry_state_t *dcc_command_registry_state_const(
    const dcc_command_registry_t *registry
);
uint32_t dcc_command_registry_builder_type(const dcc_application_command_builder_t *command);
dcc_snowflake_t dcc_command_registry_options_guild_id(
    const dcc_command_registry_options_t *options
);
uint8_t dcc_command_registry_options_delete_stale(
    const dcc_command_registry_options_t *options
);
uint8_t dcc_command_registry_options_dry_run(
    const dcc_command_registry_options_t *options
);
dcc_status_t dcc_command_registry_plan_state_init(dcc_command_registry_plan_t *plan);
dcc_status_t dcc_command_registry_plan_push(
    dcc_command_registry_plan_t *plan,
    dcc_command_registry_diff_action_t action,
    const char *name,
    uint32_t type,
    dcc_snowflake_t remote_id,
    size_t local_index,
    size_t remote_index,
    char *local_json,
    const char *remote_json
);
const char *dcc_command_registry_remote_name(const dcc_command_registry_remote_entry_t *remote);
uint32_t dcc_command_registry_remote_type(const dcc_command_registry_remote_entry_t *remote);
dcc_snowflake_t dcc_command_registry_remote_id(const dcc_command_registry_remote_entry_t *remote);
const char *dcc_command_registry_remote_json(const dcc_command_registry_remote_entry_t *remote);
size_t dcc_command_registry_find_remote(
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    const dcc_application_command_builder_t *command
);
int dcc_command_registry_remote_has_local(
    const dcc_command_registry_state_t *state,
    const dcc_command_registry_remote_entry_t *remote
);
dcc_snowflake_t dcc_command_registry_plan_guild_id(
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan
);
dcc_status_t dcc_command_registry_apply_create(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const char *json,
    dcc_rest_cb cb,
    void *user_data
);
dcc_status_t dcc_command_registry_apply_update(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const char *json,
    dcc_rest_cb cb,
    void *user_data
);
dcc_status_t dcc_command_registry_apply_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
);
dcc_status_t dcc_command_registry_format_append(
    char *buf,
    size_t buf_len,
    size_t *used,
    const char *fmt,
    ...
);
dcc_status_t dcc_command_registry_append_json_value(
    dcc_application_command_json_buffer_t *buffer,
    const dcc_json_t *json
);
dcc_status_t dcc_command_registry_remote_entry_from_json(
    const dcc_json_t *json,
    dcc_command_registry_remote_entry_t *entry
);

#endif
