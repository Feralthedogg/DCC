#include <dcc/dcc.h>
#include <dcc/sugar.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef enum wait_filter_smoke_kind {
    WAIT_FILTER_BUTTON = 1,
    WAIT_FILTER_BUTTON_PREFIX,
    WAIT_FILTER_COMPONENT,
    WAIT_FILTER_MESSAGE
} wait_filter_smoke_kind_t;

typedef struct wait_filter_smoke_thread {
    dcc_ctx_t *ctx;
    wait_filter_smoke_kind_t kind;
    atomic_int finished;
    dcc_status_t status;
    dcc_app_wait_result_t result;
} wait_filter_smoke_thread_t;

static void *wait_filter_smoke_thread_main(void *arg) {
    wait_filter_smoke_thread_t *state = (wait_filter_smoke_thread_t *)arg;
    switch (state->kind) {
        case WAIT_FILTER_BUTTON:
            state->status = DCC_WAIT_BUTTON(state->ctx, "target", 1000U, &state->result);
            break;
        case WAIT_FILTER_BUTTON_PREFIX:
            state->status = DCC_WAIT_BUTTON_PREFIX(state->ctx, "page:", 1000U, &state->result);
            break;
        case WAIT_FILTER_COMPONENT:
            state->status = DCC_WAIT_COMPONENT(state->ctx, 1000U, &state->result);
            break;
        case WAIT_FILTER_MESSAGE:
            state->status = DCC_WAIT_MESSAGE(state->ctx, 1000U, &state->result);
            break;
    }
    atomic_store_explicit(&state->finished, 1, memory_order_release);
    return NULL;
}

static dcc_status_t dispatch_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_snowflake_t id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    const char *custom_id
) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.interaction.id = id;
    event.data.interaction.user_id = user_id;
    event.data.interaction.channel_id = channel_id;
    event.data.interaction.guild_id = 222U;
    event.data.interaction.custom_id = custom_id;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_message(
    dcc_app_t *app,
    dcc_snowflake_t id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    const char *content
) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_MESSAGE_CREATE;
    event.data.message.id = id;
    event.data.message.author.id = user_id;
    event.data.message.channel_id = channel_id;
    event.data.message.guild_id = 222U;
    event.data.message.content = content;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_interaction_until_done(
    dcc_app_t *app,
    wait_filter_smoke_thread_t *state,
    dcc_event_type_t type,
    dcc_snowflake_t id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    const char *custom_id
) {
    dcc_status_t status = DCC_OK;
    for (unsigned attempt = 0U; attempt < 200U &&
         !atomic_load_explicit(&state->finished, memory_order_acquire); ++attempt) {
        status = dispatch_interaction(app, type, id, user_id, channel_id, custom_id);
        if (status != DCC_OK) {
            return status;
        }
        usleep(5000);
    }
    return status;
}

static dcc_status_t dispatch_message_until_done(
    dcc_app_t *app,
    wait_filter_smoke_thread_t *state,
    dcc_snowflake_t id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    const char *content
) {
    dcc_status_t status = DCC_OK;
    for (unsigned attempt = 0U; attempt < 200U &&
         !atomic_load_explicit(&state->finished, memory_order_acquire); ++attempt) {
        status = dispatch_message(app, id, user_id, channel_id, content);
        if (status != DCC_OK) {
            return status;
        }
        usleep(5000);
    }
    return status;
}

