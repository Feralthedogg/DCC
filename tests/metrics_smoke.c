#include <dcc/dcc.h>

#include <stdio.h>
#include <string.h>

int main(void) {
    char out[4096];
    size_t out_len = 0U;
    dcc_cluster_health_summary_t cluster = {
        .size = sizeof(cluster),
        .total_shards = 2U,
        .ready_shards = 2U,
        .starts = 3U,
    };
    if (dcc_cluster_health_summary_prometheus(&cluster, out, sizeof(out), &out_len) != DCC_OK ||
        out_len == 0U ||
        strstr(out, "dcc_cluster_shards{state=\"ready\"} 2\n") == NULL) {
        fprintf(stderr, "cluster metrics mismatch\n");
        return 1;
    }
    dcc_cluster_identify_stats_t identify = {
        .size = sizeof(identify),
        .waits = 4U,
        .reservations = 5U,
        .remaining = 995U,
        .total = 1000U,
        .max_concurrency = 2U,
    };
    if (dcc_cluster_identify_stats_prometheus(&identify, out, sizeof(out), &out_len) != DCC_OK ||
        strstr(out, "dcc_cluster_identify_remaining 995\n") == NULL ||
        strstr(out, "dcc_cluster_identify_limit{kind=\"max_concurrency\"} 2\n") == NULL) {
        fprintf(stderr, "cluster identify metrics mismatch\n");
        return 1;
    }

    dcc_interaction_server_health_snapshot_t interaction = {
        .size = sizeof(interaction),
        .ready = 1U,
        .accepting = 1U,
        .stats = {
            .active_requests = 2U,
            .response_2xx = 7U,
        },
        .protection = {
            .replayed_requests = 3U,
            .deadline_exceeded_requests = 1U,
        },
    };
    if (dcc_interaction_server_health_snapshot_prometheus(
            &interaction,
            out,
            sizeof(out),
            &out_len
        ) != DCC_OK ||
        strstr(out, "dcc_interaction_server_ready 1\n") == NULL ||
        strstr(out, "dcc_interaction_server_responses_total{class=\"2xx\"} 7\n") == NULL ||
        strstr(out, "dcc_interaction_server_replayed_total 3\n") == NULL) {
        fprintf(stderr, "interaction metrics mismatch\n");
        return 1;
    }

    dcc_voice_health_snapshot_t voice = {
        .size = sizeof(voice),
        .media_ready = 1U,
        .dave = {
            .backend_available = 1U,
            .media_ready = 1U,
            .encrypt_success = 9U,
            .decrypt_failures = 2U,
        },
    };
    if (dcc_voice_client_health_snapshot_prometheus(&voice, out, sizeof(out), &out_len) != DCC_OK ||
        strstr(out, "dcc_voice_media_ready 1\n") == NULL ||
        strstr(out, "dcc_voice_dave_backend_available 1\n") == NULL ||
        strstr(
            out,
            "dcc_voice_dave_crypto_total{operation=\"encrypt\",result=\"success\"} 9\n"
        ) == NULL) {
        fprintf(stderr, "voice metrics mismatch\n");
        return 1;
    }

    char small[8];
    if (dcc_cluster_health_summary_prometheus(&cluster, small, sizeof(small), &out_len) != DCC_ERR_NOMEM ||
        out_len < sizeof(small) ||
        dcc_cluster_health_summary_prometheus(NULL, out, sizeof(out), NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot_prometheus(NULL, out, sizeof(out), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot_prometheus(NULL, out, sizeof(out), NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "metrics error contract mismatch\n");
        return 1;
    }
    return 0;
}
