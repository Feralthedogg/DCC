#include <dcc/sugar.h>

#include <stdint.h>
#include <stdio.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif

static uint64_t pingpong_now_ms(void) {
#if defined(_WIN32)
    FILETIME file_time;
    ULARGE_INTEGER ticks;
    GetSystemTimeAsFileTime(&file_time);
    ticks.LowPart = file_time.dwLowDateTime;
    ticks.HighPart = file_time.dwHighDateTime;
    return (ticks.QuadPart - 116444736000000000ULL) / 10000ULL;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL;
#endif
}

static long long pingpong_latency_ms(dcc_snowflake_t interaction_id) {
    uint64_t created_ms = 0U;
    uint64_t current_ms = 0U;
    if (dcc_snowflake_created_at_ms(interaction_id, &created_ms) != DCC_OK) {
        return 0;
    }
    current_ms = pingpong_now_ms();
    if (current_ms < created_ms) {
        return 0;
    }
    return (long long)(current_ms - created_ms);
}

DCC_SLASH_FN(on_ping) {
    (void)user_data;

    char footer_text[64];
    snprintf(
        footer_text,
        sizeof(footer_text),
        "%lld ms",
        pingpong_latency_ms(DCC_CTX_INTERACTION_ID(ctx))
    );

    dcc_embed_builder_t embed =
        DCC_EMBED_COLOR_FOOTER("🏓", "퐁!", 0xFF0000U, footer_text, NULL);
    (void)DCC_CTX_REPLY(ctx, DCC_MESSAGE_EMBEDS(embed));
}

DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("PingPong Bot ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
}

DCC_SIMPLE_BOT_MAIN(
    "pingpong",
    DCC_LISTEN_SLASH("핑", "퐁으로 응답합니다", on_ping),
    DCC_LISTEN_READY(on_ready)
)
