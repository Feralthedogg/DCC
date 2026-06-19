#ifndef DCC_COMMAND_SYNC_INTERNAL_H
#define DCC_COMMAND_SYNC_INTERNAL_H

#include <dcc/dcc.h>

#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dcc_command_sync_options {
    const char *commands_path;
    const char *remote_path;
    const char *token;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    int delete_stale;
    int dry_run;
    int apply;
    int plan;
    int check_only;
} dcc_command_sync_options_t;

typedef struct dcc_command_sync_raw_store {
    char **items;
    size_t count;
    size_t cap;
} dcc_command_sync_raw_store_t;

typedef struct dcc_command_sync_local {
    dcc_command_registry_t registry;
    dcc_command_sync_raw_store_t raw;
    dcc_json_t *root;
} dcc_command_sync_local_t;

typedef struct dcc_command_sync_remote_body {
    char *body;
    size_t body_len;
    uint16_t status;
    dcc_status_t error;
    int called;
} dcc_command_sync_remote_body_t;

void dcc_command_sync_usage(FILE *stream, const char *argv0);
int dcc_command_sync_parse_options(
    int argc,
    char **argv,
    dcc_command_sync_options_t *options
);
int dcc_command_sync_validate_options(const dcc_command_sync_options_t *options);
int dcc_command_sync_parse_snowflake(const char *text, dcc_snowflake_t *out);
const char *dcc_command_sync_env_nonempty(const char *name);

dcc_status_t dcc_command_sync_load_file(
    const char *path,
    char **out,
    size_t *out_len
);
dcc_status_t dcc_command_sync_parse_local_commands(
    const char *json,
    size_t json_len,
    dcc_command_sync_local_t *out
);
void dcc_command_sync_local_deinit(dcc_command_sync_local_t *local);
dcc_status_t dcc_command_sync_read_remote_snapshot(
    const dcc_command_sync_options_t *options,
    dcc_client_t *client,
    dcc_command_registry_remote_snapshot_t *out
);
dcc_status_t dcc_command_sync_format_plan(
    const dcc_command_registry_plan_t *plan,
    char **out
);
void dcc_command_sync_fetch_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
);
void dcc_command_sync_apply_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
);

#endif
