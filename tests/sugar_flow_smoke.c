#include <dcc/sugar/flow.h>

#include <stdio.h>
#include <string.h>

static int expect(const char *name, int ok) {
    if (!ok) {
        fprintf(stderr, "flow sugar check failed: %s\n", name);
        return 0;
    }
    return 1;
}

int main(void) {
    dcc_client_t *client = (dcc_client_t *)1;
    const dcc_interaction_t *interaction = (const dcc_interaction_t *)2;
    dcc_interaction_flow_t flow = DCC_FLOW_STARTED(client, interaction, 1000U);
    dcc_interaction_flow_t auto_flow =
        DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED(client, interaction, 2000U, 1500U);
    dcc_interaction_flow_t private_auto_flow =
        DCC_FLOW_AUTO_DEFER_PRIVATE_STARTED(client, interaction, 3000U, 2000U);
    dcc_modal_builder_t modal = {
        .custom_id = "flow.modal",
        .title = "Flow modal",
        .has_custom_id = 1U,
        .has_title = 1U
    };

    if (!expect("constructor size", flow.size == sizeof(flow)) ||
        !expect("constructor client", flow.client == client) ||
        !expect("constructor interaction", flow.interaction == interaction) ||
        !expect("constructor state", flow.state == DCC_INTERACTION_FLOW_READY) ||
        !expect("constructor started", flow.started_at_ms == 1000U) ||
        !expect("auto defer started", auto_flow.started_at_ms == 2000U) ||
        !expect("auto defer after", auto_flow.auto_defer_after_ms == 1500U) ||
        !expect("auto defer ephemeral", auto_flow.auto_defer_ephemeral == 1U) ||
        !expect("private auto defer started", private_auto_flow.started_at_ms == 3000U) ||
        !expect("private auto defer after", private_auto_flow.auto_defer_after_ms == 2000U) ||
        !expect("private auto defer ephemeral", private_auto_flow.auto_defer_ephemeral == 1U) ||
        !expect("state null", DCC_FLOW_STATE(NULL) == DCC_INTERACTION_FLOW_FAILED) ||
        !expect("state name null", strcmp(DCC_FLOW_STATE_NAME(NULL), "failed") == 0) ||
        !expect("message text", strcmp(DCC_FLOW_MESSAGE_TEXT("pong").content, "pong") == 0) ||
        !expect("message text flag", DCC_FLOW_MESSAGE_TEXT("pong").has_content == 1U) ||
        !expect("defer invalid", DCC_FLOW_DEFER(NULL) == DCC_ERR_INVALID_ARG) ||
        !expect("defer cb invalid", DCC_FLOW_DEFER_CB(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG) ||
        !expect("defer ephemeral invalid", DCC_FLOW_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG) ||
        !expect("defer private invalid", DCC_FLOW_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG) ||
        !expect("defer update invalid", DCC_FLOW_DEFER_UPDATE(NULL) == DCC_ERR_INVALID_ARG) ||
        !expect("maybe auto defer invalid", DCC_FLOW_MAYBE_AUTO_DEFER(NULL, 3000U) == DCC_ERR_INVALID_ARG) ||
        !expect("reply invalid", DCC_FLOW_REPLY(NULL, DCC_FLOW_MESSAGE_TEXT("pong")) == DCC_ERR_INVALID_ARG) ||
        !expect("reply text invalid", DCC_FLOW_REPLY_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
        !expect("reply private invalid", DCC_FLOW_REPLY_PRIVATE(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
        !expect("reply silent invalid", DCC_FLOW_REPLY_SILENT(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
        !expect("edit invalid", DCC_FLOW_EDIT_TEXT(NULL, "edited") == DCC_ERR_INVALID_ARG) ||
        !expect("followup invalid", DCC_FLOW_FOLLOWUP_TEXT(NULL, "next") == DCC_ERR_INVALID_ARG) ||
        !expect("followup private invalid", DCC_FLOW_FOLLOWUP_PRIVATE(NULL, "next") == DCC_ERR_INVALID_ARG) ||
        !expect("followup silent invalid", DCC_FLOW_FOLLOWUP_SILENT(NULL, "next") == DCC_ERR_INVALID_ARG) ||
        !expect("modal invalid", DCC_FLOW_SHOW_MODAL(NULL, modal) == DCC_ERR_INVALID_ARG)) {
        return 1;
    }

    return 0;
}
