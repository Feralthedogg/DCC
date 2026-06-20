#include "json_smoke_gateway_foundation_internal.h"
#include "json_smoke_support.h"

int dcc_json_smoke_gateway_foundation(void) {
    dcc_status_t st = DCC_OK;

    const char gateway_input[] =
        "{\"t\":\"READY\",\"s\":99,\"op\":0,\"d\":{\"session_id\":\"mock-session\","
        "\"guilds\":[{\"id\":\"333\",\"unavailable\":false},{\"id\":\"444\",\"unavailable\":true}],"
        "\"resume_gateway_url\":\"wss://gateway.discord.gg/?v=10&encoding=json\"}}";
    dcc_json_gateway_payload_t *payload = dcc_json_smoke_payload();
    if (payload == NULL) {
        fprintf(stderr, "json smoke payload allocation failed\n");
        return 1;
    }
    st = dcc_json_parse_gateway_payload(gateway_input, sizeof(gateway_input) - 1, payload);
    if (st != DCC_OK ||
        !payload->has_op ||
        payload->op != 0 ||
        !payload->has_seq ||
        payload->seq != 99 ||
        !payload->has_event_name ||
        strcmp(payload->event_name, "READY") != 0 ||
        !payload->has_session_id ||
        strcmp(payload->session_id, "mock-session") != 0 ||
        payload->ready_guild_ids == NULL ||
        payload->ready_guild_ids_count != 2 ||
        payload->ready_guild_count != 2 ||
        payload->ready_guild_ids[0] != 333 ||
        payload->ready_guild_ids[1] != 444 ||
        !payload->has_resume_gateway_url ||
        strcmp(payload->resume_gateway_url, "wss://gateway.discord.gg/?v=10&encoding=json") != 0) {
        fprintf(stderr, "gateway projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char message_input[] =
        "{\"t\":\"MESSAGE_CREATE\",\"s\":100,\"op\":0,\"d\":{\"id\":\"111\","
        "\"channel_id\":\"222\",\"guild_id\":\"333\",\"content\":\"hello\","
        "\"components\":[{\"type\":17,\"id\":1,\"accent_color\":5793266,"
        "\"components\":[{\"type\":10,\"content\":\"status\"},"
        "{\"type\":9,\"components\":[{\"type\":10,\"content\":\"body\"}],"
        "\"accessory\":{\"type\":2,\"style\":1,\"label\":\"Refresh\","
        "\"custom_id\":\"status.refresh\",\"disabled\":false}},"
        "{\"type\":12,\"items\":[{\"media\":{\"url\":\"attachment://graph.png\"},"
        "\"description\":\"Graph\",\"spoiler\":true}]}]},"
        "{\"type\":14,\"divider\":true,\"spacing\":1}],"
        "\"timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"edited_timestamp\":\"2026-06-15T01:00:00.000000+00:00\","
        "\"tts\":false,\"mention_everyone\":true,\"pinned\":true,"
        "\"type\":19,\"webhook_id\":\"777\","
        "\"author\":{\"id\":\"444\",\"username\":\"feral\"}}}";
    st = dcc_json_parse_gateway_payload(message_input, sizeof(message_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.message_id != 0 ||
        payload->gateway.id != 111 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.guild_id != 333 ||
        payload->message.webhook_id != 777 ||
        payload->message.type != 19 ||
        payload->message.tts != 0 ||
        payload->message.mention_everyone != 1 ||
        payload->message.pinned != 1 ||
        payload->message.timestamp == NULL ||
        strcmp(payload->message.timestamp, "2026-06-15T00:00:00.000000+00:00") != 0 ||
        payload->message.edited_timestamp == NULL ||
        strcmp(payload->message.edited_timestamp, "2026-06-15T01:00:00.000000+00:00") != 0 ||
        payload->message.components_json == NULL ||
        strstr(payload->message.components_json, "\"custom_id\":\"status.refresh\"") == NULL ||
        payload->message.components_json_truncated != 0 ||
        payload->message.components == NULL ||
        payload->message.components_count != 7 ||
        payload->message.component_root_count != 2 ||
        payload->message.components_truncated != 0 ||
        payload->gateway.timestamp_text == NULL ||
        strcmp(payload->gateway.timestamp_text, "2026-06-15T00:00:00.000000+00:00") != 0) {
        fprintf(stderr, "gateway message timestamp projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }
    const dcc_component_v2_t *container = dcc_message_component_root(&payload->message, 0);
    const dcc_component_v2_t *separator = dcc_message_component_root(&payload->message, 1);
    const dcc_component_v2_t *title = dcc_component_v2_child(&payload->message, container, 0);
    const dcc_component_v2_t *section = dcc_component_v2_child(&payload->message, container, 1);
    const dcc_component_v2_t *gallery = dcc_component_v2_child(&payload->message, container, 2);
    const dcc_component_v2_t *body = dcc_component_v2_child(&payload->message, section, 0);
    const dcc_component_v2_t *refresh = dcc_component_v2_accessory(&payload->message, section);
    const dcc_component_v2_t *refresh_by_id =
        dcc_message_component_by_custom_id(&payload->message, "status.refresh");
    if (container == NULL ||
        container->type != DCC_COMPONENT_V2_CONTAINER ||
        !container->has_id ||
        container->id != 1 ||
        !container->has_accent_color ||
        container->accent_color != 5793266U ||
        container->child_count != 3 ||
        title == NULL ||
        title->type != DCC_COMPONENT_V2_TEXT_DISPLAY ||
        title->content == NULL ||
        strcmp(title->content, "status") != 0 ||
        section == NULL ||
        section->type != DCC_COMPONENT_V2_SECTION ||
        body == NULL ||
        body->content == NULL ||
        strcmp(body->content, "body") != 0 ||
        refresh == NULL ||
        refresh != refresh_by_id ||
        refresh->type != DCC_COMPONENT_V2_BUTTON ||
        refresh->style != DCC_BUTTON_PRIMARY ||
        refresh->label == NULL ||
        strcmp(refresh->label, "Refresh") != 0 ||
        !refresh->has_disabled ||
        refresh->disabled != 0 ||
        gallery == NULL ||
        gallery->type != DCC_COMPONENT_V2_MEDIA_GALLERY ||
        gallery->media_count != 1 ||
        gallery->media == NULL ||
        gallery->media[0].url == NULL ||
        strcmp(gallery->media[0].url, "attachment://graph.png") != 0 ||
        gallery->media[0].description == NULL ||
        strcmp(gallery->media[0].description, "Graph") != 0 ||
        !gallery->media[0].has_spoiler ||
        gallery->media[0].spoiler != 1 ||
        separator == NULL ||
        separator->type != DCC_COMPONENT_V2_SEPARATOR ||
        !separator->has_divider ||
        separator->divider != 1 ||
        !separator->has_spacing ||
        separator->spacing != DCC_COMPONENT_V2_SPACING_SMALL) {
        fprintf(stderr, "gateway message component v2 projection validation failed\n");
        return 1;
    }
    dcc_message_t *message_copy = dcc_message_clone(&payload->message);
    const dcc_component_v2_t *copy_refresh =
        message_copy != NULL ? dcc_message_component_by_custom_id(message_copy, "status.refresh") : NULL;
    if (message_copy == NULL ||
        message_copy->components_json == NULL ||
        message_copy->components_json == payload->message.components_json ||
        strcmp(message_copy->components_json, payload->message.components_json) != 0 ||
        message_copy->components == NULL ||
        message_copy->components == payload->message.components ||
        message_copy->components_count != payload->message.components_count ||
        copy_refresh == NULL ||
        copy_refresh->label == NULL ||
        copy_refresh->label == refresh->label ||
        strcmp(copy_refresh->label, "Refresh") != 0) {
        fprintf(stderr, "gateway message components clone validation failed\n");
        dcc_message_free(message_copy);
        return 1;
    }
    dcc_message_free(message_copy);

    const char member_input[] =
        "{\"t\":\"GUILD_MEMBER_ADD\",\"s\":101,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"global_name\":\"Member\",\"discriminator\":\"0\",\"bot\":false}}}";
    st = dcc_json_parse_gateway_payload(member_input, sizeof(member_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.user_id != 446 ||
        payload->gateway.user.id != 446 ||
        payload->gateway.user.username == NULL ||
        strcmp(payload->gateway.user.username, "member") != 0 ||
        payload->gateway.user.global_name == NULL ||
        strcmp(payload->gateway.user.global_name, "Member") != 0 ||
        payload->gateway.user.discriminator != 0 ||
        payload->gateway.user.bot != 0) {
        fprintf(stderr, "gateway user projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char voice_state_input[] =
        "{\"t\":\"VOICE_STATE_UPDATE\",\"s\":102,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_id\":null,\"user_id\":\"444\","
        "\"session_id\":\"voice-session\","
        "\"request_to_speak_timestamp\":\"2026-06-15T03:00:00.000000+00:00\","
        "\"deaf\":false,\"mute\":true,\"self_deaf\":false,\"self_mute\":true,"
        "\"self_stream\":false,\"self_video\":false,\"suppress\":true}}";
    st = dcc_json_parse_gateway_payload(voice_state_input, sizeof(voice_state_input) - 1, payload);
    if (st != DCC_OK ||
        payload->voice_state.guild_id != 333 ||
        payload->voice_state.channel_id != 0 ||
        payload->voice_state.user_id != 444 ||
        payload->voice_state.session_id == NULL ||
        strcmp(payload->voice_state.session_id, "voice-session") != 0 ||
        payload->voice_state.request_to_speak_timestamp == NULL ||
        strcmp(payload->voice_state.request_to_speak_timestamp, "2026-06-15T03:00:00.000000+00:00") != 0 ||
        payload->voice_state.deaf != 0 ||
        payload->voice_state.mute != 1 ||
        payload->voice_state.self_deaf != 0 ||
        payload->voice_state.self_mute != 1 ||
        payload->voice_state.self_stream != 0 ||
        payload->voice_state.self_video != 0 ||
        payload->voice_state.suppress != 1) {
        fprintf(stderr, "gateway voice state projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char user_update_input[] =
        "{\"t\":\"USER_UPDATE\",\"s\":103,\"op\":0,\"d\":{\"id\":\"447\","
        "\"username\":\"self\",\"global_name\":null,\"avatar\":\"user-avatar\","
        "\"discriminator\":\"0\",\"bot\":false,\"public_flags\":64}}";
    st = dcc_json_parse_gateway_payload(user_update_input, sizeof(user_update_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.user_id != 447 ||
        payload->gateway.user.id != 447 ||
        payload->gateway.user.username == NULL ||
        strcmp(payload->gateway.user.username, "self") != 0 ||
        payload->gateway.user.global_name != NULL ||
        payload->gateway.user.avatar == NULL ||
        strcmp(payload->gateway.user.avatar, "user-avatar") != 0 ||
        payload->gateway.user.flags != 64 ||
        payload->gateway.user.discriminator != 0 ||
        payload->gateway.user.bot != 0 ||
        !payload->has_user ||
        payload->user.id != 447 ||
        payload->user.username == NULL ||
        strcmp(payload->user.username, "self") != 0 ||
        payload->user.global_name != NULL ||
        payload->user.avatar == NULL ||
        strcmp(payload->user.avatar, "user-avatar") != 0 ||
        payload->user.flags != 64) {
        fprintf(stderr, "gateway user update projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char channel_input[] =
        "{\"t\":\"CHANNEL_CREATE\",\"s\":102,\"op\":0,\"d\":{\"id\":\"222\","
        "\"guild_id\":\"333\",\"parent_id\":\"111\",\"last_message_id\":\"999\","
        "\"type\":0,\"name\":\"general\",\"topic\":\"chat\","
        "\"position\":2,\"nsfw\":true,\"rate_limit_per_user\":5,"
        "\"bitrate\":64000,\"user_limit\":25}}";
    st = dcc_json_parse_gateway_payload(channel_input, sizeof(channel_input) - 1, payload);
    if (st != DCC_OK ||
        payload->channel.id != 222 ||
        payload->channel.guild_id != 333 ||
        payload->channel.parent_id != 111 ||
        payload->channel.last_message_id != 999 ||
        payload->channel.type != 0 ||
        payload->channel.name == NULL ||
        strcmp(payload->channel.name, "general") != 0 ||
        payload->channel.topic == NULL ||
        strcmp(payload->channel.topic, "chat") != 0 ||
        payload->channel.position != 2 ||
        payload->channel.nsfw != 1 ||
        payload->channel.rate_limit_per_user != 5 ||
        payload->channel.bitrate != 64000 ||
        payload->channel.user_limit != 25) {
        fprintf(stderr, "gateway channel projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char role_input[] =
        "{\"t\":\"GUILD_ROLE_CREATE\",\"s\":103,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"role\":{\"id\":\"555\",\"name\":\"mods\",\"permissions\":\"1024\","
        "\"color\":16711680,\"position\":4,\"hoist\":true,"
        "\"managed\":false,\"mentionable\":true}}}";
    st = dcc_json_parse_gateway_payload(role_input, sizeof(role_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.role_id != 555 ||
        payload->gateway.name == NULL ||
        strcmp(payload->gateway.name, "mods") != 0 ||
        payload->role.id != 555 ||
        payload->role.guild_id != 333 ||
        payload->role.name == NULL ||
        strcmp(payload->role.name, "mods") != 0 ||
        payload->role.permissions != 1024 ||
        payload->role.color != 16711680 ||
        payload->role.position != 4 ||
        payload->role.hoist != 1 ||
        payload->role.managed != 0 ||
        payload->role.mentionable != 1) {
        fprintf(stderr, "gateway role projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    if (dcc_json_smoke_gateway_foundation_guild_create() != 0) {
        return 1;
    }

    if (dcc_json_smoke_gateway_foundation_automation() != 0 ||
        dcc_json_smoke_gateway_foundation_members() != 0) {
        return 1;
    }

    return 0;
}
