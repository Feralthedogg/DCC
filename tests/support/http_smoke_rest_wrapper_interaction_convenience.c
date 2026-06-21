#include "http_smoke_rest_wrapper_interactions_internal.h"
#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int build_convenience_modal(
    dcc_modal_builder_t *modal,
    dcc_component_builder_t *text_input,
    dcc_component_builder_t *modal_row
) {
    dcc_component_builder_init(text_input, DCC_COMPONENT_TEXT_INPUT);
    if (dcc_component_builder_set_custom_id(text_input, "feedback") != DCC_OK ||
        dcc_component_builder_set_text_input_style(text_input, DCC_TEXT_INPUT_PARAGRAPH) != DCC_OK ||
        dcc_component_builder_set_label(text_input, "Feedback") != DCC_OK ||
        dcc_component_builder_set_min_length(text_input, 3) != DCC_OK ||
        dcc_component_builder_set_max_length(text_input, 1000) != DCC_OK ||
        dcc_component_builder_set_required(text_input, 1) != DCC_OK ||
        dcc_component_builder_set_value(text_input, "seed") != DCC_OK ||
        dcc_component_builder_set_placeholder(text_input, "Tell us") != DCC_OK) {
        return 0;
    }

    dcc_component_builder_init(modal_row, DCC_COMPONENT_ACTION_ROW);
    if (dcc_component_builder_set_children(modal_row, text_input, 1) != DCC_OK) {
        return 0;
    }

    dcc_modal_builder_init(modal);
    return dcc_modal_builder_set_custom_id(modal, "feedback-modal") == DCC_OK &&
        dcc_modal_builder_set_title(modal, "Feedback") == DCC_OK &&
        dcc_modal_builder_set_components(modal, modal_row, 1) == DCC_OK;
}

static int build_convenience_autocomplete(
    dcc_autocomplete_builder_t *autocomplete,
    dcc_autocomplete_choice_t choices[3]
) {
    dcc_autocomplete_choice_init(&choices[0], "Red");
    dcc_autocomplete_choice_init(&choices[1], "Limit");
    dcc_autocomplete_choice_init(&choices[2], "Ratio");
    if (dcc_autocomplete_choice_set_string_value(&choices[0], "red") != DCC_OK ||
        dcc_autocomplete_choice_set_name_localizations_json(&choices[0], "{\"ko\":\"red-ko\"}") != DCC_OK ||
        dcc_autocomplete_choice_set_integer_value(&choices[1], 42) != DCC_OK ||
        dcc_autocomplete_choice_set_number_value(&choices[2], 3.5) != DCC_OK) {
        return 0;
    }

    dcc_autocomplete_builder_init(autocomplete);
    return dcc_autocomplete_builder_set_choices(autocomplete, choices, 3) == DCC_OK;
}

