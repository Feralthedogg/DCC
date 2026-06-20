#include "internal/json/dcc_json.h"

#include <dcc/dcc.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct dcc_json_bench_sample {
    const char *name;
    const char *json;
    size_t len;
} dcc_json_bench_sample_t;

typedef dcc_status_t (*dcc_json_bench_parse_fn)(
    const char *input,
    size_t len,
    dcc_json_gateway_payload_t *out
);

typedef dcc_status_t (*dcc_json_bench_validate_fn)(
    const char *input,
    size_t len
);

static volatile uint64_t dcc_json_bench_sink;

static double dcc_json_bench_elapsed(clock_t start, clock_t end);

static dcc_status_t dcc_json_bench_validate_stage1(const char *input, size_t len) {
    return dcc_json_stage1_validate(input, len);
}

static dcc_status_t dcc_json_bench_parse_direct(
    const char *input,
    size_t len,
    dcc_json_gateway_payload_t *out
) {
    return dcc_json_parse_gateway_payload(input, len, out);
}

static dcc_status_t dcc_json_bench_parse_stage1_direct(
    const char *input,
    size_t len,
    dcc_json_gateway_payload_t *out
) {
    dcc_status_t status = dcc_json_stage1_validate(input, len);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_json_parse_gateway_payload(input, len, out);
}

static int dcc_json_bench_run_validate(
    const char *label,
    dcc_json_bench_validate_fn validate,
    const dcc_json_bench_sample_t *sample,
    unsigned long long iterations
) {
    for (unsigned i = 0; i < 128U; ++i) {
        dcc_status_t status = validate(sample->json, sample->len);
        if (status != DCC_OK) {
            fprintf(stderr, "%s warmup failed for %s: %s\n", label, sample->name, dcc_status_string(status));
            return 1;
        }
        dcc_json_bench_sink += sample->len;
    }

    clock_t start = clock();
    for (unsigned long long i = 0; i < iterations; ++i) {
        dcc_status_t status = validate(sample->json, sample->len);
        if (status != DCC_OK) {
            fprintf(stderr, "%s failed for %s: %s\n", label, sample->name, dcc_status_string(status));
            return 1;
        }
        dcc_json_bench_sink += sample->len;
    }
    clock_t end = clock();

    double seconds = dcc_json_bench_elapsed(start, end);
    double ns_per_op = seconds > 0.0 ? seconds * 1000000000.0 / (double)iterations : 0.0;
    double mib = ((double)sample->len * (double)iterations) / (1024.0 * 1024.0);
    double mib_per_sec = seconds > 0.0 ? mib / seconds : 0.0;

    printf(
        "%-18s %-22s %12.2f ns/op %10.2f MiB/s\n",
        sample->name,
        label,
        ns_per_op,
        mib_per_sec
    );
    return 0;
}

static unsigned long long dcc_json_bench_iterations(void) {
    const char *env = getenv("DCC_JSON_BENCH_ITERS");
    if (env == NULL || *env == '\0') {
        return 20000ULL;
    }

    char *end = NULL;
    unsigned long long value = strtoull(env, &end, 10);
    if (end == env || value == 0ULL) {
        return 20000ULL;
    }
    if (value > 100000000ULL) {
        return 100000000ULL;
    }
    return value;
}

static void dcc_json_bench_consume(const dcc_json_gateway_payload_t *payload) {
    uint64_t value = (uint64_t)payload->has_op + (uint64_t)payload->has_seq;
    value += (uint64_t)payload->op + payload->seq;
    value += (uint64_t)payload->has_event_name;
    value += (uint64_t)(unsigned char)payload->event_name[0];
    if (payload->has_message) {
        value += payload->message.id + payload->message.channel_id + payload->message.guild_id;
        value += payload->message.webhook_id + payload->message.type;
        value += payload->message.tts + payload->message.mention_everyone + payload->message.pinned;
        if (payload->message.timestamp != NULL) {
            value += (uint64_t)(unsigned char)payload->message.timestamp[0];
        }
        if (payload->message.edited_timestamp != NULL) {
            value += (uint64_t)(unsigned char)payload->message.edited_timestamp[0];
        }
    }
    if (payload->has_guild) {
        value += payload->guild.id + payload->guild.member_count;
    }
    if (payload->has_heartbeat_interval) {
        value += (uint64_t)payload->heartbeat_interval;
    }
    dcc_json_bench_sink += value;
}

