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
    dcc_embed_field_t fields[] = {
        DCC_EMBED_FIELD_INLINE("Shard", "0"),
    };
    dcc_embed_builder_t embeds[] = {
        DCC_EMBED_WITH_FIELDS("Status", "Gateway online", fields, DCC_ARRAY_LEN(fields)),
    };
    embeds[0].footer = DCC_EMBED_FOOTER("DCC", NULL);
    embeds[0].has_footer = 1U;
    embeds[0].image = DCC_EMBED_IMAGE("attachment://status.png");
    embeds[0].has_image = 1U;
    embeds[0].author = DCC_EMBED_AUTHOR("runtime", NULL, NULL);
    embeds[0].has_author = 1U;
    dcc_message_builder_t message =
        DCC_MESSAGE_TEXT_EMBEDS("ready", embeds, DCC_ARRAY_LEN(embeds));

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
    dcc_component_builder_t button =
        DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh");
    dcc_component_builder_t row = DCC_ACTION_ROW(&button, 1U);

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
    dcc_component_v2_builder_t title = DCC_V2_TEXT("# Runtime status");
    dcc_component_v2_builder_t refresh =
        DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh");
    dcc_component_v2_builder_t section =
        DCC_V2_SECTION(&title, 1U, &refresh);
    dcc_component_v2_builder_t separator = DCC_V2_SEPARATOR_SMALL();
    dcc_component_v2_builder_t children[] = {
        section,
        separator,
    };
    dcc_component_v2_builder_t container =
        DCC_V2_CONTAINER_ACCENT(children, DCC_ARRAY_LEN(children), 0x5865F2U);

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

    dcc_message_builder_t message = DCC_MESSAGE_COMPONENTS_V2(&container, 1U);
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
    dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("upload.config");
    upload.min_values = 1U;
    upload.max_values = 2U;
    upload.required = 1U;
    upload.has_min_values = 1U;
    upload.has_max_values = 1U;
    upload.has_required = 1U;

    dcc_component_v2_builder_t label = DCC_V2_LABEL("Config files", &upload);
    dcc_modal_builder_t modal =
        DCC_MODAL_V2_BUILDER("upload-modal", "Upload config", &label, 1U);

    char *json = NULL;
    if (dcc_modal_builder_build_json(&modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 modal\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"custom_id\":\"upload-modal\"") ||
        require_contains(json, "\"components\":[{\"type\":18") ||
        require_contains(json, "\"component\":{\"type\":19");
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

static int run_sugar_options_smoke(void) {
    dcc_client_options_t client = DCC_CLIENT_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_client_options_t sharded_client =
        DCC_CLIENT_SHARDED_OPTIONS("token", DCC_INTENTS_MESSAGES, 1U, 2U);
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

    if (client.size != sizeof(client) ||
        client.intents != DCC_INTENT_GUILDS ||
        sharded_client.shard_id != 1U ||
        sharded_client.shard_count != 2U ||
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
        flow.auto_defer_ephemeral != 1U) {
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
    if (run_sugar_options_smoke() != 0) {
        return 1;
    }
    return 0;
}
