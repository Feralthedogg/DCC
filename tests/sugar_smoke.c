#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int run_sugar_message_smoke(void) {
    dcc_embed_builder_t embed =
        DCC_EMBED_WITH_FIELDS("Status", "Gateway online", DCC_EMBED_FIELD_INLINE("Shard", "0"));
    embed.footer = DCC_EMBED_FOOTER("DCC", NULL);
    embed.has_footer = 1U;
    embed.image = DCC_EMBED_IMAGE("attachment://status.png");
    embed.has_image = 1U;
    embed.author = DCC_EMBED_AUTHOR("runtime", NULL, NULL);
    embed.has_author = 1U;
    dcc_message_builder_t message = DCC_MESSAGE_TEXT_EMBEDS("ready", embed);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"content\":\"ready\"") ||
        require_contains(json, "\"embeds\":[{\"title\":\"Status\"") ||
        require_contains(json, "\"inline\":true") ||
        require_contains(json, "\"footer\":{\"text\":\"DCC\"") ||
        require_contains(json, "\"image\":{\"url\":\"attachment://status.png\"") ||
        require_contains(json, "\"author\":{\"name\":\"runtime\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_legacy_component_smoke(void) {
    dcc_component_builder_t row =
        DCC_ACTION_ROW(DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"));

    char *json = NULL;
    if (dcc_component_builder_build_json(&row, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar legacy component\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"type\":2") ||
        require_contains(json, "\"style\":1") ||
        require_contains(json, "\"custom_id\":\"status.refresh\"");
    dcc_component_builder_json_free(json);
    return failed;
}

static int run_sugar_component_v2_smoke(void) {
    dcc_component_v2_builder_t container =
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2U,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            ),
            DCC_V2_SEPARATOR_SMALL()
        );

    char *json = NULL;
    if (dcc_component_v2_builder_build_json(&container, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 container\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":17") ||
        require_contains(json, "\"type\":10") ||
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"type\":14") ||
        require_contains(json, "\"accent_color\":5793266") ||
        require_contains(json, "\"custom_id\":\"status.refresh\"");
    dcc_component_v2_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t row =
        DCC_V2_ACTION_ROW(
            DCC_V2_BUTTON_SUCCESS("Yes", "confirm.yes"),
            DCC_V2_BUTTON_DANGER("No", "confirm.no")
        );
    if (dcc_component_v2_builder_build_json(&row, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 action row\n");
        return 1;
    }
    failed =
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"custom_id\":\"confirm.yes\"") ||
        require_contains(json, "\"custom_id\":\"confirm.no\"");
    dcc_component_v2_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_message_builder_t message = DCC_MESSAGE_COMPONENTS_V2(container);
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"components\":[{\"type\":17");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_modal_smoke(void) {
    dcc_component_builder_t birthday_row =
        DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD", 1U));
    dcc_modal_builder_t birthday_modal =
        DCC_MODAL("birthday-modal", "Birthday", birthday_row);

    char *json = NULL;
    if (dcc_modal_builder_build_json(&birthday_modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar legacy modal alias\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"custom_id\":\"birthday-modal\"") ||
        require_contains(json, "\"placeholder\":\"YYYYMMDD\"") ||
        require_contains(json, "\"required\":true");
    dcc_modal_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("upload.config");
    upload.min_values = 1U;
    upload.max_values = 2U;
    upload.required = 1U;
    upload.has_min_values = 1U;
    upload.has_max_values = 1U;
    upload.has_required = 1U;

    dcc_component_v2_builder_t label = DCC_V2_LABEL("Config files", upload);
    dcc_component_v2_builder_t birthday = DCC_V2_LABEL(
        "Birthday",
        DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD", 1U)
    );
    dcc_component_v2_builder_t private_age =
        DCC_MODAL_V2_CHECKBOX("hide_age", "Hide age", 0U);
    dcc_modal_builder_t modal =
        DCC_MODAL_V2("upload-modal", "Upload config", label, birthday, private_age);

    json = NULL;
    if (dcc_modal_builder_build_json(&modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 modal\n");
        return 1;
    }
    failed =
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"custom_id\":\"upload-modal\"") ||
        require_contains(json, "\"components\":[{\"type\":18") ||
        require_contains(json, "\"component\":{\"type\":19") ||
        require_contains(json, "\"placeholder\":\"YYYYMMDD\"") ||
        require_contains(json, "\"type\":23");
    dcc_modal_builder_json_free(json);
    return failed;
}

static int run_sugar_command_smoke(void) {
    dcc_application_command_builder_t command =
        DCC_SLASH_COMMAND_OPTIONS("ping", "Latency check", "[]");

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar slash command\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"name\":\"ping\"") ||
        require_contains(json, "\"description\":\"Latency check\"") ||
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"options\":[]");
    dcc_application_command_builder_json_free(json);
    return failed;
}

