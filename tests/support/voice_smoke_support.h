#ifndef DCC_TEST_VOICE_SMOKE_SUPPORT_H
#define DCC_TEST_VOICE_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <stdatomic.h>
#include <sys/types.h>
#endif

#include <stdint.h>
#include <stddef.h>

#if !defined(_WIN32)
#define VOICE_WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#endif

typedef struct voice_smoke_state {
    dcc_client_t *client;
    dcc_voice_client_t *voice_client;
    const uint8_t *receive_audio;
    const uint8_t *combined_audio;
    const uint8_t *mixed_audio;
    size_t mixed_audio_size;
    uint32_t shard_id;
    int failed;
    int buffer_send_seen;
    int udp_buffer_send_seen;
    int ready_seen;
    int receive_seen;
    int receive_combined_seen;
    int mixed_combined_seen;
    int track_marker_seen;
    int speaking_seen;
    int disconnect_seen;
    int platform_seen;
} voice_smoke_state_t;

typedef struct voice_cancel_state {
    int first_seen;
    int first_cancelled;
    int second_seen;
} voice_cancel_state_t;

#if !defined(_WIN32)
typedef struct voice_timer_stop_state {
    dcc_client_t *client;
    dcc_voice_client_t *voice_client;
    unsigned delay_us;
} voice_timer_stop_state_t;

typedef struct voice_client_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} voice_client_wait_state_t;
#endif

typedef struct voice_decode_state {
    int seen;
    int seen_count;
    uint32_t ssrc;
    size_t payload_size;
    size_t expected_payload_size;
    uint8_t expected_first_byte;
    uint8_t seen_first_bytes[8];
} voice_decode_state_t;

#if !defined(_WIN32)
typedef struct voice_auto_jitter_state {
    voice_smoke_state_t event_state;
    voice_decode_state_t decode_state;
    const uint8_t *packet;
    size_t packet_size;
    int setup_failed;
} voice_auto_jitter_state_t;

typedef struct voice_ws_server {
    int fd;
    int udp_fd;
    unsigned short port;
    unsigned short udp_port;
    unsigned idle_max_us;
    atomic_uint idle_started;
    atomic_uint idle_release;
    int saw_identify;
    int saw_heartbeat;
    int saw_discovery;
    int saw_select_protocol;
} voice_ws_server_t;

typedef struct voice_ws_loop_server {
    int fd;
    unsigned short port;
    int accepts;
    int saw_identify;
    int saw_resume;
} voice_ws_loop_server_t;

typedef struct voice_ws_chaos_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint saw_identify;
    atomic_uint saw_resume;
    atomic_uint sent_4015;
    atomic_uint sent_4014;
    atomic_uint bad;
} voice_ws_chaos_server_t;

typedef struct voice_full_reconnect_ws_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint saw_identify;
    atomic_uint saw_resume;
} voice_full_reconnect_ws_server_t;

typedef struct voice_full_reconnect_gateway_server {
    int fd;
    unsigned short port;
    atomic_uint saw_identify;
    atomic_uint saw_full_voice_state_update;
} voice_full_reconnect_gateway_server_t;

typedef struct voice_ws_task {
    dcc_voice_client_t *voice_client;
    char url[128];
    dcc_status_t status;
} voice_ws_task_t;

typedef struct voice_ws_loop_task {
    dcc_voice_client_t *voice_client;
    char url[128];
    dcc_status_t status;
} voice_ws_loop_task_t;
#endif


typedef struct voice_encode_state {
    int seen;
    size_t sample_count;
    uint64_t duration_ms;
} voice_encode_state_t;

typedef struct voice_dave_mls_state {
    int external_seen;
    int announce_seen;
    uint16_t sequence;
    uint16_t transition_id;
    size_t payload_size;
    uint8_t first_payload_byte;
} voice_dave_mls_state_t;


#if !defined(_WIN32)
void *voice_client_wait_thread_main(void *arg);
#endif

dcc_status_t voice_dave_mls_handler(
    void *user_data,
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_opcode_t opcode,
    uint16_t sequence,
    uint16_t transition_id,
    const uint8_t *payload,
    size_t payload_size
);
dcc_status_t voice_decode_pcm(
    void *user_data,
    uint32_t ssrc,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *pcm_out,
    size_t pcm_out_cap,
    size_t *pcm_out_size
);
dcc_status_t voice_encode_pcm(
    void *user_data,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms,
    uint8_t *opus_out,
    size_t opus_out_cap,
    size_t *opus_out_size
);
void voice_cancel_first(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void voice_cancel_second(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void voice_smoke_on_event(dcc_client_t *client, const dcc_event_t *event, void *user_data);
int voice_smoke_add_listener(dcc_client_t *client, dcc_event_type_t type, voice_smoke_state_t *state);
int voice_session_state_smoke(dcc_client_t *client, dcc_voice_client_t *voice_client);
int voice_invalid_arg_smoke(
    voice_smoke_state_t *state,
    const uint8_t *secret_key,
    size_t secret_key_size,
    const uint8_t *queued_audio,
    size_t queued_audio_size,
    const int16_t *pcm_audio,
    size_t pcm_audio_count,
    const int16_t *mix_pcm_a,
    size_t mix_pcm_count,
    const uint8_t *receive_audio,
    size_t receive_audio_size,
    const char *voice_endpoint_frame,
    const char *voice_noop_frame,
    const uint8_t *voice_rtp_packet,
    size_t voice_rtp_packet_size,
    const uint8_t *voice_bad_rtp_packet,
    size_t voice_bad_rtp_packet_size
);
int voice_rtp_reorder_jitter_smoke(
    dcc_voice_client_t *voice_client,
    voice_decode_state_t *decode_state,
    dcc_voice_client_stats_t *voice_stats,
    const char *voice_endpoint_frame,
    const char *voice_speaking_frame,
    const uint8_t *voice_rtp_packet,
    size_t voice_rtp_packet_size,
    const uint8_t *receive_audio
);
int voice_dave_gateway_smoke(dcc_voice_client_t *voice_client);

#if !defined(_WIN32)
uint64_t voice_rusage_cpu_us(void);
uint64_t voice_u64_delta(uint64_t after, uint64_t before);
int voice_ws_start_server(voice_ws_server_t *server, pthread_t *thread);
void voice_ws_client_task(void *arg);
int voice_ws_loop_start_server(voice_ws_loop_server_t *server, pthread_t *thread);
int voice_ws_chaos_start_server(voice_ws_chaos_server_t *server, pthread_t *thread);
void voice_ws_loop_client_task(void *arg);
int voice_full_reconnect_ws_start_server(voice_full_reconnect_ws_server_t *server, pthread_t *thread);
int voice_full_reconnect_gateway_start_server(
    voice_full_reconnect_gateway_server_t *server,
    pthread_t *thread
);
int voice_auto_jitter_timer_smoke(const uint8_t *packet, size_t packet_size, const uint8_t *receive_audio);
int voice_udp_smoke(
    dcc_voice_client_t *voice_client,
    voice_smoke_state_t *state,
    voice_decode_state_t *decode_state,
    const uint8_t *queued_audio,
    const uint8_t *secret_key,
    size_t secret_key_size
);
int voice_websocket_smoke(void);
int voice_websocket_loop_smoke(void);
int voice_websocket_managed_loop_smoke(void);
int voice_websocket_managed_close_chaos_smoke(void);
int voice_websocket_full_reconnect_smoke(void);
#endif

#endif
