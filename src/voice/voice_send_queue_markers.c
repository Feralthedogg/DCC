#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

static char *dcc_voice_strdup(const char *text) {
    if (text == NULL) {
        return NULL;
    }

    size_t len = strlen(text);
    if (len == SIZE_MAX) {
        return NULL;
    }

    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, text, len + 1U);
    return copy;
}

dcc_status_t dcc_voice_client_insert_marker(dcc_voice_client_t *voice_client, const char *track_meta) {
    if (voice_client == NULL || track_meta == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *copy = dcc_voice_strdup(track_meta);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_voice_send_item_t item;
    memset(&item, 0, sizeof(item));
    item.type = DCC_VOICE_SEND_ITEM_MARKER;
    item.track_meta = copy;
    return dcc_voice_send_queue_push(voice_client, &item);
}

dcc_status_t dcc_voice_client_skip_to_next_marker(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    while (voice_client->send_queue_len > 0) {
        dcc_voice_send_item_type_t type = voice_client->send_queue[0].type;
        dcc_status_t status = dcc_voice_send_queue_pop_front(voice_client);
        if (status != DCC_OK) {
            return status;
        }
        if (type == DCC_VOICE_SEND_ITEM_MARKER) {
            break;
        }
    }
    return DCC_OK;
}

size_t dcc_voice_client_send_queue_size(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->send_queue_len : 0;
}

uint32_t dcc_voice_client_tracks_remaining(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->send_queue_len == 0) {
        return 0;
    }

    size_t tracks = dcc_voice_send_queue_marker_count(voice_client) + 1U;
    return tracks > UINT32_MAX ? UINT32_MAX : (uint32_t)tracks;
}
