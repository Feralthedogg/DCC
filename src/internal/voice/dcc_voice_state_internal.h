#ifndef DCC_VOICE_STATE_INTERNAL_H
#define DCC_VOICE_STATE_INTERNAL_H

#include "internal/voice/dcc_voice_codec_state_internal.h"
#include "internal/voice/dcc_voice_dave_fields_internal.h"
#include "internal/voice/dcc_voice_owner_registry_internal.h"
#include "internal/voice/dcc_voice_receive_state_internal.h"
#include "internal/voice/dcc_voice_send_queue_state_internal.h"
#include "internal/voice/dcc_voice_session_fields_internal.h"
#include "internal/voice/dcc_voice_udp_state_internal.h"
#include "internal/voice/dcc_voice_websocket_state_internal.h"

#include <dcc/dcc.h>

struct dcc_voice_client {
    DCC_VOICE_SESSION_STATE_FIELDS;
    DCC_VOICE_DAVE_STATE_FIELDS;
    DCC_VOICE_SESSION_DESCRIPTOR_STATE_FIELDS;
    DCC_VOICE_UDP_STATE_FIELDS;
    DCC_VOICE_RECEIVE_STATE_FIELDS;
    DCC_VOICE_WEBSOCKET_STATE_FIELDS;
    DCC_VOICE_CODEC_STATE_FIELDS;
    DCC_VOICE_SEND_QUEUE_STATE_FIELDS;
};

#endif
