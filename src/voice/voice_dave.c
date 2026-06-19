#include "internal/voice/dcc_voice_internal.h"

const char *dcc_voice_dave_mls_opcode_string(dcc_voice_dave_mls_opcode_t opcode) {
    switch (opcode) {
        case DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER:
            return "external_sender";
        case DCC_VOICE_DAVE_MLS_KEY_PACKAGE:
            return "key_package";
        case DCC_VOICE_DAVE_MLS_PROPOSALS:
            return "proposals";
        case DCC_VOICE_DAVE_MLS_COMMIT_WELCOME:
            return "commit_welcome";
        case DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION:
            return "announce_commit_transition";
        case DCC_VOICE_DAVE_MLS_WELCOME:
            return "welcome";
        default:
            return "unknown";
    }
}
