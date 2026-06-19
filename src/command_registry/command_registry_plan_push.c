#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/dcc_core_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_command_registry_plan_count(
    dcc_command_registry_plan_t *plan,
    dcc_command_registry_diff_action_t action
) {
    switch (action) {
        case DCC_COMMAND_REGISTRY_CREATE:
            plan->create_count++;
            break;
        case DCC_COMMAND_REGISTRY_UPDATE:
            plan->update_count++;
            break;
        case DCC_COMMAND_REGISTRY_DELETE_STALE:
            plan->delete_stale_count++;
            break;
        case DCC_COMMAND_REGISTRY_NOOP:
        default:
            plan->noop_count++;
            break;
    }
}

static void dcc_command_registry_diff_free_owned(dcc_command_registry_diff_t *diff) {
    free((char *)diff->name);
    free((char *)diff->remote_json);
    memset(diff, 0, sizeof(*diff));
}

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
) {
    dcc_command_registry_plan_state_t *state = (dcc_command_registry_plan_state_t *)plan->state;
    if (state == NULL) {
        free(local_json);
        return DCC_ERR_INVALID_ARG;
    }
    if (state->diff_count == state->diff_cap) {
        if (state->diff_cap > SIZE_MAX / 2U) {
            free(local_json);
            return DCC_ERR_NOMEM;
        }
        size_t next_cap = state->diff_cap != 0U ? state->diff_cap * 2U : 8U;
        if (next_cap > SIZE_MAX / sizeof(*state->diffs)) {
            free(local_json);
            return DCC_ERR_NOMEM;
        }
        dcc_command_registry_diff_t *diffs = (dcc_command_registry_diff_t *)realloc(
            state->diffs,
            next_cap * sizeof(*state->diffs)
        );
        if (diffs == NULL) {
            free(local_json);
            return DCC_ERR_NOMEM;
        }
        state->diffs = diffs;
        state->diff_cap = next_cap;
        plan->diffs = state->diffs;
    }

    dcc_command_registry_diff_t *diff = &state->diffs[state->diff_count];
    memset(diff, 0, sizeof(*diff));
    diff->action = action;
    diff->name = dcc_strdup(name != NULL ? name : "");
    diff->type = type;
    diff->remote_id = remote_id;
    diff->local_index = local_index;
    diff->remote_index = remote_index;
    diff->local_json = local_json;
    diff->remote_json = remote_json != NULL ? dcc_strdup(remote_json) : NULL;
    if (diff->name == NULL ||
        (remote_json != NULL && diff->remote_json == NULL)) {
        dcc_command_registry_diff_free_owned(diff);
        free(local_json);
        return DCC_ERR_NOMEM;
    }

    state->diff_count++;
    plan->diff_count = state->diff_count;
    dcc_command_registry_plan_count(plan, action);
    return DCC_OK;
}
