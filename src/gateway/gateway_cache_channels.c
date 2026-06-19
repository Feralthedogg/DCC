#include "internal/gateway/dcc_gateway_cache_internal.h"

uint8_t dcc_gateway_cache_apply_channel_event(dcc_client_t *client, const dcc_event_t *event) {
    switch (event->type) {
        case DCC_EVENT_CHANNEL_CREATE:
        case DCC_EVENT_THREAD_CREATE:
            dcc_gateway_cache_put_channel_if_valid(client, &event->data.channel);
            return 1U;
        case DCC_EVENT_CHANNEL_UPDATE:
        case DCC_EVENT_CHANNEL_INFO:
        case DCC_EVENT_THREAD_UPDATE:
        case DCC_EVENT_CHANNEL_TOPIC_UPDATE:
            if (event->data.channel.id != 0) {
                (void)dcc_cache_merge_channel(client, &event->data.channel, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_CHANNEL_DELETE:
        case DCC_EVENT_THREAD_DELETE:
            if (event->data.channel.id != 0) {
                (void)dcc_cache_remove_channel(client, event->data.channel.id);
            }
            return 1U;
        case DCC_EVENT_THREAD_LIST_SYNC:
            dcc_gateway_cache_put_channels(
                client,
                event->data.thread_list_sync.threads,
                event->data.thread_list_sync.threads_count
            );
            return 1U;
        default:
            return 0U;
    }
}
