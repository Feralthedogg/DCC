#include "gateway_smoke_handlers.h"

#include <string.h>

void on_raw_dispatch(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    if (dcc_event_type(event) == DCC_EVENT_RAW && raw != NULL) {
        gateway_state_mark_raw_event(state, raw->type);
    }
    if (dcc_event_type(event) == DCC_EVENT_RAW &&
        raw != NULL &&
        raw->type == DCC_EVENT_SUBSCRIPTION_CREATE &&
        raw->sequence == 10 &&
        dcc_event_data(event) != NULL &&
        dcc_event_data(event)->sku_id == 123 &&
        dcc_event_data(event)->sku_ids != NULL &&
        dcc_event_data(event)->sku_ids_count == 2 &&
        raw->name != NULL &&
        strcmp(raw->name, "SUBSCRIPTION_CREATE") == 0 &&
        raw->json != NULL &&
        raw->json_len > 0 &&
        strstr(raw->json, "\"sku_id\":\"123\"") != NULL) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(dcc_event_data(event));
        if (copy != NULL &&
            copy->sku_id == 123 &&
            copy->sku_ids != NULL &&
            copy->sku_ids_count == 2 &&
            copy->sku_ids[0] == 123) {
            state->raw_subscription_seen = 1;
        }
        dcc_gateway_event_data_free(copy);
    }
}

void on_message_reaction_add(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_message_reaction_t *reaction = dcc_event_message_reaction(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data == NULL || reaction == NULL) {
        return;
    }

    if (type == DCC_EVENT_MESSAGE_REACTION_ADD &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->message_id == 111 &&
        data->user_id == 444 &&
        data->message_author_id == 445 &&
        data->emoji_id == 9999 &&
        data->emoji_name != NULL &&
        strcmp(data->emoji_name, "spark") == 0 &&
        data->user.id == 444 &&
        data->user.username != NULL &&
        strcmp(data->user.username, "reactor") == 0 &&
        data->nick != NULL &&
        strcmp(data->nick, "spark") == 0 &&
        data->role_ids != NULL &&
        data->role_ids_count == 1 &&
        data->role_ids[0] == 555 &&
        reaction->guild_id == 333 &&
        reaction->channel_id == 222 &&
        reaction->message_id == 111 &&
        reaction->user_id == 444 &&
        reaction->user.id == 444 &&
        reaction->user.username != NULL &&
        strcmp(reaction->user.username, "reactor") == 0 &&
        reaction->member.guild_id == 333 &&
        reaction->member.user.id == 444 &&
        reaction->member.user.username != NULL &&
        strcmp(reaction->member.user.username, "reactor") == 0 &&
        reaction->member.nick != NULL &&
        strcmp(reaction->member.nick, "spark") == 0 &&
        reaction->member.role_ids != NULL &&
        reaction->member.role_ids_count == 1 &&
        reaction->member.role_ids[0] == 555 &&
        reaction->member.flags == 4 &&
        reaction->message_author_id == 445 &&
        reaction->emoji_id == 9999 &&
        reaction->emoji_name != NULL &&
        reaction->emoji.id == 9999 &&
        reaction->emoji.name != NULL &&
        reaction->emoji.animated == 1 &&
        strcmp(reaction->emoji.name, "spark") == 0 &&
        strcmp(reaction->emoji_name, "spark") == 0) {
        dcc_message_reaction_t *copy = dcc_message_reaction_clone(reaction);
        if (copy != NULL &&
            copy->message_author_id == 445 &&
            copy->user.username != NULL &&
            strcmp(copy->user.username, "reactor") == 0 &&
            copy->member.nick != NULL &&
            strcmp(copy->member.nick, "spark") == 0 &&
            copy->member.role_ids != NULL &&
            copy->member.role_ids_count == 1 &&
            copy->member.role_ids[0] == 555 &&
            copy->emoji_name != NULL &&
            copy->emoji.id == 9999 &&
            copy->emoji.name != NULL &&
            copy->emoji.animated == 1 &&
            strcmp(copy->emoji.name, "spark") == 0 &&
            strcmp(copy->emoji_name, "spark") == 0) {
            state->reaction_add_seen = 1;
        }
        dcc_message_reaction_free(copy);
    } else if ((type == DCC_EVENT_MESSAGE_REACTION_REMOVE ||
                type == DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI) &&
               data->guild_id == 333 &&
               data->channel_id == 222 &&
               data->message_id == 111 &&
               data->user_id == (type == DCC_EVENT_MESSAGE_REACTION_REMOVE ? 446U : 0U) &&
               data->message_author_id == 0 &&
               data->emoji_id == 9999 &&
               data->emoji_name != NULL &&
               strcmp(data->emoji_name, "spark") == 0 &&
               reaction->guild_id == 333 &&
               reaction->channel_id == 222 &&
               reaction->message_id == 111 &&
               reaction->user_id == (type == DCC_EVENT_MESSAGE_REACTION_REMOVE ? 446U : 0U) &&
               reaction->message_author_id == 0 &&
               reaction->emoji_id == 9999 &&
               reaction->emoji_name != NULL &&
               reaction->emoji.id == 9999 &&
               reaction->emoji.name != NULL &&
               strcmp(reaction->emoji.name, "spark") == 0 &&
               strcmp(reaction->emoji_name, "spark") == 0) {
        dcc_message_reaction_t *copy = dcc_message_reaction_clone(reaction);
        if (copy != NULL &&
            copy->message_id == 111 &&
            copy->user_id == (type == DCC_EVENT_MESSAGE_REACTION_REMOVE ? 446U : 0U) &&
            copy->emoji_id == 9999 &&
            copy->emoji.name != NULL &&
            strcmp(copy->emoji.name, "spark") == 0) {
            if (type == DCC_EVENT_MESSAGE_REACTION_REMOVE) {
                state->reaction_remove_seen = 1;
            } else {
                state->reaction_remove_emoji_seen = 1;
            }
        }
        dcc_message_reaction_free(copy);
    } else if (type == DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL &&
               data->guild_id == 333 &&
               data->channel_id == 222 &&
               data->message_id == 111 &&
               data->user_id == 0 &&
               data->message_author_id == 0 &&
               data->emoji_id == 0 &&
               data->emoji_name == NULL &&
               reaction->guild_id == 333 &&
               reaction->channel_id == 222 &&
               reaction->message_id == 111 &&
               reaction->user_id == 0 &&
               reaction->message_author_id == 0 &&
               reaction->emoji_id == 0 &&
               reaction->emoji_name == NULL &&
               reaction->emoji.id == 0 &&
               reaction->emoji.name == NULL) {
        dcc_message_reaction_t *copy = dcc_message_reaction_clone(reaction);
        if (copy != NULL &&
            copy->message_id == 111 &&
            copy->user_id == 0 &&
            copy->emoji_id == 0 &&
            copy->emoji_name == NULL &&
            copy->emoji.name == NULL) {
            state->reaction_remove_all_seen = 1;
        }
        dcc_message_reaction_free(copy);
    }
}

