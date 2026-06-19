#include "gateway_smoke_handlers.h"

#include <string.h>

void on_message_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_message_t *message = dcc_event_message(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    if (message != NULL &&
        data != NULL &&
        message->id == 111 &&
        message->channel_id == 222 &&
        message->guild_id == 333 &&
        message->flags == 64 &&
        message->webhook_id == 777 &&
        message->type == 19 &&
        message->tts == 0 &&
        message->mention_everyone == 1 &&
        message->pinned == 1 &&
        message->content != NULL &&
        strcmp(message->content, "hello DCC") == 0 &&
        message->timestamp != NULL &&
        strcmp(message->timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
        message->edited_timestamp != NULL &&
        strcmp(message->edited_timestamp, "2026-06-15T01:00:00.000000+00:00") == 0 &&
        message->author.id == 444 &&
        message->author.username != NULL &&
        strcmp(message->author.username, "feral") == 0 &&
        message->author.global_name != NULL &&
        strcmp(message->author.global_name, "Feral") == 0 &&
        message->author.discriminator == 0 &&
        message->author.bot == 0 &&
        data->timestamp_text != NULL &&
        strcmp(data->timestamp_text, "2026-06-15T00:00:00.000000+00:00") == 0) {
        dcc_message_t *copy = dcc_message_clone(message);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->content != NULL &&
            strcmp(copy->content, "hello DCC") == 0 &&
            copy->timestamp != NULL &&
            strcmp(copy->timestamp, "2026-06-15T00:00:00.000000+00:00") == 0 &&
            copy->edited_timestamp != NULL &&
            strcmp(copy->edited_timestamp, "2026-06-15T01:00:00.000000+00:00") == 0 &&
            copy->webhook_id == 777 &&
            copy->type == 19 &&
            copy->tts == 0 &&
            copy->mention_everyone == 1 &&
            copy->pinned == 1 &&
            data_copy != NULL &&
            data_copy->timestamp_text != NULL &&
            strcmp(data_copy->timestamp_text, "2026-06-15T00:00:00.000000+00:00") == 0) {
            state->message_create_seen = 1;
        }
        dcc_gateway_event_data_free(data_copy);
        dcc_message_free(copy);
    }
}

void on_message_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_message_t *message = dcc_event_message(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    if (message != NULL &&
        data != NULL &&
        message->id == 111 &&
        message->channel_id == 222 &&
        message->guild_id == 333 &&
        data->id == 111 &&
        data->channel_id == 222 &&
        data->guild_id == 333 &&
        data->content != NULL &&
        strcmp(data->content, "edited") == 0 &&
        message->content != NULL &&
        strcmp(message->content, "edited") == 0 &&
        message->edited_timestamp != NULL &&
        strcmp(message->edited_timestamp, "2026-06-15T02:00:00.000000+00:00") == 0) {
        dcc_message_t *copy = dcc_message_clone(message);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->content != NULL &&
            strcmp(copy->content, "edited") == 0 &&
            copy->edited_timestamp != NULL &&
            strcmp(copy->edited_timestamp, "2026-06-15T02:00:00.000000+00:00") == 0 &&
            data_copy != NULL &&
            data_copy->content != NULL &&
            strcmp(data_copy->content, "edited") == 0) {
            state->message_update_seen = 1;
        }
        dcc_gateway_event_data_free(data_copy);
        dcc_message_free(copy);
    }
}

void on_message_delete(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_message_t *message = dcc_event_message(event);
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    if (message != NULL &&
        data != NULL &&
        message->id == 111 &&
        message->channel_id == 222 &&
        message->guild_id == 333 &&
        data->id == 111 &&
        data->channel_id == 222 &&
        data->guild_id == 333 &&
        message->content == NULL) {
        dcc_message_t *copy = dcc_message_clone(message);
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        if (copy != NULL &&
            copy->id == 111 &&
            copy->content == NULL &&
            data_copy != NULL &&
            data_copy->id == 111 &&
            data_copy->channel_id == 222 &&
            data_copy->guild_id == 333) {
            state->message_delete_seen = 1;
        }
        dcc_gateway_event_data_free(data_copy);
        dcc_message_free(copy);
    }
}
