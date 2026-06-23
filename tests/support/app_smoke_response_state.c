#include "app_smoke_response_state.h"

#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include "internal/app/dcc_app_internal.h"

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

int app_smoke_check_response_state(void) {
    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    dcc_flow_init(&ctx.flow, NULL, NULL);

    if (dcc_ctx_response_state(NULL) != DCC_INTERACTION_FLOW_FAILED ||
        strcmp(dcc_ctx_response_state_string(NULL), "failed") != 0 ||
        dcc_ctx_response_sent(NULL) ||
        dcc_ctx_deferred(NULL) ||
        dcc_ctx_replied(NULL) ||
        dcc_ctx_followed_up(NULL) ||
        !dcc_ctx_response_failed(NULL) ||
        dcc_ctx_can_followup(NULL) ||
        dcc_ctx_can_edit_original(NULL) ||
        DCC_CTX_RESPONSE_STATE(&ctx) != DCC_INTERACTION_FLOW_READY ||
        DCC_CTX_RESPONDED(&ctx) ||
        DCC_CTX_DEFERRED(&ctx) ||
        DCC_CTX_REPLIED(&ctx) ||
        DCC_CTX_FOLLOWED_UP(&ctx) ||
        DCC_CTX_RESPONSE_FAILED(&ctx) ||
        DCC_CTX_CAN_FOLLOWUP(&ctx) ||
        DCC_CTX_CAN_EDIT_ORIGINAL(&ctx) ||
        strcmp(DCC_CTX_RESPONSE_STATE_NAME(&ctx), "ready") != 0) {
        fprintf(stderr, "ctx response ready/null state mismatch\n");
        return 0;
    }

    ctx.flow.state = DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL;
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        !dcc_ctx_response_sent(&ctx) ||
        !dcc_ctx_deferred(&ctx) ||
        dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx) ||
        strcmp(dcc_ctx_response_state_string(&ctx), "deferred_ephemeral") != 0) {
        fprintf(stderr, "ctx response deferred state mismatch\n");
        return 0;
    }

    ctx.flow.state = DCC_INTERACTION_FLOW_REPLIED;
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_REPLIED ||
        !dcc_ctx_response_sent(&ctx) ||
        dcc_ctx_deferred(&ctx) ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response replied state mismatch\n");
        return 0;
    }

    ctx.flow.state = DCC_INTERACTION_FLOW_FOLLOWED_UP;
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FOLLOWED_UP ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_followed_up(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response followup state mismatch\n");
        return 0;
    }

    dcc_app_auto_defer_t auto_defer;
    memset(&auto_defer, 0, sizeof(auto_defer));
    atomic_init(&auto_defer.response_state, DCC_APP_RESPONSE_DEFERRED);
    auto_defer.ephemeral = 1U;
    ctx.flow.state = DCC_INTERACTION_FLOW_READY;
    ctx.auto_defer = &auto_defer;
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        !dcc_ctx_deferred(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response auto defer state mismatch\n");
        return 0;
    }

    atomic_store(&auto_defer.response_state, DCC_APP_RESPONSE_REPLIED);
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_REPLIED ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx)) {
        fprintf(stderr, "ctx response auto replied state mismatch\n");
        return 0;
    }

    atomic_store(&auto_defer.response_state, DCC_APP_RESPONSE_FAILED);
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FAILED ||
        !dcc_ctx_response_failed(&ctx) ||
        dcc_ctx_response_sent(&ctx)) {
        fprintf(stderr, "ctx response failed state mismatch\n");
        return 0;
    }

    return 1;
}