static double dcc_json_bench_elapsed(clock_t start, clock_t end) {
    if (end <= start) {
        return 0.0;
    }
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

static int dcc_json_bench_run(
    const char *label,
    dcc_json_bench_parse_fn parse,
    const dcc_json_bench_sample_t *sample,
    unsigned long long iterations
) {
    static _Thread_local dcc_json_gateway_payload_t payload;

    for (unsigned i = 0; i < 128U; ++i) {
        dcc_status_t status = parse(sample->json, sample->len, &payload);
        if (status != DCC_OK) {
            fprintf(stderr, "%s warmup failed for %s: %s\n", label, sample->name, dcc_status_string(status));
            return 1;
        }
        dcc_json_bench_consume(&payload);
    }

    clock_t start = clock();
    for (unsigned long long i = 0; i < iterations; ++i) {
        dcc_status_t status = parse(sample->json, sample->len, &payload);
        if (status != DCC_OK) {
            fprintf(stderr, "%s failed for %s: %s\n", label, sample->name, dcc_status_string(status));
            return 1;
        }
        dcc_json_bench_consume(&payload);
    }
    clock_t end = clock();

    double seconds = dcc_json_bench_elapsed(start, end);
    double ns_per_op = seconds > 0.0 ? seconds * 1000000000.0 / (double)iterations : 0.0;
    double mib = ((double)sample->len * (double)iterations) / (1024.0 * 1024.0);
    double mib_per_sec = seconds > 0.0 ? mib / seconds : 0.0;

    printf(
        "%-18s %-22s %12.2f ns/op %10.2f MiB/s\n",
        sample->name,
        label,
        ns_per_op,
        mib_per_sec
    );
    return 0;
}

int main(void) {
    dcc_json_bench_sample_t samples[] = {
        {
            "hello",
            "{\"op\":10,\"s\":42,\"t\":null,\"d\":{\"heartbeat_interval\":45000}}",
            0
        },
        {
            "message_create",
            "{\"op\":0,\"s\":43,\"t\":\"MESSAGE_CREATE\",\"d\":{\"id\":\"111111111111111111\","
            "\"channel_id\":\"222222222222222222\",\"guild_id\":\"333333333333333333\","
            "\"timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
            "\"edited_timestamp\":null,\"tts\":false,\"mention_everyone\":false,"
            "\"pinned\":false,\"type\":0,\"webhook_id\":\"999999999999999999\","
            "\"content\":\"hello from a benchmark payload with enough bytes to touch the string scanner\","
            "\"author\":{\"id\":\"444444444444444444\",\"username\":\"bench-user\","
            "\"global_name\":\"Bench User\",\"discriminator\":\"0000\",\"bot\":false}}}",
            0
        },
        {
            "guild_create",
            "{\"op\":0,\"s\":44,\"t\":\"GUILD_CREATE\",\"d\":{\"id\":\"555555555555555555\","
            "\"name\":\"bench guild\",\"unavailable\":false,\"large\":false,\"member_count\":2,"
            "\"roles\":[{\"id\":\"666666666666666666\",\"name\":\"admin\"},"
            "{\"id\":\"777777777777777777\",\"name\":\"member\"}],"
            "\"channels\":[{\"id\":\"888888888888888888\",\"guild_id\":\"555555555555555555\","
            "\"type\":0,\"name\":\"general\",\"topic\":\"gateway parser benchmark\"}]}}",
            0
        }
    };
    size_t sample_count = sizeof(samples) / sizeof(samples[0]);
    for (size_t i = 0; i < sample_count; ++i) {
        samples[i].len = strlen(samples[i].json);
    }

    unsigned long long iterations = dcc_json_bench_iterations();
    printf("iterations: %llu\n", iterations);
    printf("%-18s %-22s %20s %16s\n", "sample", "path", "latency", "throughput");

    for (size_t i = 0; i < sample_count; ++i) {
        if (dcc_json_bench_run_validate("stage1-validate", dcc_json_bench_validate_stage1, &samples[i], iterations) != 0) {
            return 1;
        }
        if (dcc_json_bench_run("gateway-fast", dcc_json_bench_parse_direct, &samples[i], iterations) != 0) {
            return 1;
        }
        if (dcc_json_bench_run("stage1+gateway-fast", dcc_json_bench_parse_stage1_direct, &samples[i], iterations) != 0) {
            return 1;
        }
    }

    printf("sink: %llu\n", (unsigned long long)dcc_json_bench_sink);
    return 0;
}
