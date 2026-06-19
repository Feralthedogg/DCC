#ifndef DCC_VOICE_WEBSOCKET_STATE_INTERNAL_H
#define DCC_VOICE_WEBSOCKET_STATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdatomic.h>
#include <stdint.h>

#define DCC_VOICE_WEBSOCKET_STATE_FIELDS \
    dcc_snowflake_t websocket_self_user_id; \
    uint32_t websocket_reconnect_delay_ms; \
    atomic_bool websocket_task_running; \
    atomic_bool websocket_task_stop; \
    atomic_bool websocket_task_completed; \
    atomic_int websocket_last_status; \
    atomic_flag websocket_lock; \
    struct dcc_ws *websocket_current_ws

#endif
