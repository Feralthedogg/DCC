#include "json_smoke_gateway_extended_internal.h"
#include "json_smoke_support.h"

int dcc_json_smoke_gateway_extended(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t *payload = dcc_json_smoke_payload();
    if (payload == NULL) {
        fprintf(stderr, "json smoke payload allocation failed\n");
        return 1;
    }

    const char pins_input[] =
        "{\"t\":\"CHANNEL_PINS_UPDATE\",\"s\":110,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"channel_id\":\"222\",\"last_pin_timestamp\":\"2026-06-15T00:00:00.000000+00:00\"}}";
    st = dcc_json_parse_gateway_payload(pins_input, sizeof(pins_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.timestamp_text == NULL ||
        payload->channel_pins_update.guild_id != 333 ||
        payload->channel_pins_update.channel_id != 222 ||
        payload->channel_pins_update.last_pin_timestamp == NULL) {
        fprintf(stderr, "gateway pins projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char burst_reaction_input[] =
        "{\"t\":\"MESSAGE_REACTION_ADD\",\"s\":111,\"op\":0,\"d\":{\"user_id\":\"446\","
        "\"channel_id\":\"222\",\"message_id\":\"111\",\"guild_id\":\"333\","
        "\"message_author_id\":\"445\",\"burst\":true,"
        "\"member\":{\"user\":{\"id\":\"446\",\"username\":\"reactor\","
        "\"global_name\":\"Reactor\",\"discriminator\":\"0\",\"bot\":false},"
        "\"nick\":\"spark\",\"roles\":[\"555\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":false,"
        "\"communication_disabled_until\":null,\"flags\":4},"
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\",\"animated\":true}}}";
    st = dcc_json_parse_gateway_payload(burst_reaction_input, sizeof(burst_reaction_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.user_id != 446 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.message_id != 111 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.message_author_id != 445 ||
        payload->gateway.burst != 1 ||
        payload->gateway.emoji_id != 9999 ||
        payload->gateway.emoji_name == NULL ||
        strcmp(payload->gateway.emoji_name, "spark") != 0 ||
        payload->gateway.user.id != 446 ||
        payload->gateway.user.username == NULL ||
        strcmp(payload->gateway.user.username, "reactor") != 0 ||
        payload->gateway.nick == NULL ||
        strcmp(payload->gateway.nick, "spark") != 0 ||
        payload->gateway.role_ids == NULL ||
        payload->gateway.role_ids_count != 1 ||
        payload->gateway.role_ids[0] != 555 ||
        payload->message_reaction.user_id != 446 ||
        payload->message_reaction.user.id != 446 ||
        payload->message_reaction.user.username == NULL ||
        strcmp(payload->message_reaction.user.username, "reactor") != 0 ||
        payload->message_reaction.member.guild_id != 333 ||
        payload->message_reaction.member.user.id != 446 ||
        payload->message_reaction.member.user.username == NULL ||
        strcmp(payload->message_reaction.member.user.username, "reactor") != 0 ||
        payload->message_reaction.member.nick == NULL ||
        strcmp(payload->message_reaction.member.nick, "spark") != 0 ||
        payload->message_reaction.member.role_ids == NULL ||
        payload->message_reaction.member.role_ids_count != 1 ||
        payload->message_reaction.member.role_ids[0] != 555 ||
        payload->message_reaction.member.flags != 4 ||
        payload->message_reaction.channel_id != 222 ||
        payload->message_reaction.message_id != 111 ||
        payload->message_reaction.guild_id != 333 ||
        payload->message_reaction.message_author_id != 445 ||
        payload->message_reaction.burst != 1 ||
        payload->message_reaction.emoji_id != 9999 ||
        payload->message_reaction.emoji_name == NULL ||
        payload->message_reaction.emoji.id != 9999 ||
        payload->message_reaction.emoji.name == NULL ||
        payload->message_reaction.emoji.animated != 1 ||
        strcmp(payload->message_reaction.emoji.name, "spark") != 0 ||
        strcmp(payload->message_reaction.emoji_name, "spark") != 0) {
        fprintf(stderr, "gateway burst reaction projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char stage_input[] =
        "{\"t\":\"STAGE_INSTANCE_CREATE\",\"s\":112,\"op\":0,\"d\":{"
        "\"id\":\"900\",\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"topic\":\"town hall\",\"privacy_level\":2,"
        "\"discoverable_disabled\":true,\"guild_scheduled_event_id\":\"888\"}}";
    st = dcc_json_parse_gateway_payload(stage_input, sizeof(stage_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.id != 900 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.channel_id != 222 ||
        payload->gateway.topic == NULL ||
        strcmp(payload->gateway.topic, "town hall") != 0 ||
        payload->gateway.privacy_level != 2 ||
        payload->gateway.discoverable_disabled != 1 ||
        payload->gateway.scheduled_event_id != 888 ||
        payload->stage_instance.id != 900 ||
        payload->stage_instance.guild_id != 333 ||
        payload->stage_instance.channel_id != 222 ||
        payload->stage_instance.topic == NULL ||
        strcmp(payload->stage_instance.topic, "town hall") != 0 ||
        payload->stage_instance.privacy_level != 2 ||
        payload->stage_instance.discoverable_disabled != 1 ||
        payload->stage_instance.scheduled_event_id != 888) {
        fprintf(stderr, "gateway stage projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char soundboard_input[] =
        "{\"t\":\"GUILD_SOUNDBOARD_SOUND_CREATE\",\"s\":113,\"op\":0,\"d\":{"
        "\"id\":\"5678\",\"guild_id\":\"333\",\"name\":\"spark\","
        "\"volume\":0.75,\"emoji_id\":\"9999\",\"emoji_name\":\"spark\","
        "\"available\":true,\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"avatar\":\"member-avatar\",\"public_flags\":64}}}";
    st = dcc_json_parse_gateway_payload(soundboard_input, sizeof(soundboard_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.sound_id != 5678 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.name == NULL ||
        strcmp(payload->gateway.name, "spark") != 0 ||
        payload->gateway.volume < 0.74 ||
        payload->gateway.volume > 0.76 ||
        payload->gateway.emoji_id != 9999 ||
        payload->gateway.emoji_name == NULL ||
        strcmp(payload->gateway.emoji_name, "spark") != 0 ||
        payload->gateway.available != 1 ||
        payload->gateway.user_id != 446 ||
        payload->soundboard_sound.id != 5678 ||
        payload->soundboard_sound.guild_id != 333 ||
        payload->soundboard_sound.name == NULL ||
        strcmp(payload->soundboard_sound.name, "spark") != 0 ||
        payload->soundboard_sound.volume < 0.74 ||
        payload->soundboard_sound.volume > 0.76 ||
        payload->soundboard_sound.emoji_id != 9999 ||
        payload->soundboard_sound.emoji_name == NULL ||
        strcmp(payload->soundboard_sound.emoji_name, "spark") != 0 ||
        payload->soundboard_sound.available != 1 ||
        payload->soundboard_sound.user_id != 446 ||
        payload->soundboard_sound.user.id != 446 ||
        payload->soundboard_sound.user.username == NULL ||
        strcmp(payload->soundboard_sound.user.username, "member") != 0 ||
        payload->soundboard_sound.user.avatar == NULL ||
        strcmp(payload->soundboard_sound.user.avatar, "member-avatar") != 0 ||
        payload->soundboard_sound.user.flags != 64) {
        fprintf(stderr, "gateway soundboard projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char entitlement_input[] =
        "{\"t\":\"ENTITLEMENT_CREATE\",\"s\":114,\"op\":0,\"d\":{"
        "\"id\":\"700\",\"sku_id\":\"123\",\"application_id\":\"666\","
        "\"subscription_id\":\"800\",\"promotion_id\":\"900\","
        "\"gift_code_flags\":3,\"flags\":2,"
        "\"user_id\":\"446\",\"guild_id\":\"333\",\"type\":8,"
        "\"starts_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"ends_at\":null}}";
    st = dcc_json_parse_gateway_payload(entitlement_input, sizeof(entitlement_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.entitlement_id != 700 ||
        payload->gateway.sku_id != 123 ||
        payload->gateway.application_id != 666 ||
        payload->gateway.subscription_id != 800 ||
        payload->gateway.promotion_id != 900 ||
        payload->gateway.gift_code_flags != 3 ||
        payload->gateway.flags != 2 ||
        payload->gateway.user_id != 446 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.type != 8 ||
        payload->gateway.start_time == NULL ||
        payload->gateway.end_time != NULL ||
        payload->entitlement.id != 700 ||
        payload->entitlement.sku_id != 123 ||
        payload->entitlement.application_id != 666 ||
        payload->entitlement.subscription_id != 800 ||
        payload->entitlement.promotion_id != 900 ||
        payload->entitlement.gift_code_flags != 3 ||
        payload->entitlement.flags != 2 ||
        payload->entitlement.user_id != 446 ||
        payload->entitlement.guild_id != 333 ||
        payload->entitlement.type != 8 ||
        payload->entitlement.start_time == NULL ||
        payload->entitlement.end_time != NULL) {
        fprintf(stderr, "gateway entitlement projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char subscription_input[] =
        "{\"t\":\"SUBSCRIPTION_UPDATE\",\"s\":115,\"op\":0,\"d\":{"
        "\"id\":\"999\",\"user_id\":\"446\",\"sku_ids\":[\"123\",\"124\"],"
        "\"entitlement_ids\":[\"700\"],"
        "\"current_period_start\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"current_period_end\":\"2026-07-15T00:00:00.000000+00:00\","
        "\"canceled_at\":null,\"status\":1}}";
    st = dcc_json_parse_gateway_payload(subscription_input, sizeof(subscription_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.id != 999 ||
        payload->gateway.user_id != 446 ||
        payload->gateway.sku_ids == NULL ||
        payload->gateway.sku_ids_count != 2 ||
        payload->gateway.sku_ids[0] != 123 ||
        payload->gateway.sku_ids[1] != 124 ||
        payload->gateway.entitlement_ids == NULL ||
        payload->gateway.entitlement_ids_count != 1 ||
        payload->gateway.entitlement_ids[0] != 700 ||
        payload->gateway.current_period_start == NULL ||
        payload->gateway.current_period_end == NULL ||
        payload->gateway.canceled_at != NULL ||
        payload->gateway.status == NULL ||
        strcmp(payload->gateway.status, "1") != 0 ||
        payload->subscription.id != 999 ||
        payload->subscription.user_id != 446 ||
        payload->subscription.sku_ids == NULL ||
        payload->subscription.sku_ids_count != 2 ||
        payload->subscription.sku_ids[0] != 123 ||
        payload->subscription.sku_ids[1] != 124 ||
        payload->subscription.entitlement_ids == NULL ||
        payload->subscription.entitlement_ids_count != 1 ||
        payload->subscription.entitlement_ids[0] != 700 ||
        payload->subscription.current_period_start == NULL ||
        payload->subscription.current_period_end == NULL ||
        payload->subscription.canceled_at != NULL ||
        payload->subscription.status == NULL ||
        strcmp(payload->subscription.status, "1") != 0) {
        fprintf(stderr, "gateway subscription projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char integration_input[] =
        "{\"t\":\"INTEGRATION_CREATE\",\"s\":116,\"op\":0,\"d\":{"
        "\"id\":\"321\",\"guild_id\":\"333\",\"name\":\"builds\","
        "\"type\":\"discord\",\"enabled\":true,\"role_id\":\"555\"}}";
    st = dcc_json_parse_gateway_payload(integration_input, sizeof(integration_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.id != 321 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.name == NULL ||
        strcmp(payload->gateway.name, "builds") != 0 ||
        payload->gateway.type_text == NULL ||
        strcmp(payload->gateway.type_text, "discord") != 0 ||
        payload->gateway.enabled != 1 ||
        payload->gateway.role_id != 555) {
        fprintf(stderr, "gateway integration projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char nullable_id_input[] =
        "{\"t\":\"INTEGRATION_UPDATE\",\"s\":117,\"op\":0,\"d\":{"
        "\"id\":\"321\",\"guild_id\":\"333\",\"application_id\":null,"
        "\"role_id\":null,\"sku_id\":null,\"guild_scheduled_event_id\":null}}";
    st = dcc_json_parse_gateway_payload(nullable_id_input, sizeof(nullable_id_input) - 1, payload);
    if (st != DCC_OK ||
        payload->gateway.id != 321 ||
        payload->gateway.guild_id != 333 ||
        payload->gateway.application_id != 0 ||
        payload->gateway.role_id != 0 ||
        payload->gateway.sku_id != 0 ||
        payload->gateway.scheduled_event_id != 0) {
        fprintf(stderr, "gateway nullable id projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    if (dcc_json_smoke_gateway_extended_presence_lists() != 0) {
        return 1;
    }

    return 0;
}
