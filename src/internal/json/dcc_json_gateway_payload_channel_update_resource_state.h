#ifndef DCC_JSON_GATEWAY_PAYLOAD_CHANNEL_UPDATE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_CHANNEL_UPDATE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_CHANNEL_UPDATE_RESOURCE_FIELDS \
    int has_channel_pins_update; \
    dcc_channel_pins_update_t channel_pins_update; \
    int has_voice_server_update; \
    dcc_voice_server_update_t voice_server_update

#endif
