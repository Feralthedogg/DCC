#include <dcc/dcc.h>

#include <stdio.h>
#include <stdlib.h>

static void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL) {
        printf("DCC ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
    }
    (void)dcc_client_stop(client);
}

int main(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = getenv("BOT_TOKEN"),
        .intents = DCC_INTENT_GUILDS,
        .shard_id = 0,
        .shard_count = 1,
    };

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    (void)dcc_client_on(client, DCC_EVENT_LOG, on_log, NULL, NULL);
    (void)dcc_client_on(client, DCC_EVENT_READY, on_ready, NULL, NULL);

    st = dcc_client_start(client);
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    }

    if (st != DCC_OK) {
        fprintf(stderr, "run failed: %s: %s\n", dcc_status_string(st), dcc_client_last_error(client));
        dcc_client_destroy(client);
        return 1;
    }

    dcc_runtime_stats_t stats = {
        .size = sizeof(stats),
    };
    if (dcc_client_runtime_stats(client, &stats) == DCC_OK) {
        printf("tasks=%llu events=%llu\n",
               (unsigned long long)stats.tasks_spawned,
               (unsigned long long)stats.events_dispatched);
    }

    dcc_client_destroy(client);
    return 0;
}

