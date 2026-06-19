#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_thread_member_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated,
    int added_members
) {
    out->gateway.member_ids = out->gateway_member_ids;
    out->gateway.member_ids_count = stored;
    out->gateway.member_ids_truncated = truncated;
    if (added_members) {
        out->gateway.added_members = out->gateway_thread_members;
        out->gateway.added_members_count = stored;
        out->gateway.added_members_truncated = truncated;
        out->thread_members_update.added_members = out->gateway.added_members;
        out->thread_members_update.added_members_count = out->gateway.added_members_count;
        out->thread_members_update.added_members_truncated = out->gateway.added_members_truncated;
        out->thread_members_update.member_ids = out->gateway.member_ids;
        out->thread_members_update.member_ids_count = out->gateway.member_ids_count;
        out->thread_members_update.member_ids_truncated = out->gateway.member_ids_truncated;
        out->has_thread_members_update = 1;
    } else {
        out->thread_members_update.member_ids = out->gateway.member_ids;
        out->thread_members_update.member_ids_count = out->gateway.member_ids_count;
        out->thread_members_update.member_ids_truncated = out->gateway.member_ids_truncated;
        out->guild_members_chunk.member_ids = out->gateway.member_ids;
        out->guild_members_chunk.member_ids_count = out->gateway.member_ids_count;
        out->guild_members_chunk.member_ids_truncated = out->gateway.member_ids_truncated;
        out->thread_list_sync.member_ids = out->gateway.member_ids;
        out->thread_list_sync.member_ids_count = out->gateway.member_ids_count;
        out->thread_list_sync.member_ids_truncated = out->gateway.member_ids_truncated;
        out->thread_list_sync.members = out->gateway_thread_members;
        out->thread_list_sync.members_count = stored;
        out->thread_list_sync.members_truncated = truncated;
        out->has_thread_members_update = 1;
        out->has_guild_members_chunk = 1;
        out->has_thread_list_sync = 1;
    }
}
