#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_gateway_handle_ready(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    const dcc_json_t *ssrc_json = dcc_json_object_get(d, "ssrc");
    uint32_t ssrc = 0;
    if (ssrc_json != NULL && dcc_voice_gateway_json_u32(ssrc_json, &ssrc) != 0) {
        return DCC_ERR_JSON;
    }
    voice_client->ssrc = ssrc;

    const dcc_json_t *ip_json = dcc_json_object_get(d, "ip");
    const dcc_json_t *port_json = dcc_json_object_get(d, "port");
    if (ip_json != NULL || port_json != NULL) {
        const char *ip = dcc_json_string(ip_json);
        uint16_t port = 0;
        if (ip == NULL ||
            ip[0] == '\0' ||
            dcc_voice_gateway_json_u16(port_json, &port) != 0 ||
            port == 0) {
            return DCC_ERR_JSON;
        }
        dcc_status_t status = dcc_voice_copy_text(
            voice_client->udp_host,
            sizeof(voice_client->udp_host),
            ip
        );
        if (status != DCC_OK) {
            return status;
        }
        voice_client->udp_port = port;
        voice_client->discovered_address[0] = '\0';
        voice_client->discovered_port = 0;
        voice_client->select_protocol_sent = 0;
    }
    dcc_voice_gateway_select_mode_from_ready(voice_client, dcc_json_object_get(d, "modes"));
    return DCC_OK;
}

dcc_status_t dcc_voice_gateway_handle_session_description(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    const dcc_json_t *secret_key = dcc_json_object_get(d, "secret_key");
    if (secret_key != NULL) {
        dcc_status_t status = dcc_voice_gateway_parse_secret_key(voice_client, secret_key);
        if (status != DCC_OK) {
            return status;
        }
        voice_client->packet_nonce = 1U;
    }

    const dcc_json_t *dave_protocol_json = dcc_json_object_get(d, "dave_protocol_version");
    if (voice_client->dave_version != DCC_VOICE_DAVE_NONE) {
        uint32_t dave_protocol_version = 0;
        if (dcc_voice_gateway_json_u32(dave_protocol_json, &dave_protocol_version) != 0 ||
            dcc_voice_gateway_dave_version_from_u32(dave_protocol_version) != voice_client->dave_version) {
            voice_client->dave_enabled = 0;
            voice_client->dave_version = DCC_VOICE_DAVE_NONE;
            voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
            voice_client->dave_transition_pending = 0;
            voice_client->dave_transition_ready = 0;
            voice_client->dave_transition_id = 0;
        }
    }
    return dcc_voice_client_notify_ready(voice_client, voice_client->channel_id);
}

dcc_status_t dcc_voice_gateway_handle_hello(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    const dcc_json_t *heartbeat_json = dcc_json_object_get(d, "heartbeat_interval");
    uint32_t heartbeat_interval = 0;
    if (heartbeat_json != NULL && dcc_voice_gateway_json_u32(heartbeat_json, &heartbeat_interval) != 0) {
        return DCC_ERR_JSON;
    }
    voice_client->heartbeat_interval_ms = heartbeat_interval;
    return DCC_OK;
}
