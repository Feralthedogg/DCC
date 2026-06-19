#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_guild_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
) {
    switch (event_name[7]) {
        case 'C':
            if (dcc_json_event_prefix_eq(
                    event_name,
                    sizeof(out->event_name),
                    "GUILD_SCHEDULED_EVENT_",
                    sizeof("GUILD_SCHEDULED_EVENT_") - 1
                ) &&
                out->gateway.scheduled_event_id == 0) {
                out->gateway.scheduled_event_id = out->gateway.id;
                if (out->scheduled_event.id == 0) {
                    out->scheduled_event.id = out->gateway.id;
                }
            }
            return;
        case 'O':
            if (dcc_json_event_prefix_eq(
                    event_name,
                    sizeof(out->event_name),
                    "GUILD_SOUNDBOARD_SOUND_",
                    sizeof("GUILD_SOUNDBOARD_SOUND_") - 1
                ) &&
                out->gateway.sound_id == 0) {
                out->gateway.sound_id = out->gateway.id;
                if (out->soundboard_sound.id == 0) {
                    out->soundboard_sound.id = out->gateway.id;
                }
            }
            return;
        default:
            return;
    }
}
