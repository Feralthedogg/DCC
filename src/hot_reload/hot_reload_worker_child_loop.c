#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_session_receive_internal.h"
#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_worker_dispatch_json(
    dcc_client_t *client,
    const char *json,
    size_t json_len
) {
    static _Thread_local dcc_json_gateway_payload_t payload;
    dcc_status_t status = dcc_json_parse_gateway_payload(json, json_len, &payload);
    if (status != DCC_OK) {
        return status;
    }

    dcc_gateway_session_t session;
    memset(&session, 0, sizeof(session));
    session.client = client;
    return dcc_gateway_session_handle_payload(&session, &payload, json, json_len);
}

static int dcc_worker_send_health(dcc_hot_reload_t *hot_reload) {
    dcc_hot_reload_worker_health_t health = {
        .generation = hot_reload != NULL ? dcc_hot_reload_generation(hot_reload) : 0U,
        .status = DCC_OK,
        .reserved = 0U,
    };
    return dcc_hot_reload_worker_send_header(
               DCC_HOT_RELOAD_WORKER_FD_OUT,
               DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
               sizeof(health)
           ) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &health, sizeof(health)) != 0
        ? -1
        : 0;
}

static int dcc_worker_dispatch_event(
    dcc_client_t *client,
    const dcc_hot_reload_worker_event_t *event
) {
    char *json = (char *)malloc((size_t)event->json_len + 1U);
    if (json == NULL) {
        return 1;
    }
    if (dcc_hot_reload_worker_read_all(DCC_HOT_RELOAD_WORKER_FD_IN, json, (size_t)event->json_len) != 0) {
        free(json);
        return 1;
    }
    json[(size_t)event->json_len] = '\0';

    dcc_hot_reload_worker_capture_t capture;
    memset(&capture, 0, sizeof(capture));
    dcc_hot_reload_worker_capture_start(client, &capture);
    dcc_status_t status = dcc_worker_dispatch_json(client, json, (size_t)event->json_len);
    dcc_hot_reload_worker_capture_stop(client);
    free(json);

    int send_failed = dcc_hot_reload_worker_capture_send_result(&capture, status);
    dcc_hot_reload_worker_capture_deinit(&capture);
    return send_failed != 0 ? 1 : 0;
}

int dcc_hot_reload_worker_child_event_loop(dcc_hot_reload_t *hot_reload) {
    dcc_client_t *client = hot_reload != NULL ? hot_reload->client : NULL;
    for (;;) {
        dcc_hot_reload_worker_header_t header;
        if (dcc_hot_reload_worker_read_header(DCC_HOT_RELOAD_WORKER_FD_IN, &header, 0) != 0) {
            return 1;
        }
        if (header.kind == DCC_HOT_RELOAD_WORKER_MSG_STOP) {
            return 0;
        }
        if (header.kind == DCC_HOT_RELOAD_WORKER_MSG_HEALTH && header.size == 0U) {
            if (dcc_worker_send_health(hot_reload) != 0) {
                return 1;
            }
            continue;
        }
        if (header.kind != DCC_HOT_RELOAD_WORKER_MSG_EVENT ||
            header.size != sizeof(dcc_hot_reload_worker_event_t)) {
            return 1;
        }

        dcc_hot_reload_worker_event_t event;
        if (dcc_hot_reload_worker_read_all(DCC_HOT_RELOAD_WORKER_FD_IN, &event, sizeof(event)) != 0 ||
            event.json_len > (uint64_t)SIZE_MAX - 1U ||
            dcc_worker_dispatch_event(client, &event) != 0) {
            return 1;
        }
    }
}
