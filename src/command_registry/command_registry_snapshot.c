#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_command_registry_remote_snapshot_deinit(
    dcc_command_registry_remote_snapshot_t *snapshot
) {
    if (snapshot == NULL) {
        return;
    }
    dcc_command_registry_remote_snapshot_state_t *state =
        (dcc_command_registry_remote_snapshot_state_t *)snapshot->state;
    if (state != NULL) {
        for (size_t i = 0; i < state->entry_count; ++i) {
            free((char *)state->entries[i].name);
            free((char *)state->entries[i].canonical_json);
        }
        free(state->entries);
        free(state);
    }
    memset(snapshot, 0, sizeof(*snapshot));
}

static dcc_status_t dcc_command_registry_snapshot_alloc(
    const dcc_json_t *root,
    dcc_command_registry_remote_snapshot_t *out
) {
    size_t count = dcc_json_array_size(root);
    dcc_command_registry_remote_snapshot_state_t *state =
        (dcc_command_registry_remote_snapshot_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (count != 0) {
        if (count > SIZE_MAX / sizeof(*state->entries)) {
            free(state);
            return DCC_ERR_NOMEM;
        }
        state->entries = (dcc_command_registry_remote_entry_t *)calloc(count, sizeof(*state->entries));
        if (state->entries == NULL) {
            free(state);
            return DCC_ERR_NOMEM;
        }
    }
    state->entry_count = count;
    out->size = sizeof(*out);
    out->state = state;
    out->entries = state->entries;
    out->entry_count = state->entry_count;
    return DCC_OK;
}

dcc_status_t dcc_command_registry_remote_snapshot_parse_json(
    const char *json,
    size_t json_len,
    dcc_command_registry_remote_snapshot_t *out
) {
    if (json == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(json, json_len, &root);
    if (status != DCC_OK) {
        return status;
    }
    if (dcc_json_typeof(root) != DCC_JSON_ARRAY) {
        dcc_json_free(root);
        return DCC_ERR_JSON;
    }

    status = dcc_command_registry_snapshot_alloc(root, out);
    if (status == DCC_OK) {
        dcc_command_registry_remote_snapshot_state_t *state =
            (dcc_command_registry_remote_snapshot_state_t *)out->state;
        for (size_t i = 0; i < state->entry_count && status == DCC_OK; ++i) {
            status = dcc_command_registry_remote_entry_from_json(
                dcc_json_array_get(root, i),
                &state->entries[i]
            );
        }
    }
    dcc_json_free(root);
    if (status != DCC_OK) {
        dcc_command_registry_remote_snapshot_deinit(out);
    }
    return status;
}
