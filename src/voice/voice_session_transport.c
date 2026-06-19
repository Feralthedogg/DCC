#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

void dcc_voice_reset_transport_state(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }
    voice_client->ssrc = 0;
    voice_client->ssrc_map_len = 0;
    memset(voice_client->ssrc_map_ssrcs, 0, sizeof(voice_client->ssrc_map_ssrcs));
    memset(voice_client->ssrc_map_user_ids, 0, sizeof(voice_client->ssrc_map_user_ids));
    dcc_voice_client_clear_secret_key(voice_client);
    (void)dcc_voice_client_clear_receive_reorder(voice_client);
    dcc_voice_client_disconnect_udp(voice_client);
    voice_client->udp_host[0] = '\0';
    voice_client->udp_port = 0;
    voice_client->discovered_address[0] = '\0';
    voice_client->discovered_port = 0;
    voice_client->select_protocol_sent = 0;
}
