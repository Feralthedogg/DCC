#include <dcc/dcc.h>

#include <stdio.h>
#include <stdint.h>

typedef struct bot_module_state {
    uint64_t ready_count;
    uint64_t message_count;
} bot_module_state_t;

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    bot_module_state_t *state = (bot_module_state_t *)user_data;
    if (state != NULL) {
        state->ready_count++;
    }
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    fprintf(stderr,
            "bot module ready #%llu on shard %u/%u session=%s\n",
            (unsigned long long)(state != NULL ? state->ready_count : 0),
            ready != NULL ? ready->shard_id : 0,
            ready != NULL ? ready->shard_count : 0,
            ready != NULL && ready->session_id != NULL ? ready->session_id : "");
}

static void on_message(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    bot_module_state_t *state = (bot_module_state_t *)user_data;
    if (state != NULL) {
        state->message_count++;
    }
    const dcc_message_t *message = dcc_event_message(event);
    if (message != NULL && message->content != NULL) {
        fprintf(stderr,
                "message #%llu: %s\n",
                (unsigned long long)(state != NULL ? state->message_count : 0),
                message->content);
    }
}

static const dcc_bot_module_handler_t bot_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, on_ready),
    DCC_BOT_MODULE_ON(DCC_EVENT_MESSAGE_CREATE, on_message),
};

DCC_BOT_MODULE_STATE(bot_module_state_t, bot_handlers)