void on_message_delete_bulk(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_message_delete_bulk_t *bulk_delete = dcc_event_message_delete_bulk(event);
    if (data != NULL &&
        bulk_delete != NULL &&
        dcc_event_type(event) == DCC_EVENT_MESSAGE_DELETE_BULK &&
        data->guild_id == 333 &&
        data->channel_id == 222 &&
        data->ids != NULL &&
        data->ids_count == 2 &&
        data->ids[0] == 111 &&
        data->ids[1] == 112 &&
        data->ids_truncated == 0 &&
        bulk_delete->guild_id == 333 &&
        bulk_delete->channel_id == 222 &&
        bulk_delete->ids != NULL &&
        bulk_delete->ids_count == 2 &&
        bulk_delete->ids[0] == 111 &&
        bulk_delete->ids[1] == 112 &&
        bulk_delete->ids_truncated == 0) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_message_delete_bulk_t *bulk_copy = dcc_message_delete_bulk_clone(bulk_delete);
        if (copy != NULL &&
            copy->ids != NULL &&
            copy->ids_count == 2 &&
            copy->ids[0] == 111 &&
            copy->ids[1] == 112 &&
            bulk_copy != NULL &&
            bulk_copy->ids != NULL &&
            bulk_copy->ids_count == 2 &&
            bulk_copy->ids[1] == 112) {
            state->bulk_delete_seen = 1;
        }
        dcc_message_delete_bulk_free(bulk_copy);
        dcc_gateway_event_data_free(copy);
    }
}
