#ifndef DCC_VOICE_SUPERVISOR_H
#define DCC_VOICE_SUPERVISOR_H

#include <dcc/events/wait/types.h>
#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_connect_wait_options {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    uint8_t self_mute;
    uint8_t self_deaf;
    uint8_t enable_dave;
    uint32_t descriptor_timeout_ms;
    uint32_t ready_timeout_ms;
} dcc_voice_connect_wait_options_t;

typedef struct dcc_voice_connect_wait_result {
    size_t size;
    dcc_status_t connect_status;
    dcc_status_t descriptor_status;
    dcc_status_t ready_status;
    dcc_event_snapshot_t descriptor_snapshots[2];
    size_t descriptor_completed;
    dcc_voice_health_snapshot_t health;
} dcc_voice_connect_wait_result_t;

DCC_API void dcc_voice_connect_wait_options_init(
    dcc_voice_connect_wait_options_t *options
);
DCC_API dcc_status_t dcc_voice_client_connect_and_wait_until_ready(
    dcc_voice_client_t *voice_client,
    const dcc_voice_connect_wait_options_t *options,
    dcc_voice_connect_wait_result_t *out
);

#ifdef __cplusplus
}
#endif

#endif
