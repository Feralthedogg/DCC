#include "json_smoke_gateway_foundation_internal.h"
#include "json_smoke_support.h"

int dcc_json_smoke_gateway_foundation_members(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t *payload = dcc_json_smoke_payload();
    if (payload == NULL) {
        fprintf(stderr, "json smoke payload allocation failed\n");
        return 1;
    }

    const char scheduled_input[] =
        "{\"t\":\"GUILD_SCHEDULED_EVENT_CREATE\",\"s\":107,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"name\":\"event\","
        "\"description\":\"desc\",\"status\":2,\"user_count\":7,"
        "\"image\":\"directhash\",\"entity_metadata\":{\"location\":\"stage\"},"
        "\"creator\":{\"id\":\"446\",\"username\":\"creator\","
        "\"global_name\":\"Creator\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"creator-avatar\",\"public_flags\":32},"
        "\"scheduled_start_time\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"scheduled_end_time\":null,\"privacy_level\":2,\"entity_type\":3,"
        "\"entity_id\":null,\"creator_id\":\"446\"}}";
    st = dcc_json_parse_gateway_payload(scheduled_input, sizeof(scheduled_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.scheduled_event_id != 888 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.name == NULL ||
        strcmp(payload->gateway.name, "event") != 0 ||
        payload->gateway.start_time == NULL ||
        payload->gateway.end_time != NULL ||
        payload->gateway.privacy_level != 2 ||
        payload->gateway.entity_type != 3 ||
        payload->gateway.entity_id != 0 ||
        payload->gateway.creator_id != 446 ||
        payload->scheduled_event.id != 888 ||
        payload->scheduled_event.guild_id != 333 ||
        payload->scheduled_event.channel_id != 222 ||
        payload->scheduled_event.name == NULL ||
        strcmp(payload->scheduled_event.name, "event") != 0 ||
        payload->scheduled_event.description == NULL ||
        strcmp(payload->scheduled_event.description, "desc") != 0 ||
        payload->scheduled_event.image == NULL ||
        strcmp(payload->scheduled_event.image, "directhash") != 0 ||
        payload->scheduled_event.location == NULL ||
        strcmp(payload->scheduled_event.location, "stage") != 0 ||
        payload->scheduled_event.creator.id != 446 ||
        payload->scheduled_event.creator.username == NULL ||
        strcmp(payload->scheduled_event.creator.username, "creator") != 0 ||
        payload->scheduled_event.creator.avatar == NULL ||
        strcmp(payload->scheduled_event.creator.avatar, "creator-avatar") != 0 ||
        payload->scheduled_event.creator.flags != 32 ||
        payload->scheduled_event.status != 2 ||
        payload->scheduled_event.user_count != 7 ||
        payload->scheduled_event.start_time == NULL ||
        payload->scheduled_event.end_time != NULL ||
        payload->scheduled_event.privacy_level != 2 ||
        payload->scheduled_event.entity_type != 3 ||
        payload->scheduled_event.entity_id != 0 ||
        payload->scheduled_event.creator_id != 446) {
        fprintf(stderr, "gateway scheduled event projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char member_update_input[] =
        "{\"t\":\"GUILD_MEMBER_UPDATE\",\"s\":108,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"user\":{\"id\":\"446\",\"username\":\"member\"},\"nick\":\"nick\","
        "\"roles\":[\"555\",\"556\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\"}}";
    st = dcc_json_parse_gateway_payload(member_update_input, sizeof(member_update_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.user_id != 446 ||
        payload->gateway.nick == NULL ||
        strcmp(payload->gateway.nick, "nick") != 0 ||
        payload->gateway.role_ids == NULL ||
        payload->gateway.role_ids_count != 2 ||
        payload->gateway.role_ids[0] != 555 ||
        payload->gateway.role_ids[1] != 556 ||
        payload->gateway.joined_at == NULL ||
        payload->gateway.premium_since != NULL ||
        payload->gateway.pending != 1 ||
        payload->gateway.communication_disabled_until == NULL ||
        payload->member.guild_id != 333 ||
        payload->member.user.id != 446 ||
        payload->member.user.username == NULL ||
        strcmp(payload->member.user.username, "member") != 0 ||
        payload->member.nick == NULL ||
        strcmp(payload->member.nick, "nick") != 0 ||
        payload->member.role_ids == NULL ||
        payload->member.role_ids_count != 2 ||
        payload->member.role_ids[0] != 555 ||
        payload->member.role_ids[1] != 556 ||
        payload->member.joined_at == NULL ||
        payload->member.premium_since != NULL ||
        payload->member.pending != 1 ||
        payload->member.communication_disabled_until == NULL) {
        fprintf(stderr, "gateway member update projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char members_chunk_input[] =
        "{\"t\":\"GUILD_MEMBERS_CHUNK\",\"s\":109,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"chunk_index\":1,\"chunk_count\":2,"
        "\"members\":[{\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"global_name\":\"Member\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"member-user-avatar\",\"public_flags\":64},"
        "\"nick\":\"nick\",\"roles\":[\"555\",\"556\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"flags\":2}],"
        "\"not_found\":[\"447\"],\"nonce\":\"n1\"}}";
    st = dcc_json_parse_gateway_payload(members_chunk_input, sizeof(members_chunk_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.chunk_index != 1 ||
        payload->gateway.chunk_count != 2 ||
        payload->gateway.member_ids == NULL ||
        payload->gateway.member_ids_count != 1 ||
        payload->gateway.member_ids[0] != 446 ||
        payload->gateway.members == NULL ||
        payload->gateway.members_count != 1 ||
        payload->gateway.members[0].guild_id != 333 ||
        payload->gateway.members[0].user.id != 446 ||
        payload->gateway.members[0].user.username == NULL ||
        strcmp(payload->gateway.members[0].user.username, "member") != 0 ||
        payload->gateway.members[0].user.avatar == NULL ||
        strcmp(payload->gateway.members[0].user.avatar, "member-user-avatar") != 0 ||
        payload->gateway.members[0].user.flags != 64 ||
        payload->gateway.members[0].role_ids == NULL ||
        payload->gateway.members[0].role_ids_count != 2 ||
        payload->gateway.members[0].role_ids[1] != 556 ||
        payload->gateway.members[0].nick == NULL ||
        strcmp(payload->gateway.members[0].nick, "nick") != 0 ||
        payload->gateway.members[0].pending != 1 ||
        payload->gateway.not_found_ids == NULL ||
        payload->gateway.not_found_ids_count != 1 ||
        payload->gateway.not_found_ids[0] != 447 ||
        payload->gateway.nonce == NULL ||
        strcmp(payload->gateway.nonce, "n1") != 0 ||
        payload->guild_members_chunk.guild_id != 333 ||
        payload->guild_members_chunk.chunk_index != 1 ||
        payload->guild_members_chunk.chunk_count != 2 ||
        payload->guild_members_chunk.member_ids == NULL ||
        payload->guild_members_chunk.member_ids_count != 1 ||
        payload->guild_members_chunk.member_ids[0] != 446 ||
        payload->guild_members_chunk.members == NULL ||
        payload->guild_members_chunk.members_count != 1 ||
        payload->guild_members_chunk.members[0].user.username == NULL ||
        strcmp(payload->guild_members_chunk.members[0].user.username, "member") != 0 ||
        payload->guild_members_chunk.members[0].user.avatar == NULL ||
        strcmp(payload->guild_members_chunk.members[0].user.avatar, "member-user-avatar") != 0 ||
        payload->guild_members_chunk.members[0].user.flags != 64 ||
        payload->guild_members_chunk.members[0].role_ids == NULL ||
        payload->guild_members_chunk.members[0].role_ids_count != 2 ||
        payload->guild_members_chunk.members[0].pending != 1 ||
        payload->guild_members_chunk.not_found_ids == NULL ||
        payload->guild_members_chunk.not_found_ids_count != 1 ||
        payload->guild_members_chunk.not_found_ids[0] != 447 ||
        payload->guild_members_chunk.nonce == NULL ||
        strcmp(payload->guild_members_chunk.nonce, "n1") != 0) {
        fprintf(stderr, "gateway members chunk projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char thread_members_input[] =
        "{\"t\":\"THREAD_MEMBERS_UPDATE\",\"s\":110,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"guild_id\":\"333\",\"member_count\":2,"
        "\"added_members\":[{\"id\":\"888\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"join_timestamp\":\"2026-06-15T00:00:00.000000+00:00\",\"flags\":1}],"
        "\"removed_member_ids\":[\"444\",\"445\"]}}";
    st = dcc_json_parse_gateway_payload(thread_members_input, sizeof(thread_members_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.id != 888 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.count != 2 ||
        payload->gateway.member_ids == NULL ||
        payload->gateway.member_ids_count != 1 ||
        payload->gateway.member_ids[0] != 446 ||
        payload->gateway.added_members == NULL ||
        payload->gateway.added_members_count != 1 ||
        payload->gateway.added_members[0].id != 888 ||
        payload->gateway.added_members[0].user_id != 446 ||
        payload->gateway.added_members[0].guild_id != 333 ||
        payload->gateway.added_members[0].flags != 1 ||
        payload->gateway.added_members[0].join_timestamp == NULL ||
        strcmp(payload->gateway.added_members[0].join_timestamp, "2026-06-15T00:00:00.000000+00:00") != 0 ||
        payload->thread_members_update.id != 888 ||
        payload->thread_members_update.guild_id != 333 ||
        payload->thread_members_update.count != 2 ||
        payload->thread_members_update.added_members == NULL ||
        payload->thread_members_update.added_members_count != 1 ||
        payload->thread_members_update.added_members[0].user_id != 446 ||
        payload->thread_members_update.added_members[0].join_timestamp == NULL ||
        strcmp(
            payload->thread_members_update.added_members[0].join_timestamp,
            "2026-06-15T00:00:00.000000+00:00"
        ) != 0 ||
        payload->thread_members_update.member_ids == NULL ||
        payload->thread_members_update.member_ids_count != 1 ||
        payload->thread_members_update.member_ids[0] != 446 ||
        payload->thread_members_update.removed_member_ids == NULL ||
        payload->thread_members_update.removed_member_ids_count != 2 ||
        payload->thread_members_update.removed_member_ids[1] != 445) {
        fprintf(stderr, "gateway thread members update projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char typing_input[] =
        "{\"t\":\"TYPING_START\",\"s\":110,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"channel_id\":\"222\",\"user_id\":\"444\",\"timestamp\":123456,"
        "\"member\":{\"user\":{\"id\":\"444\",\"username\":\"typer\","
        "\"global_name\":null,\"discriminator\":\"0\",\"bot\":false},"
        "\"nick\":\"tap\",\"roles\":[\"555\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"flags\":2}}}";
    st = dcc_json_parse_gateway_payload(typing_input, sizeof(typing_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.user_id != 444 ||
        payload->gateway.user.id != 444 ||
        payload->gateway.user.username == NULL ||
        strcmp(payload->gateway.user.username, "typer") != 0 ||
        payload->gateway.timestamp != 123456 ||
        payload->gateway.nick == NULL ||
        strcmp(payload->gateway.nick, "tap") != 0 ||
        payload->gateway.role_ids == NULL ||
        payload->gateway.role_ids_count != 1 ||
        payload->gateway.role_ids[0] != 555 ||
        payload->gateway.joined_at == NULL ||
        payload->gateway.premium_since != NULL ||
        payload->gateway.pending != 1 ||
        payload->gateway.communication_disabled_until == NULL ||
        payload->gateway.flags != 2 ||
        payload->member.guild_id != 333 ||
        payload->member.user.id != 444 ||
        payload->member.user.username == NULL ||
        strcmp(payload->member.user.username, "typer") != 0 ||
        payload->member.nick == NULL ||
        strcmp(payload->member.nick, "tap") != 0 ||
        payload->member.role_ids == NULL ||
        payload->member.role_ids_count != 1 ||
        payload->member.role_ids[0] != 555 ||
        payload->member.flags != 2) {
        fprintf(stderr, "gateway typing member projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    return 0;
}