static int run_wait_case(dcc_app_t *app, dcc_ctx_t *ctx, wait_filter_smoke_kind_t kind) {
    wait_filter_smoke_thread_t state;
    memset(&state, 0, sizeof(state));
    state.ctx = ctx;
    state.kind = kind;
    atomic_init(&state.finished, 0);

    pthread_t thread;
    if (pthread_create(&thread, NULL, wait_filter_smoke_thread_main, &state) != 0) {
        fprintf(stderr, "wait thread create failed\n");
        return 0;
    }
    usleep(10000);

    dcc_status_t status = DCC_OK;
    switch (kind) {
        case WAIT_FILTER_BUTTON:
            status = dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, 1U, 999U, 111U, "target");
            if (status == DCC_OK) {
                status = dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, 2U, 333U, 111U, "other");
            }
            if (status == DCC_OK) {
                status = dispatch_interaction_until_done(
                    app,
                    &state,
                    DCC_EVENT_BUTTON_CLICK,
                    3U,
                    333U,
                    111U,
                    "target"
                );
            }
            break;
        case WAIT_FILTER_BUTTON_PREFIX:
            status = dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, 4U, 333U, 111U, "pager:bad");
            if (status == DCC_OK) {
                status = dispatch_interaction_until_done(
                    app,
                    &state,
                    DCC_EVENT_BUTTON_CLICK,
                    5U,
                    333U,
                    111U,
                    "page:next"
                );
            }
            break;
        case WAIT_FILTER_COMPONENT:
            status = dispatch_interaction(app, DCC_EVENT_SELECT_CLICK, 6U, 333U, 999U, "choice");
            if (status == DCC_OK) {
                status = dispatch_interaction_until_done(
                    app,
                    &state,
                    DCC_EVENT_SELECT_CLICK,
                    7U,
                    333U,
                    111U,
                    "choice"
                );
            }
            break;
        case WAIT_FILTER_MESSAGE:
            status = dispatch_message(app, 8U, 999U, 111U, "wrong user");
            if (status == DCC_OK) {
                status = dispatch_message_until_done(
                    app,
                    &state,
                    9U,
                    333U,
                    111U,
                    "matched message"
                );
            }
            break;
    }

    (void)pthread_join(thread, NULL);
    if (status != DCC_OK || state.status != DCC_OK) {
        fprintf(
            stderr,
            "wait case failed: kind=%d dispatch=%s wait=%s\n",
            (int)kind,
            dcc_status_string(status),
            dcc_status_string(state.status)
        );
        DCC_WAIT_RESULT_DEINIT(&state.result);
        return 0;
    }

    int ok = 0;
    switch (kind) {
        case WAIT_FILTER_BUTTON:
            ok = state.result.interaction != NULL &&
                DCC_WAIT_TYPE(&state.result) == DCC_EVENT_BUTTON_CLICK &&
                DCC_WAIT_ID(&state.result) == 3U &&
                DCC_WAIT_USER_ID(&state.result) == 333U &&
                DCC_WAIT_CHANNEL_ID(&state.result) == 111U &&
                DCC_WAIT_GUILD_ID(&state.result) == 222U &&
                strcmp(DCC_WAIT_CUSTOM_ID(&state.result), "target") == 0;
            break;
        case WAIT_FILTER_BUTTON_PREFIX:
            ok = state.result.interaction != NULL &&
                DCC_WAIT_ID(&state.result) == 5U &&
                strcmp(DCC_WAIT_CUSTOM_ID(&state.result), "page:next") == 0;
            break;
        case WAIT_FILTER_COMPONENT:
            ok = state.result.interaction != NULL &&
                DCC_WAIT_INTERACTION(&state.result) == state.result.interaction &&
                DCC_WAIT_ID(&state.result) == 7U &&
                DCC_WAIT_TYPE(&state.result) == DCC_EVENT_SELECT_CLICK;
            break;
        case WAIT_FILTER_MESSAGE:
            ok = state.result.message != NULL &&
                DCC_WAIT_MESSAGE_OBJECT(&state.result) == state.result.message &&
                DCC_WAIT_ID(&state.result) == 9U &&
                DCC_WAIT_USER_ID(&state.result) == 333U &&
                strcmp(DCC_WAIT_MESSAGE_CONTENT(&state.result, ""), "matched message") == 0;
            break;
    }
    DCC_WAIT_RESULT_DEINIT(&state.result);
    return ok;
}

