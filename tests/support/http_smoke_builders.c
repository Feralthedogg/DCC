#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_message_builder_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    dcc_snowflake_t sticker_ids[] = {777, 778};
    dcc_embed_field_t embed_fields[] = {
        {
            .name = "field one",
            .value = "value \"one\"",
            .inline_field = 1,
            .has_inline = 1,
        },
        {
            .name = "field two",
            .value = "value two",
        },
    };
    dcc_embed_builder_t embed;
    dcc_embed_builder_init(&embed);
    if (dcc_embed_builder_set_title(&embed, "embed \"title\"") != DCC_OK ||
        dcc_embed_builder_set_description(&embed, "line one\nline two") != DCC_OK ||
        dcc_embed_builder_set_url(&embed, "https://example.com/embed") != DCC_OK ||
        dcc_embed_builder_set_timestamp(&embed, "2026-06-16T00:00:00.000000+00:00") != DCC_OK ||
        dcc_embed_builder_set_color(&embed, 1122867) != DCC_OK ||
        dcc_embed_builder_set_footer(&embed, "footer", "https://example.com/footer.png") != DCC_OK ||
        dcc_embed_builder_set_image(&embed, "https://example.com/image.png") != DCC_OK ||
        dcc_embed_builder_set_thumbnail(&embed, "https://example.com/thumb.png") != DCC_OK ||
        dcc_embed_builder_set_author(&embed, "author", "https://example.com/author", "https://example.com/author.png") != DCC_OK ||
        dcc_embed_builder_set_fields(&embed, embed_fields, sizeof(embed_fields) / sizeof(embed_fields[0])) != DCC_OK) {
        fprintf(stderr, "failed to build embed builder state\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_component_emoji_t button_emoji = {
        .name = "wave",
        .id = 888,
        .has_id = 1,
    };
    dcc_component_builder_t buttons[2];
    dcc_component_builder_init(&buttons[0], DCC_COMPONENT_BUTTON);
    dcc_component_builder_init(&buttons[1], DCC_COMPONENT_BUTTON);
    if (dcc_component_builder_set_style(&buttons[0], DCC_BUTTON_PRIMARY) != DCC_OK ||
        dcc_component_builder_set_label(&buttons[0], "Launch") != DCC_OK ||
        dcc_component_builder_set_emoji(&buttons[0], &button_emoji) != DCC_OK ||
        dcc_component_builder_set_custom_id(&buttons[0], "launch") != DCC_OK ||
        dcc_component_builder_set_disabled(&buttons[0], 0) != DCC_OK ||
        dcc_component_builder_set_style(&buttons[1], DCC_BUTTON_LINK) != DCC_OK ||
        dcc_component_builder_set_label(&buttons[1], "Docs") != DCC_OK ||
        dcc_component_builder_set_url(&buttons[1], "https://example.com/docs") != DCC_OK) {
        fprintf(stderr, "failed to build button component state\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_component_builder_t row;
    dcc_component_builder_init(&row, DCC_COMPONENT_ACTION_ROW);
    if (dcc_component_builder_set_children(&row, buttons, sizeof(buttons) / sizeof(buttons[0])) != DCC_OK) {
        fprintf(stderr, "failed to build action row component state\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_select_option_t select_options[] = {
        {
            .label = "Red",
            .value = "red",
            .description = "Pick red",
            .is_default = 1,
            .has_default = 1,
        },
        {
            .label = "Blue",
            .value = "blue",
        },
    };
    dcc_component_builder_t select;
    dcc_component_builder_init(&select, DCC_COMPONENT_STRING_SELECT);
    if (dcc_component_builder_set_custom_id(&select, "select-color") != DCC_OK ||
        dcc_component_builder_set_options(
            &select,
            select_options,
            sizeof(select_options) / sizeof(select_options[0])
        ) != DCC_OK ||
        dcc_component_builder_set_placeholder(&select, "Pick") != DCC_OK ||
        dcc_component_builder_set_min_values(&select, 1) != DCC_OK ||
        dcc_component_builder_set_max_values(&select, 1) != DCC_OK) {
        fprintf(stderr, "failed to build select component state\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_component_builder_t components[] = { row, select };
    dcc_poll_answer_t poll_answers[] = {
        {
            .media = {
                .text = "Alpha \"A\"",
                .emoji = {
                    .name = "one",
                },
                .has_emoji = 1,
            },
        },
        {
            .media = {
                .text = "Beta",
                .emoji = {
                    .name = "two",
                    .id = 889,
                    .animated = 1,
                    .has_id = 1,
                    .has_animated = 1,
                },
                .has_emoji = 1,
            },
        },
    };
    dcc_poll_builder_t poll = {
        .size = sizeof(poll),
        .question = {
            .text = "Pick \"one\"",
        },
        .answers = poll_answers,
        .answer_count = sizeof(poll_answers) / sizeof(poll_answers[0]),
        .duration_hours = 48,
        .layout_type = DCC_POLL_LAYOUT_DEFAULT,
        .allow_multiselect = 1,
        .has_duration = 1,
        .has_layout_type = 1,
        .has_allow_multiselect = 1,
    };
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    if (dcc_message_builder_set_content(&message, "hello \"json\"\nnext") != DCC_OK ||
        dcc_message_builder_set_tts(&message, 1) != DCC_OK ||
        dcc_message_builder_set_flags(&message, 64) != DCC_OK ||
        dcc_message_builder_set_nonce(&message, "nonce-1") != DCC_OK ||
        dcc_message_builder_set_sticker_ids(&message, sticker_ids, sizeof(sticker_ids) / sizeof(sticker_ids[0])) != DCC_OK ||
        dcc_message_builder_set_message_reference_json(&message, "{\"message_id\":\"111\"}") != DCC_OK ||
        dcc_message_builder_set_allowed_mentions_json(&message, "{\"parse\":[]}") != DCC_OK ||
        dcc_message_builder_set_embeds(&message, &embed, 1) != DCC_OK ||
        dcc_message_builder_set_components(&message, components, sizeof(components) / sizeof(components[0])) != DCC_OK ||
        dcc_message_builder_set_attachments_json(&message, "[{\"id\":\"0\",\"filename\":\"hello.txt\"}]") != DCC_OK ||
        dcc_message_builder_set_poll(&message, &poll) != DCC_OK) {
        fprintf(stderr, "failed to build message builder state\n");
        dcc_client_destroy(client);
        return 1;
    }

    const char expected_body[] =
        "{\"content\":\"hello \\\"json\\\"\\nnext\","
        "\"tts\":true,"
        "\"flags\":64,"
        "\"nonce\":\"nonce-1\","
        "\"sticker_ids\":[\"777\",\"778\"],"
        "\"message_reference\":{\"message_id\":\"111\"},"
        "\"allowed_mentions\":{\"parse\":[]},"
        "\"embeds\":[{"
        "\"title\":\"embed \\\"title\\\"\","
        "\"description\":\"line one\\nline two\","
        "\"url\":\"https://example.com/embed\","
        "\"timestamp\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"color\":1122867,"
        "\"footer\":{\"text\":\"footer\",\"icon_url\":\"https://example.com/footer.png\"},"
        "\"image\":{\"url\":\"https://example.com/image.png\"},"
        "\"thumbnail\":{\"url\":\"https://example.com/thumb.png\"},"
        "\"author\":{\"name\":\"author\",\"url\":\"https://example.com/author\",\"icon_url\":\"https://example.com/author.png\"},"
        "\"fields\":["
        "{\"name\":\"field one\",\"value\":\"value \\\"one\\\"\",\"inline\":true},"
        "{\"name\":\"field two\",\"value\":\"value two\"}"
        "]}],"
        "\"components\":["
        "{\"type\":1,\"components\":["
        "{\"type\":2,\"style\":1,\"label\":\"Launch\",\"emoji\":{\"id\":\"888\",\"name\":\"wave\"},\"custom_id\":\"launch\",\"disabled\":false},"
        "{\"type\":2,\"style\":5,\"label\":\"Docs\",\"url\":\"https://example.com/docs\"}"
        "]},"
        "{\"type\":3,\"custom_id\":\"select-color\",\"options\":["
        "{\"label\":\"Red\",\"value\":\"red\",\"description\":\"Pick red\",\"default\":true},"
        "{\"label\":\"Blue\",\"value\":\"blue\"}"
        "],\"placeholder\":\"Pick\",\"min_values\":1,\"max_values\":1}"
        "],"
        "\"attachments\":[{\"id\":\"0\",\"filename\":\"hello.txt\"}],"
        "\"poll\":{"
        "\"question\":{\"text\":\"Pick \\\"one\\\"\"},"
        "\"answers\":["
        "{\"poll_media\":{\"text\":\"Alpha \\\"A\\\"\",\"emoji\":{\"name\":\"one\"}}},"
        "{\"poll_media\":{\"text\":\"Beta\",\"emoji\":{\"id\":\"889\",\"name\":\"two\",\"animated\":true}}}"
        "],"
        "\"duration\":48,"
        "\"allow_multiselect\":true,"
        "\"layout_type\":1"
        "}}";

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message builder server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_message_builder(client, 222, &message, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/channels/222/messages") != 0 ||
        strcmp(server.content_type, "application/json") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected message builder request: st=%s method=%s path=%s content_type=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.content_type,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message builder edit server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_message_builder(client, 222, 333, &message, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PATCH") != 0 ||
        strcmp(server.path, "/channels/222/messages/333") != 0 ||
        strcmp(server.content_type, "application/json") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected message builder edit request: st=%s method=%s path=%s content_type=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.content_type,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    const char file_data[] = "builder-file";
    dcc_rest_multipart_file_t file = {
        .field_name = "files[0]",
        .filename = "builder.txt",
        .content_type = "text/plain",
        .data = file_data,
        .data_len = sizeof(file_data) - 1U,
    };

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message builder create multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_message_builder_multipart(client, 222, &message, &file, 1, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "POST",
            "/channels/222/messages",
            expected_body,
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"builder.txt\"\r\nContent-Type: text/plain\r\n\r\nbuilder-file")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message builder multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_message_builder_multipart(client, 222, 333, &message, &file, 1, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/channels/222/messages/333",
            expected_body,
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"builder.txt\"\r\nContent-Type: text/plain\r\n\r\nbuilder-file")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}

#endif
