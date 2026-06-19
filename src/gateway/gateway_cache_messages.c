#include "internal/gateway/dcc_gateway_cache_internal.h"

uint8_t dcc_gateway_cache_apply_message_event(dcc_client_t *client, const dcc_event_t *event) {
    switch (event->type) {
        case DCC_EVENT_MESSAGE_CREATE:
            dcc_gateway_cache_put_message_if_valid(client, &event->data.message);
            return 1U;
        case DCC_EVENT_MESSAGE_UPDATE:
            if (event->data.message.id != 0) {
                dcc_gateway_cache_put_user_if_valid(client, &event->data.message.author);
                (void)dcc_cache_merge_message(client, &event->data.message, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_MESSAGE_DELETE:
            if (event->data.message.id != 0) {
                (void)dcc_cache_remove_message(client, event->data.message.id);
            }
            return 1U;
        case DCC_EVENT_MESSAGE_DELETE_BULK:
            dcc_gateway_cache_remove_messages(
                client,
                event->data.message_delete_bulk.ids,
                event->data.message_delete_bulk.ids_count
            );
            return 1U;
        default:
            return 0U;
    }
}
