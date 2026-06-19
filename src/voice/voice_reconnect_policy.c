#include "internal/voice/dcc_voice_internal.h"

dcc_voice_reconnect_action_t dcc_voice_reconnect_action_for_close_code(uint16_t close_code) {
    switch (close_code) {
        case 0U:
        case 1000U:
            return DCC_VOICE_RECONNECT_ACTION_NONE;
        case 4001U:
        case 4002U:
        case 4003U:
        case 4004U:
        case 4005U:
        case 4011U:
        case 4012U:
        case 4014U:
        case 4016U:
        case 4017U:
        case 4020U:
        case 4021U:
        case 4022U:
            return DCC_VOICE_RECONNECT_ACTION_TERMINATE;
        default:
            return DCC_VOICE_RECONNECT_ACTION_RESUME;
    }
}

const char *dcc_voice_reconnect_action_string(dcc_voice_reconnect_action_t action) {
    switch (action) {
        case DCC_VOICE_RECONNECT_ACTION_NONE:
            return "none";
        case DCC_VOICE_RECONNECT_ACTION_RESUME:
            return "resume";
        case DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT:
            return "full_reconnect";
        case DCC_VOICE_RECONNECT_ACTION_TERMINATE:
            return "terminate";
        default:
            return "unknown";
    }
}
