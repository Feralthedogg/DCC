#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"

#include <llam/runtime.h>

#include <string.h>

static void dcc_client_main_task(void *arg) {
    dcc_client_t *client = (dcc_client_t *)arg;
    dcc_emit_log(client, DCC_LOG_INFO, "dcc runtime started");

    if ((client->gateway_url == NULL || client->gateway_url[0] == '\0') &&
        (client->token == NULL || client->token[0] == '\0')) {
        dcc_event_t ready;
        memset(&ready, 0, sizeof(ready));
        ready.type = DCC_EVENT_READY;
        ready.raw.type = DCC_EVENT_READY;
        ready.raw.name = dcc_event_type_name(DCC_EVENT_READY);
        ready.data.ready.shard_id = client->shard_id;
        ready.data.ready.shard_count = client->shard_count;
        ready.data.ready.session_id = "";
        (void)dcc_event_bus_dispatch(&client->events, client, &ready);
    }

    while (!atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        (void)llam_sleep_ns(100000000ULL);
    }

    dcc_emit_log(client, DCC_LOG_INFO, "dcc runtime stopping");
}

dcc_status_t dcc_client_start(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t st = dcc_client_lifecycle_admission_enter(
        &client->start_admission
    );
    if (st != DCC_OK) {
        dcc_set_error(client, "client teardown has started");
        return st;
    }

    bool expected = false;
    if (!atomic_compare_exchange_strong(&client->started, &expected, true)) {
        dcc_set_error(client, "client is already started");
        st = DCC_ERR_STATE;
        goto done;
    }

    atomic_store_explicit(&client->stopping, false, memory_order_release);

    st = dcc_runtime_init(&client->runtime);
    if (st != DCC_OK) {
        atomic_store(&client->started, false);
        dcc_set_error(client, "failed to initialize LLAM runtime");
        goto done;
    }

    st = dcc_gateway_start(client);
    if (st != DCC_OK) {
        atomic_store(&client->started, false);
        goto done;
    }

    st = dcc_runtime_spawn(&client->runtime, dcc_client_main_task, client);
    if (st != DCC_OK) {
        atomic_store(&client->started, false);
        dcc_set_error(client, "failed to spawn client runtime task");
        goto done;
    }

done:
    dcc_client_lifecycle_admission_leave(&client->start_admission);
    return st;
}

dcc_status_t dcc_client_stop(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&client->stopping, true, memory_order_release);
    (void)dcc_gateway_request_stop(client);
    dcc_voice_client_request_stop_owned(client);
    return dcc_runtime_request_stop(&client->runtime);
}

dcc_status_t dcc_client_wait(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t admission_status = dcc_client_lifecycle_admission_enter(
        &client->wait_admission
    );
    if (admission_status != DCC_OK) {
        dcc_set_error(client, "client teardown has started");
        return admission_status;
    }
    if (!atomic_load(&client->started)) {
        dcc_client_lifecycle_admission_leave(&client->wait_admission);
        return DCC_ERR_STATE;
    }
    dcc_status_t st = dcc_runtime_run(&client->runtime);
    atomic_store(&client->started, false);
    dcc_client_lifecycle_admission_leave(&client->wait_admission);
    return st == DCC_ERR_CANCELED ? DCC_OK : st;
}
