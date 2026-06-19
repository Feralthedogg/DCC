#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_guild_member_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.member_ids = out->gateway_member_ids;
    out->gateway.member_ids_count = stored;
    out->gateway.member_ids_truncated = truncated;
    out->gateway.members = out->gateway_members;
    out->gateway.members_count = stored;
    out->gateway.members_truncated = truncated;
    out->guild_members_chunk.member_ids = out->gateway.member_ids;
    out->guild_members_chunk.member_ids_count = out->gateway.member_ids_count;
    out->guild_members_chunk.member_ids_truncated = out->gateway.member_ids_truncated;
    out->guild_members_chunk.members = out->gateway.members;
    out->guild_members_chunk.members_count = out->gateway.members_count;
    out->guild_members_chunk.members_truncated = out->gateway.members_truncated;
    out->has_guild_members_chunk = 1;
}
