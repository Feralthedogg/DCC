#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_command_registry_diff_deinit(dcc_command_registry_diff_t *diff) {
    if (diff == NULL) {
        return;
    }
    free((char *)diff->name);
    free((char *)diff->local_json);
    free((char *)diff->remote_json);
    memset(diff, 0, sizeof(*diff));
}

dcc_status_t dcc_command_registry_plan_state_init(dcc_command_registry_plan_t *plan) {
    dcc_command_registry_plan_state_t *state = (dcc_command_registry_plan_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    plan->state = state;
    plan->diffs = state->diffs;
    return DCC_OK;
}

void dcc_command_registry_plan_deinit(dcc_command_registry_plan_t *plan) {
    if (plan == NULL) {
        return;
    }
    dcc_command_registry_plan_state_t *state = (dcc_command_registry_plan_state_t *)plan->state;
    if (state != NULL) {
        for (size_t i = 0; i < state->diff_count; ++i) {
            dcc_command_registry_diff_deinit(&state->diffs[i]);
        }
        free(state->diffs);
        free(state);
    }
    memset(plan, 0, sizeof(*plan));
}
