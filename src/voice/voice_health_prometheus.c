#include <dcc/voice/lifecycle.h>

#include <stdio.h>
#include <stddef.h>

dcc_status_t dcc_voice_client_health_snapshot_prometheus(
    const dcc_voice_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (snapshot == NULL || snapshot->size < offsetof(dcc_voice_health_snapshot_t, dave) ||
        out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_voice_dave_stats_t dave = {0};
    if (snapshot->size >= offsetof(dcc_voice_health_snapshot_t, dave) + sizeof(snapshot->dave)) {
        dave = snapshot->dave;
    }
    int len = snprintf(
        out,
        out_size,
        "# TYPE dcc_voice_media_ready gauge\n"
        "dcc_voice_media_ready %u\n"
        "# TYPE dcc_voice_reconnect_attempts_total counter\n"
        "dcc_voice_reconnect_attempts_total %u\n"
        "# TYPE dcc_voice_receive_packets_total counter\n"
        "dcc_voice_receive_packets_total{result=\"lost\"} %u\n"
        "dcc_voice_receive_packets_total{result=\"dropped\"} %u\n"
        "# TYPE dcc_voice_dave_backend_available gauge\n"
        "dcc_voice_dave_backend_available %u\n"
        "# TYPE dcc_voice_dave_media_ready gauge\n"
        "dcc_voice_dave_media_ready %u\n"
        "# TYPE dcc_voice_dave_crypto_total counter\n"
        "dcc_voice_dave_crypto_total{operation=\"encrypt\",result=\"success\"} %llu\n"
        "dcc_voice_dave_crypto_total{operation=\"encrypt\",result=\"failure\"} %llu\n"
        "dcc_voice_dave_crypto_total{operation=\"decrypt\",result=\"success\"} %llu\n"
        "dcc_voice_dave_crypto_total{operation=\"decrypt\",result=\"failure\"} %llu\n"
        "dcc_voice_dave_crypto_total{operation=\"decrypt\",result=\"missing_key\"} %llu\n",
        (unsigned)(snapshot->media_ready != 0U),
        snapshot->stats.reconnect_attempts,
        snapshot->stats.receive_packets_lost,
        snapshot->stats.receive_packets_dropped,
        (unsigned)(dave.backend_available != 0U),
        (unsigned)(dave.media_ready != 0U),
        (unsigned long long)dave.encrypt_success,
        (unsigned long long)dave.encrypt_failures,
        (unsigned long long)dave.decrypt_success,
        (unsigned long long)dave.decrypt_failures,
        (unsigned long long)dave.missing_key_failures
    );
    if (len < 0) return DCC_ERR_RUNTIME;
    if (out_len != NULL) *out_len = (size_t)len;
    return (size_t)len < out_size ? DCC_OK : DCC_ERR_NOMEM;
}