static int run_sugar_array_compat_smoke(void) {
    dcc_embed_field_t fields[] = {
        DCC_EMBED_FIELD("Mode", "array"),
        DCC_EMBED_FIELD_INLINE("Shard", "1"),
    };
    dcc_embed_builder_t embed =
        DCC_EMBED_WITH_FIELDS_ARRAY("Compat", "Explicit arrays", fields, DCC_ARRAY_LEN(fields));
    dcc_message_builder_t message =
        DCC_MESSAGE_EMBEDS_ARRAY(&embed, 1U);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar array compat message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"title\":\"Compat\"") ||
        require_contains(json, "\"name\":\"Mode\"") ||
        require_contains(json, "\"inline\":true");
    dcc_message_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t title = DCC_V2_TEXT("array compat");
    dcc_component_v2_builder_t refresh =
        DCC_V2_BUTTON_PRIMARY("Refresh", "compat.refresh");
    dcc_component_v2_builder_t section =
        DCC_V2_SECTION_ARRAY(&title, 1U, &refresh);
    dcc_component_v2_builder_t container =
        DCC_V2_CONTAINER_ACCENT_ARRAY(&section, 1U, 0x57F287U);
    dcc_message_builder_t component_message =
        DCC_MESSAGE_COMPONENTS_V2_ARRAY(&container, 1U);

    json = NULL;
    if (dcc_message_builder_build_json(&component_message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar array compat components v2 message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"custom_id\":\"compat.refresh\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_options_smoke(void) {
    dcc_client_options_t client = DCC_CLIENT_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_client_options_t sharded_client =
        DCC_CLIENT_SHARDED_OPTIONS("token", DCC_INTENTS_MESSAGES, 1U, 2U);
    dcc_client_options_t inferred_client =
        DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE("token", DCC_INTENTS_ALL);
    dcc_command_registry_options_t registry =
        DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN(123ULL);
    dcc_command_registry_options_t registry_delete_stale =
        DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DELETE_STALE();
    dcc_application_command_registration_options_t registration =
        DCC_APPLICATION_COMMAND_REGISTRATION_GLOBAL();
    dcc_application_command_registration_options_t guild_registration =
        DCC_APPLICATION_COMMAND_REGISTRATION_GUILD(456ULL);
    dcc_component_session_options_t session_options =
        DCC_COMPONENT_SESSION_OPTIONS_USER("secret", strlen("secret"), 789ULL);
    dcc_component_session_check_t session_check =
        DCC_COMPONENT_SESSION_CHECK("dcc:v1:session:next:nonce:sig", 1000U, 789ULL, 111ULL, 222ULL);
    dcc_component_session_listener_options_t listener =
        DCC_COMPONENT_SESSION_LISTENER_OPTIONS();
    dcc_hot_reload_canary_options_t canary =
        DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT();
    dcc_hot_reload_options_t hot_reload =
        DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS("dcc_hot_reload_worker", canary);
    dcc_rest_firewall_options_t firewall = DCC_REST_FIREWALL_OPTIONS();
    dcc_rest_firewall_request_t firewall_request =
        DCC_REST_FIREWALL_REQUEST("POST", "/interactions/1/token/callback", 1000U, 1U);
    dcc_replay_record_t replay =
        DCC_REPLAY_GATEWAY_RECORD("READY", 1000U, "{\"op\":0}", strlen("{\"op\":0}"));
    dcc_replay_record_t interaction_replay =
        DCC_REPLAY_INTERACTION_RECORD(1001U, "{\"type\":2}", strlen("{\"type\":2}"));
    dcc_interaction_flow_t flow = DCC_FLOW_AUTO_DEFER_EPHEMERAL(NULL, NULL, 1500U);
    dcc_message_builder_t stickers = DCC_MESSAGE_STICKERS(111ULL, 222ULL);
    dcc_poll_builder_t poll =
        DCC_POLL("Deploy?", 24U, DCC_POLL_ANSWER("Now"), DCC_POLL_ANSWER("Later"));
    dcc_component_builder_t legacy_select =
        DCC_STRING_SELECT_BUILDER(
            "legacy.select",
            DCC_SELECT_OPTION("One", "one"),
            DCC_SELECT_OPTION_DEFAULT("Two", "two")
        );
    dcc_component_v2_builder_t gallery =
        DCC_V2_MEDIA_GALLERY(
            DCC_V2_MEDIA("attachment://one.png", "One"),
            DCC_V2_MEDIA_SPOILER("attachment://two.png", "Two")
        );
    dcc_component_v2_builder_t thumbnail =
        DCC_V2_THUMBNAIL(DCC_V2_MEDIA("attachment://thumb.png", "Thumb"));
    dcc_component_v2_builder_t file =
        DCC_V2_FILE(DCC_V2_MEDIA("attachment://report.txt", "Report"));
    dcc_component_v2_builder_t string_select =
        DCC_V2_STRING_SELECT(
            "v2.string",
            DCC_SELECT_OPTION("Alpha", "alpha"),
            DCC_SELECT_OPTION("Beta", "beta")
        );
    dcc_component_v2_builder_t channel_select =
        DCC_V2_CHANNEL_SELECT(
            "v2.channel",
            DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(333ULL)),
            DCC_V2_CHANNEL_TYPES(0U, 5U)
        );
    dcc_component_v2_builder_t radio =
        DCC_V2_RADIO_GROUP(
            "v2.radio",
            DCC_SELECT_OPTION("Fast", "fast"),
            DCC_SELECT_OPTION("Safe", "safe")
        );
    dcc_component_v2_builder_t checkbox_group =
        DCC_V2_CHECKBOX_GROUP(
            "v2.checks",
            DCC_SELECT_OPTION("Logs", "logs"),
            DCC_SELECT_OPTION("Metrics", "metrics")
        );
    dcc_message_builder_t managed_message = DCC_MESSAGE_TEXT("managed");
    dcc_managed_message_options_t managed =
        DCC_MANAGED_MESSAGE_OPTIONS(333ULL, &managed_message, NULL, NULL, NULL);
    dcc_managed_message_options_t managed_keep =
        DCC_MANAGED_MESSAGE_KEEP_PREVIOUS_OPTIONS(444ULL, &managed_message, NULL, NULL, NULL);

    if (client.size != sizeof(client) ||
        client.intents != DCC_INTENT_GUILDS ||
        sharded_client.shard_id != 1U ||
        sharded_client.shard_count != 2U ||
        inferred_client.enable_cache != 1U ||
        inferred_client.infer_guild_id_from_channel != 1U ||
        (inferred_client.intents & DCC_INTENT_MESSAGE_CONTENT) == 0U ||
        (inferred_client.intents & DCC_INTENT_GUILD_PRESENCES) == 0U ||
        (inferred_client.intents & DCC_INTENT_DIRECT_MESSAGE_POLLS) == 0U ||
        registry.guild_id != 123ULL ||
        registry.dry_run != 1U ||
        registry_delete_stale.delete_stale != 1U ||
        registration.guild_id != 0U ||
        guild_registration.guild_id != 456ULL ||
        session_options.lock_user != 1U ||
        session_options.user_id != 789ULL ||
        session_check.guild_id != 222ULL ||
        listener.listen_buttons != 1U ||
        listener.listen_selects != 1U ||
        listener.listen_forms != 1U ||
        canary.canary_percent != 5U ||
        hot_reload.backend != DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER ||
        hot_reload.worker_canary_enabled != 1U ||
        firewall.invalid_request_hard_limit != DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT ||
        firewall_request.critical != 1U ||
        replay.kind != DCC_REPLAY_GATEWAY ||
        interaction_replay.kind != DCC_REPLAY_INTERACTION ||
        flow.auto_defer_ephemeral != 1U ||
        stickers.sticker_ids_count != 2U ||
        stickers.sticker_ids[1] != 222ULL ||
        poll.answer_count != 2U ||
        poll.duration_hours != 24U ||
        legacy_select.options_count != 2U ||
        legacy_select.options[1].has_default != 1U ||
        gallery.media_count != 2U ||
        gallery.media[1].has_spoiler != 1U ||
        thumbnail.media_count != 1U ||
        file.media_count != 1U ||
        string_select.options_count != 2U ||
        channel_select.default_value_count != 1U ||
        channel_select.channel_type_count != 2U ||
        radio.options_count != 2U ||
        checkbox_group.options_count != 2U ||
        managed.channel_id != 333ULL ||
        managed.message != &managed_message ||
        managed_keep.keep_previous != 1U) {
        fprintf(stderr, "sugar option macro field mismatch\n");
        return 1;
    }

    return 0;
}

int main(void) {
    if (run_sugar_message_smoke() != 0) {
        return 1;
    }
    if (run_sugar_legacy_component_smoke() != 0) {
        return 1;
    }
    if (run_sugar_component_v2_smoke() != 0) {
        return 1;
    }
    if (run_sugar_modal_smoke() != 0) {
        return 1;
    }
    if (run_sugar_command_smoke() != 0) {
        return 1;
    }
    if (run_sugar_array_compat_smoke() != 0) {
        return 1;
    }
    if (run_sugar_options_smoke() != 0) {
        return 1;
    }
    return 0;
}
