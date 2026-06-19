#ifndef DCC_VOICE_TYPES_ENUMS_H
#define DCC_VOICE_TYPES_ENUMS_H

#include <dcc/voice/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_voice_client_platform {
    DCC_VOICE_CLIENT_PLATFORM_DESKTOP = 0,
    DCC_VOICE_CLIENT_PLATFORM_MOBILE = 1
} dcc_voice_client_platform_t;

typedef enum dcc_voice_encryption_mode {
    DCC_VOICE_ENCRYPTION_NONE = 0,
    DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE = 1
} dcc_voice_encryption_mode_t;

typedef enum dcc_voice_dave_version {
    DCC_VOICE_DAVE_NONE = 0,
    DCC_VOICE_DAVE_VERSION_1 = 1
} dcc_voice_dave_version_t;

typedef enum dcc_voice_dave_mls_opcode {
    DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER = 25,
    DCC_VOICE_DAVE_MLS_KEY_PACKAGE = 26,
    DCC_VOICE_DAVE_MLS_PROPOSALS = 27,
    DCC_VOICE_DAVE_MLS_COMMIT_WELCOME = 28,
    DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION = 29,
    DCC_VOICE_DAVE_MLS_WELCOME = 30
} dcc_voice_dave_mls_opcode_t;

typedef enum dcc_voice_session_state {
    DCC_VOICE_SESSION_DISCONNECTED = 0,
    DCC_VOICE_SESSION_REQUESTED = 1,
    DCC_VOICE_SESSION_READY = 2,
    DCC_VOICE_SESSION_ACTIVE = 3,
    DCC_VOICE_SESSION_RECONNECTING = 4,
    DCC_VOICE_SESSION_TERMINATED = 5
} dcc_voice_session_state_t;

typedef enum dcc_voice_reconnect_action {
    DCC_VOICE_RECONNECT_ACTION_NONE = 0,
    DCC_VOICE_RECONNECT_ACTION_RESUME = 1,
    DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT = 2,
    DCC_VOICE_RECONNECT_ACTION_TERMINATE = 3
} dcc_voice_reconnect_action_t;

typedef enum dcc_voice_health {
    DCC_VOICE_HEALTH_UNKNOWN = 0,
    DCC_VOICE_HEALTH_DISCONNECTED,
    DCC_VOICE_HEALTH_CONNECTING,
    DCC_VOICE_HEALTH_READY,
    DCC_VOICE_HEALTH_ACTIVE,
    DCC_VOICE_HEALTH_RECONNECTING,
    DCC_VOICE_HEALTH_DEGRADED,
    DCC_VOICE_HEALTH_TERMINATED
} dcc_voice_health_t;

typedef enum dcc_voice_opus_application {
    DCC_VOICE_OPUS_APPLICATION_VOIP = 2048,
    DCC_VOICE_OPUS_APPLICATION_AUDIO = 2049,
    DCC_VOICE_OPUS_APPLICATION_RESTRICTED_LOWDELAY = 2051
} dcc_voice_opus_application_t;

#ifdef __cplusplus
}
#endif

#endif
