#include "internal/voice/dcc_voice_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_voice_send_item_free(dcc_voice_send_item_t *item) {
    if (item == NULL) {
        return;
    }
    free(item->data);
    free(item->track_meta);
    memset(item, 0, sizeof(*item));
}

static dcc_status_t dcc_voice_send_queue_remove_at(dcc_voice_client_t *voice_client, size_t index) {
    if (voice_client == NULL || index >= voice_client->send_queue_len) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_send_item_free(&voice_client->send_queue[index]);
    for (size_t i = index + 1U; i < voice_client->send_queue_len; ++i) {
        voice_client->send_queue[i - 1U] = voice_client->send_queue[i];
    }
    voice_client->send_queue_len--;
    memset(&voice_client->send_queue[voice_client->send_queue_len], 0, sizeof(voice_client->send_queue[0]));
    return DCC_OK;
}

dcc_status_t dcc_voice_send_queue_pop_front(dcc_voice_client_t *voice_client) {
    return dcc_voice_send_queue_remove_at(voice_client, 0);
}

dcc_status_t dcc_voice_send_queue_push(dcc_voice_client_t *voice_client, dcc_voice_send_item_t *item) {
    if (voice_client == NULL || item == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->send_queue_len >= DCC_VOICE_SEND_QUEUE_CAP) {
        dcc_voice_send_item_free(item);
        return DCC_ERR_NOMEM;
    }

    voice_client->send_queue[voice_client->send_queue_len++] = *item;
    memset(item, 0, sizeof(*item));
    return DCC_OK;
}

size_t dcc_voice_send_queue_marker_count(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return 0;
    }

    size_t markers = 0;
    for (size_t i = 0; i < voice_client->send_queue_len; ++i) {
        if (voice_client->send_queue[i].type == DCC_VOICE_SEND_ITEM_MARKER) {
            markers++;
        }
    }
    return markers;
}

dcc_status_t dcc_voice_client_clear_send_queue(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    while (voice_client->send_queue_len > 0) {
        dcc_status_t status = dcc_voice_send_queue_pop_front(voice_client);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