int run_public_rest_wrapper_interaction_convenience_smoke(dcc_client_t *client) {
    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;
    dcc_status_t st;

    dcc_interaction_t interaction = {
        .id = 558,
        .application_id = 123,
        .token = "tok/en",
    };
    const char interaction_file_data[] = "interaction-file";
    dcc_rest_multipart_file_t interaction_file = {
        .field_name = "files[0]",
        .filename = "interaction.txt",
        .content_type = "text/plain",
        .data = interaction_file_data,
        .data_len = sizeof(interaction_file_data) - 1U,
    };

    dcc_component_builder_t text_input;
    dcc_component_builder_t modal_row;
    dcc_autocomplete_choice_t choices[3];
    dcc_modal_builder_t modal;
    dcc_autocomplete_builder_t autocomplete;
    if (!build_convenience_modal(&modal, &text_input, &modal_row) ||
        !build_convenience_autocomplete(&autocomplete, choices)) {
        fprintf(stderr, "failed to build interaction convenience state\n");
        return 1;
    }

#define EXPECT_INTERACTION_CONVENIENCE(label, expected_method, expected_path, expected_body, call_expr) \
    do { \
        if (start_server(&server, &thread) != 0) { \
            fprintf(stderr, "failed to start %s server: %s\n", (label), strerror(errno)); \
            return 1; \
        } \
        set_api_base_for_server(&server); \
        memset(&seen, 0, sizeof(seen)); \
        st = (call_expr); \
        (void)pthread_join(thread, NULL); \
        close(server.fd); \
        if (st != DCC_OK || \
            !seen.called || \
            strcmp(server.method, (expected_method)) != 0 || \
            strcmp(server.path, (expected_path)) != 0 || \
            ((expected_body) != NULL && strcmp(server.body, (expected_body)) != 0)) { \
            fprintf(stderr, "unexpected %s request: st=%s method=%s path=%s body=%s\n", \
                    (label), \
                    dcc_status_string(st), \
                    server.method, \
                    server.path, \
                    server.body); \
            return 1; \
        } \
    } while (0)

#define EXPECT_INTERACTION_MULTIPART(label, expected_method, expected_path, expected_body, call_expr) \
    do { \
        if (start_server(&server, &thread) != 0) { \
            fprintf(stderr, "failed to start %s server: %s\n", (label), strerror(errno)); \
            return 1; \
        } \
        set_api_base_for_server(&server); \
        memset(&seen, 0, sizeof(seen)); \
        st = (call_expr); \
        (void)pthread_join(thread, NULL); \
        close(server.fd); \
        if (st != DCC_OK || \
            !seen.called || \
            !expect_multipart_request( \
                &server, \
                (expected_method), \
                (expected_path), \
                (expected_body), \
                "name=\"payload_json\"", \
                "name=\"files[0]\"; filename=\"interaction.txt\"\r\nContent-Type: text/plain\r\n\r\ninteraction-file")) { \
            return 1; \
        } \
    } while (0)

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction convenience callback server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_from_interaction(
        client,
        &interaction,
        "{\"type\":5}",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/558/tok%2Fen/callback") != 0 ||
        strcmp(server.body, "{\"type\":5}") != 0) {
        fprintf(stderr, "unexpected interaction convenience callback request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience type callback",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":6}",
        dcc_rest_interaction_response_create_type_from_interaction(
            client,
            &interaction,
            DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience deferred message callback",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":5}",
        dcc_rest_interaction_response_create_deferred_message_from_interaction(
            client,
            &interaction,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "direct interaction ephemeral text",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":4,\"data\":{\"content\":\"Saved\",\"flags\":64}}",
        dcc_interaction_reply_ephemeral_text(
            client,
            &interaction,
            "Saved",
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "direct interaction error embed",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":4,\"data\":{\"flags\":64,\"embeds\":[{\"title\":\"Invalid\","
        "\"description\":\"Use YYYYMMDD\",\"color\":15548997}]}}",
        dcc_interaction_reply_error(
            client,
            &interaction,
            "Invalid",
            "Use YYYYMMDD",
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience modal callback",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":9,\"data\":{\"custom_id\":\"feedback-modal\",\"title\":\"Feedback\","
        "\"components\":[{\"type\":1,\"components\":[{\"type\":4,\"custom_id\":\"feedback\","
        "\"style\":2,\"label\":\"Feedback\",\"min_length\":3,\"max_length\":1000,"
        "\"required\":true,\"value\":\"seed\",\"placeholder\":\"Tell us\"}]}]}}",
        dcc_rest_interaction_response_create_modal_from_interaction(
            client,
            &interaction,
            &modal,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience autocomplete callback",
        "POST",
        "/interactions/558/tok%2Fen/callback",
        "{\"type\":8,\"data\":{\"choices\":["
        "{\"name\":\"Red\",\"name_localizations\":{\"ko\":\"red-ko\"},\"value\":\"red\"},"
        "{\"name\":\"Limit\",\"value\":42},"
        "{\"name\":\"Ratio\",\"value\":3.5}"
        "]}}",
        dcc_rest_interaction_response_create_autocomplete_from_interaction(
            client,
            &interaction,
            &autocomplete,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience original get",
        "GET",
        "/webhooks/123/tok%2Fen/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_get_from_interaction(
            client,
            &interaction,
            rest_cb,
            &seen
        )
    );

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction convenience original edit server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_original_response_edit_from_interaction(
        client,
        &interaction,
        "{\"content\":\"edited\"}",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PATCH") != 0 ||
        strcmp(server.path, "/webhooks/123/tok%2Fen/messages/@original") != 0 ||
        strcmp(server.body, "{\"content\":\"edited\"}") != 0) {
        fprintf(stderr, "unexpected interaction convenience original edit request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    EXPECT_INTERACTION_MULTIPART(
        "interaction convenience original multipart edit",
        "PATCH",
        "/webhooks/123/tok%2Fen/messages/@original",
        "{\"content\":\"original file\"}",
        dcc_rest_interaction_original_response_edit_multipart_from_interaction(
            client,
            &interaction,
            "{\"content\":\"original file\"}",
            &interaction_file,
            1,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience original delete",
        "DELETE",
        "/webhooks/123/tok%2Fen/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_delete_from_interaction(
            client,
            &interaction,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience followup create",
        "POST",
        "/webhooks/123/tok%2Fen",
        "{\"content\":\"new followup\"}",
        dcc_rest_interaction_followup_create_from_interaction(
            client,
            &interaction,
            "{\"content\":\"new followup\"}",
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_MULTIPART(
        "interaction convenience followup create multipart",
        "POST",
        "/webhooks/123/tok%2Fen",
        "{\"content\":\"new followup file\"}",
        dcc_rest_interaction_followup_create_multipart_from_interaction(
            client,
            &interaction,
            "{\"content\":\"new followup file\"}",
            &interaction_file,
            1,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience followup get",
        "GET",
        "/webhooks/123/tok%2Fen/messages/999",
        NULL,
        dcc_rest_interaction_followup_get_from_interaction(
            client,
            &interaction,
            999,
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_CONVENIENCE(
        "interaction convenience followup edit",
        "PATCH",
        "/webhooks/123/tok%2Fen/messages/999",
        "{\"content\":\"followup edited\"}",
        dcc_rest_interaction_followup_edit_from_interaction(
            client,
            &interaction,
            999,
            "{\"content\":\"followup edited\"}",
            rest_cb,
            &seen
        )
    );

    EXPECT_INTERACTION_MULTIPART(
        "interaction convenience followup edit multipart",
        "PATCH",
        "/webhooks/123/tok%2Fen/messages/999",
        "{\"content\":\"followup file edited\"}",
        dcc_rest_interaction_followup_edit_multipart_from_interaction(
            client,
            &interaction,
            999,
            "{\"content\":\"followup file edited\"}",
            &interaction_file,
            1,
            rest_cb,
            &seen
        )
    );

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction convenience followup delete server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_followup_delete_from_interaction(client, &interaction, 999, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "DELETE") != 0 ||
        strcmp(server.path, "/webhooks/123/tok%2Fen/messages/999") != 0) {
        fprintf(stderr, "unexpected interaction convenience followup delete request: st=%s method=%s path=%s\n",
                dcc_status_string(st),
                server.method,
                server.path);
        return 1;
    }

#undef EXPECT_INTERACTION_MULTIPART
#undef EXPECT_INTERACTION_CONVENIENCE

    return 0;
}

#endif