static int run_wait_result_accessor_smoke(void) {
    const char *values[] = { "alpha", "beta" };
    const char *tags[] = { "red", "blue" };
    dcc_interaction_form_field_t fields[] = {
        {
            .custom_id = "name",
            .value_type = DCC_INTERACTION_FORM_VALUE_STRING,
            .value = "Feral"
        },
        {
            .custom_id = "visible",
            .value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN,
            .boolean_value = 1U
        },
        {
            .custom_id = "tags",
            .value_type = DCC_INTERACTION_FORM_VALUE_VALUES,
            .values = tags,
            .values_count = 2U
        }
    };
    dcc_interaction_t interaction;
    memset(&interaction, 0, sizeof(interaction));
    interaction.id = 44U;
    interaction.user_id = 55U;
    interaction.channel_id = 66U;
    interaction.guild_id = 77U;
    interaction.custom_id = "select.tags";
    interaction.values = values;
    interaction.values_count = 2U;
    interaction.form_fields = fields;
    interaction.form_fields_count = 3U;

    dcc_message_t message;
    memset(&message, 0, sizeof(message));
    message.id = 88U;
    message.author.id = 99U;
    message.channel_id = 100U;
    message.guild_id = 101U;
    message.content = "hello";

    dcc_app_wait_result_t interaction_result = {
        .size = sizeof(interaction_result),
        .snapshot = { .type = DCC_EVENT_SELECT_CLICK },
        .interaction = &interaction
    };
    dcc_app_wait_result_t message_result = {
        .size = sizeof(message_result),
        .snapshot = { .type = DCC_EVENT_MESSAGE_CREATE },
        .message = &message
    };

    return DCC_WAIT_TYPE(NULL) == 0 &&
        DCC_WAIT_INTERACTION(NULL) == NULL &&
        DCC_WAIT_MESSAGE_OBJECT(NULL) == NULL &&
        DCC_WAIT_ID(NULL) == 0U &&
        DCC_WAIT_CUSTOM_ID(NULL) == NULL &&
        DCC_WAIT_VALUE_COUNT(NULL) == 0U &&
        DCC_WAIT_VALUE(NULL, 0U) == NULL &&
        strcmp(DCC_WAIT_MESSAGE_CONTENT(NULL, "fallback"), "fallback") == 0 &&
        DCC_WAIT_TYPE(&interaction_result) == DCC_EVENT_SELECT_CLICK &&
        DCC_WAIT_ID(&interaction_result) == 44U &&
        DCC_WAIT_USER_ID(&interaction_result) == 55U &&
        DCC_WAIT_CHANNEL_ID(&interaction_result) == 66U &&
        DCC_WAIT_GUILD_ID(&interaction_result) == 77U &&
        strcmp(DCC_WAIT_CUSTOM_ID(&interaction_result), "select.tags") == 0 &&
        DCC_WAIT_VALUE_COUNT(&interaction_result) == 2U &&
        strcmp(DCC_WAIT_VALUE(&interaction_result, 0U), "alpha") == 0 &&
        strcmp(DCC_WAIT_VALUE(&interaction_result, 1U), "beta") == 0 &&
        DCC_WAIT_VALUE(&interaction_result, 2U) == NULL &&
        DCC_WAIT_FORM_FIELD(&interaction_result, "tags") == &fields[2] &&
        strcmp(DCC_WAIT_FORM_TEXT(&interaction_result, "name", ""), "Feral") == 0 &&
        DCC_WAIT_FORM_CHECKED(&interaction_result, "visible", 0U) == 1U &&
        DCC_WAIT_FORM_CHECKED(&interaction_result, "missing", 1U) == 1U &&
        DCC_WAIT_FORM_VALUE_COUNT(&interaction_result, "tags") == 2U &&
        strcmp(DCC_WAIT_FORM_VALUE(&interaction_result, "tags", 0U), "red") == 0 &&
        strcmp(DCC_WAIT_FORM_VALUE(&interaction_result, "tags", 1U), "blue") == 0 &&
        DCC_WAIT_FORM_VALUE(&interaction_result, "tags", 2U) == NULL &&
        DCC_WAIT_TYPE(&message_result) == DCC_EVENT_MESSAGE_CREATE &&
        DCC_WAIT_ID(&message_result) == 88U &&
        DCC_WAIT_USER_ID(&message_result) == 99U &&
        DCC_WAIT_CHANNEL_ID(&message_result) == 100U &&
        DCC_WAIT_GUILD_ID(&message_result) == 101U &&
        strcmp(DCC_WAIT_MESSAGE_CONTENT(&message_result, ""), "hello") == 0;
}

int main(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS | DCC_INTENT_GUILD_MESSAGES | DCC_INTENT_MESSAGE_CONTENT;

    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK || app == NULL) {
        fprintf(stderr, "app create failed: %s\n", dcc_status_string(status));
        return 1;
    }
    status = dcc_app_start(app);
    if (status != DCC_OK) {
        fprintf(stderr, "app start failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }

    dcc_interaction_t interaction;
    memset(&interaction, 0, sizeof(interaction));
    interaction.user_id = 333U;
    interaction.channel_id = 111U;
    interaction.guild_id = 222U;

    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.app = app;
    ctx.client = dcc_app_client(app);
    ctx.interaction = &interaction;

    dcc_app_wait_filter_t filter = DCC_WAIT_FILTER_CTX(&ctx);
    if (filter.user_id != 333U || filter.channel_id != 111U || filter.guild_id != 222U ||
        dcc_app_wait_filter_from_ctx(NULL, &filter) != DCC_ERR_INVALID_ARG ||
        DCC_WAIT_FOR_BUTTON_PREFIX(&ctx, "", 1U, &(dcc_app_wait_result_t){0}) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_WAIT_FOR_BUTTON(NULL, 1U, &(dcc_app_wait_result_t){0}) != DCC_ERR_INVALID_ARG ||
        DCC_APP_WAIT_FOR_MESSAGE(NULL, 1U, &(dcc_app_wait_result_t){0}) != DCC_ERR_INVALID_ARG ||
        !run_wait_result_accessor_smoke() ||
        !run_wait_case(app, &ctx, WAIT_FILTER_BUTTON) ||
        !run_wait_case(app, &ctx, WAIT_FILTER_BUTTON_PREFIX) ||
        !run_wait_case(app, &ctx, WAIT_FILTER_COMPONENT) ||
        !run_wait_case(app, &ctx, WAIT_FILTER_MESSAGE)) {
        fprintf(stderr, "wait filter smoke failed\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_app_destroy(app);
    return 0;
}

#endif
