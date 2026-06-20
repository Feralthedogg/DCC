#include "json_smoke_support.h"

int dcc_json_smoke_interactions(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t *payload = dcc_json_smoke_payload();
    if (payload == NULL) {
        fprintf(stderr, "json smoke payload allocation failed\n");
        return 1;
    }
    dcc_interaction_t *interaction = &payload->interaction;

    const char interaction_options_input[] =
        "{\"t\":\"INTERACTION_CREATE\",\"s\":111,\"op\":0,\"d\":{\"id\":\"555\","
        "\"application_id\":\"666\",\"type\":2,\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"token\":\"interaction-token\",\"message_id\":\"111\",\"version\":1,"
        "\"locale\":\"ko\",\"guild_locale\":\"en-US\","
        "\"app_permissions\":\"8192\",\"attachment_size_limit\":26214400,"
        "\"context\":0,\"authorizing_integration_owners\":{\"0\":\"333\",\"1\":\"446\"},"
        "\"entitlements\":[{\"id\":\"700\",\"sku_id\":\"123\","
        "\"application_id\":\"666\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"type\":8,\"starts_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"ends_at\":null,\"flags\":2}],"
        "\"data\":{\"id\":\"777\",\"name\":\"ping\",\"type\":1,"
        "\"options\":[{\"name\":\"text\",\"type\":3,\"value\":\"hello\"},"
        "{\"name\":\"count\",\"type\":4,\"value\":42},"
        "{\"name\":\"ratio\",\"type\":10,\"value\":1.5},"
        "{\"name\":\"visible\",\"type\":5,\"value\":true},"
        "{\"name\":\"target\",\"type\":6,\"value\":\"446\"},"
        "{\"focused\":true,\"name\":\"query\",\"type\":3,\"value\":\"he\"}],"
        "\"resolved\":{"
        "\"users\":{\"446\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"}},"
        "\"members\":{\"446\":{\"user\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"},"
        "\"roles\":[\"555\"],\"nick\":\"TargetNick\","
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":false,"
        "\"communication_disabled_until\":null,\"flags\":2,\"permissions\":\"2048\"}},"
        "\"roles\":{\"555\":{\"id\":\"555\",\"name\":\"mods\",\"permissions\":\"8\","
        "\"color\":123,\"position\":4,\"hoist\":true,\"managed\":false,"
        "\"mentionable\":true}},"
        "\"channels\":{\"222\":{\"id\":\"222\",\"guild_id\":\"333\",\"type\":0,"
        "\"name\":\"general\",\"topic\":\"ops\",\"position\":2}},"
        "\"messages\":{\"111\":{\"id\":\"111\",\"channel_id\":\"222\","
        "\"guild_id\":\"333\",\"content\":\"hello resolved\","
        "\"timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"edited_timestamp\":null,"
        "\"author\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"},"
        "\"type\":0,\"flags\":64,\"tts\":false,\"mention_everyone\":false,"
        "\"pinned\":true}},"
        "\"attachments\":{\"888\":{\"id\":\"888\",\"filename\":\"clip.ogg\","
        "\"description\":\"voice clip\","
        "\"content_type\":\"audio/ogg\",\"size\":4096,"
        "\"url\":\"https://cdn.discordapp.com/attachments/clip.ogg\","
        "\"proxy_url\":\"https://media.discordapp.net/attachments/clip.ogg\","
        "\"height\":480,\"width\":640,\"ephemeral\":true,"
        "\"duration_secs\":1.25,\"waveform\":\"AAEC\",\"flags\":4}}}}}}";
    st = dcc_json_parse_gateway_payload(
        interaction_options_input,
        sizeof(interaction_options_input) - 1,
        payload
    );
    int64_t option_count = 0;
    double option_ratio = 0.0;
    uint8_t option_visible = 0;
    dcc_snowflake_t option_target = 0;
    const dcc_interaction_option_t *focused_option = dcc_interaction_focused_option(interaction);
    const char *text_option = dcc_interaction_option_string(interaction, "text");
    const char *query_option = dcc_interaction_option_string(interaction, "query");
    dcc_interaction_t *interaction_copy = dcc_interaction_clone(interaction);
    if (st != DCC_OK ||
        payload->has_interaction != 1 ||
        payload->interaction.options == NULL ||
        payload->interaction.options_count != 6 ||
        text_option == NULL ||
        strcmp(text_option, "hello") != 0 ||
        dcc_interaction_option_integer(interaction, "count", &option_count) != 1 ||
        option_count != 42 ||
        dcc_interaction_option_number(interaction, "ratio", &option_ratio) != 1 ||
        option_ratio < 1.49 ||
        option_ratio > 1.51 ||
        dcc_interaction_option_boolean(interaction, "visible", &option_visible) != 1 ||
        option_visible != 1 ||
        dcc_interaction_option_snowflake(interaction, "target", &option_target) != 1 ||
        option_target != 446 ||
        focused_option == NULL ||
        focused_option != payload->interaction.focused_option ||
        focused_option->name == NULL ||
        strcmp(focused_option->name, "query") != 0 ||
        query_option == NULL ||
        strcmp(query_option, "he") != 0 ||
        dcc_interaction_option_by_name(interaction, "missing") != NULL ||
        !dcc_json_smoke_interaction_resolved_ok(interaction) ||
        !dcc_json_smoke_interaction_metadata_ok(interaction) ||
        interaction_copy == NULL ||
        !dcc_json_smoke_interaction_resolved_clone_ok(interaction, interaction_copy)) {
        fprintf(stderr, "gateway interaction option validation failed: %s\n", dcc_status_string(st));
        dcc_interaction_free(interaction_copy);
        return 1;
    }
    dcc_interaction_free(interaction_copy);

    const char interaction_member_permissions_input[] =
        "{\"t\":\"INTERACTION_CREATE\",\"s\":111,\"op\":0,\"d\":{\"id\":\"557\","
        "\"application_id\":\"666\",\"type\":2,\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"token\":\"interaction-token\","
        "\"member\":{\"user\":{\"id\":\"447\",\"username\":\"issuer\"},\"permissions\":\"4096\"},"
        "\"data\":{\"id\":\"779\",\"name\":\"perm\",\"type\":1}}}";
    st = dcc_json_parse_gateway_payload(
        interaction_member_permissions_input,
        sizeof(interaction_member_permissions_input) - 1,
        payload
    );
    const dcc_interaction_resolved_permission_t *issuer_permissions =
        dcc_interaction_resolved_permission(interaction, 447);
    if (st != DCC_OK ||
        issuer_permissions == NULL ||
        issuer_permissions->permissions != 4096 ||
        dcc_interaction_resolved_permission(interaction, 999) != NULL) {
        fprintf(stderr, "gateway interaction member permission validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char interaction_nested_options_input[] =
        "{\"t\":\"INTERACTION_CREATE\",\"s\":112,\"op\":0,\"d\":{\"id\":\"556\","
        "\"application_id\":\"666\",\"type\":2,\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"token\":\"interaction-token\",\"data\":{\"id\":\"778\",\"name\":\"admin\",\"type\":1,"
        "\"options\":[{\"name\":\"moderation\",\"type\":2,\"options\":["
        "{\"name\":\"ban\",\"type\":1,\"options\":["
        "{\"name\":\"target\",\"type\":6,\"value\":\"446\"},"
        "{\"name\":\"reason\",\"type\":3,\"value\":\"spam\"}]}]}]}}}";
    st = dcc_json_parse_gateway_payload(
        interaction_nested_options_input,
        sizeof(interaction_nested_options_input) - 1,
        payload
    );
    dcc_snowflake_t nested_target = 0;
    const dcc_interaction_option_t *group = dcc_interaction_subcommand_group(interaction);
    const dcc_interaction_option_t *subcommand = dcc_interaction_subcommand(interaction);
    const dcc_interaction_option_t *ban_from_group =
        dcc_interaction_option_child_by_name(group, "ban");
    const dcc_interaction_option_t *reason_from_subcommand =
        dcc_interaction_option_child_by_name(subcommand, "reason");
    const char *nested_reason = dcc_interaction_option_string(interaction, "reason");
    dcc_interaction_t *nested_copy = dcc_interaction_clone(interaction);
    const dcc_interaction_option_t *copy_group = dcc_interaction_subcommand_group(nested_copy);
    const dcc_interaction_option_t *copy_subcommand = dcc_interaction_subcommand(nested_copy);
    const char *copy_reason = dcc_interaction_option_string(nested_copy, "reason");
    int copy_target_ok = dcc_interaction_option_snowflake(nested_copy, "target", &nested_target);
    if (st != DCC_OK ||
        payload->interaction.options == NULL ||
        payload->interaction.attachment_size_limit != DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT ||
        payload->interaction.options_count != 1 ||
        group == NULL ||
        group->type != 2 ||
        group->options_count != 1 ||
        subcommand == NULL ||
        subcommand->type != 1 ||
        subcommand->options_count != 2 ||
        ban_from_group != subcommand ||
        reason_from_subcommand == NULL ||
        reason_from_subcommand->value_type != DCC_INTERACTION_OPTION_VALUE_STRING ||
        nested_reason == NULL ||
        strcmp(nested_reason, "spam") != 0 ||
        copy_group == NULL ||
        copy_subcommand == NULL ||
        copy_group == group ||
        copy_subcommand == subcommand ||
        copy_group->options_count != 1 ||
        copy_subcommand->options_count != 2 ||
        copy_reason == NULL ||
        strcmp(copy_reason, "spam") != 0 ||
        copy_target_ok != 1 ||
        nested_target != 446) {
        fprintf(stderr, "gateway nested interaction option validation failed: %s\n", dcc_status_string(st));
        dcc_interaction_free(nested_copy);
        return 1;
    }
    dcc_interaction_free(nested_copy);

    const char interaction_components_v2_input[] =
        "{\"t\":\"INTERACTION_CREATE\",\"s\":113,\"op\":0,\"d\":{\"id\":\"557\","
        "\"application_id\":\"666\",\"type\":5,\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"token\":\"interaction-token\",\"data\":{\"custom_id\":\"modal-v2\","
        "\"components\":["
        "{\"type\":18,\"label\":\"Name\",\"component\":"
        "{\"type\":4,\"custom_id\":\"name\",\"value\":\"Feral\"}},"
        "{\"type\":18,\"label\":\"Accept\",\"component\":"
        "{\"type\":23,\"custom_id\":\"accept\",\"value\":true}},"
        "{\"type\":17,\"components\":[{\"type\":1,\"components\":["
        "{\"type\":3,\"custom_id\":\"tags\",\"values\":[\"alpha\",\"beta\"]}]}]}]}}}";
    st = dcc_json_parse_gateway_payload(
        interaction_components_v2_input,
        sizeof(interaction_components_v2_input) - 1,
        payload
    );
    const dcc_interaction_form_field_t *name_field =
        dcc_interaction_form_field_by_custom_id(interaction, "name");
    const dcc_interaction_form_field_t *accept_field =
        dcc_interaction_form_field_by_custom_id(interaction, "accept");
    const dcc_interaction_form_field_t *tags_field =
        dcc_interaction_form_field_by_custom_id(interaction, "tags");
    const char *name_value = dcc_interaction_form_value(interaction, "name");
    uint8_t accepted = 0;
    int accepted_ok = dcc_interaction_form_boolean(interaction, "accept", &accepted);
    dcc_interaction_t *components_copy = dcc_interaction_clone(interaction);
    const dcc_interaction_form_field_t *copy_tags =
        dcc_interaction_form_field_by_custom_id(components_copy, "tags");
    if (st != DCC_OK ||
        payload->has_interaction != 1 ||
        payload->interaction.form_fields == NULL ||
        payload->interaction.form_fields_count != 3 ||
        name_field == NULL ||
        name_field->component_type != 4U ||
        name_value == NULL ||
        strcmp(name_value, "Feral") != 0 ||
        accept_field == NULL ||
        accept_field->component_type != 23U ||
        accepted_ok != 1 ||
        accepted != 1 ||
        tags_field == NULL ||
        tags_field->component_type != 3U ||
        tags_field->value_type != DCC_INTERACTION_FORM_VALUE_VALUES ||
        tags_field->values_count != 2 ||
        tags_field->values == NULL ||
        strcmp(tags_field->values[0], "alpha") != 0 ||
        strcmp(tags_field->values[1], "beta") != 0 ||
        dcc_interaction_form_value(interaction, "missing") != NULL ||
        components_copy == NULL ||
        copy_tags == NULL ||
        copy_tags == tags_field ||
        copy_tags->values == tags_field->values ||
        copy_tags->values_count != 2 ||
        strcmp(copy_tags->values[1], "beta") != 0) {
        fprintf(stderr, "gateway interaction components v2 validation failed: %s\n", dcc_status_string(st));
        dcc_interaction_free(components_copy);
        return 1;
    }
    dcc_interaction_free(components_copy);

    return 0;
}
