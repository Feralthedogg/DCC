#include "internal/voice/dcc_voice_internal.h"

const char *dcc_voice_health_string(dcc_voice_health_t health) {
    switch (health) {
        case DCC_VOICE_HEALTH_DISCONNECTED:
            return "disconnected";
        case DCC_VOICE_HEALTH_CONNECTING:
            return "connecting";
        case DCC_VOICE_HEALTH_READY:
            return "ready";
        case DCC_VOICE_HEALTH_ACTIVE:
            return "active";
        case DCC_VOICE_HEALTH_RECONNECTING:
            return "reconnecting";
        case DCC_VOICE_HEALTH_DEGRADED:
            return "degraded";
        case DCC_VOICE_HEALTH_TERMINATED:
            return "terminated";
        case DCC_VOICE_HEALTH_UNKNOWN:
        default:
            return "unknown";
    }
}
