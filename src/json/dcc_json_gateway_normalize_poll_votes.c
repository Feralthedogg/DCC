#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_poll_vote(dcc_json_gateway_payload_t *out) {
    if (out->poll_vote.user_id == 0 && out->gateway.user_id != 0) {
        out->poll_vote.user_id = out->gateway.user_id;
    }
    if (out->poll_vote.channel_id == 0 && out->gateway.channel_id != 0) {
        out->poll_vote.channel_id = out->gateway.channel_id;
    }
    if (out->poll_vote.message_id == 0 && out->gateway.message_id != 0) {
        out->poll_vote.message_id = out->gateway.message_id;
    }
    if (out->poll_vote.guild_id == 0 && out->gateway.guild_id != 0) {
        out->poll_vote.guild_id = out->gateway.guild_id;
    }
}
