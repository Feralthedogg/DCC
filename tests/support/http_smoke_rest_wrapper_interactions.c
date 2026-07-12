#include "http_smoke_rest_wrapper_interactions_internal.h"
#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_wrapper_interactions_smoke(dcc_client_t *client) {
    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;
    dcc_status_t st;

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction callback server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create(
        client,
        555,
        "tok/en",
        "{\"type\":4,\"data\":{\"content\":\"pong\"}}",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/555/tok%2Fen/callback") != 0 ||
        strcmp(server.body, "{\"type\":4,\"data\":{\"content\":\"pong\"}}") != 0) {
        fprintf(stderr, "unexpected interaction callback request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction callback response server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_options(
        client,
        555,
        "tok/en",
        "{\"type\":12}",
        1U,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/555/tok%2Fen/callback?with_response=true") != 0 ||
        strcmp(server.body, "{\"type\":12}") != 0 ||
        dcc_rest_interaction_response_create_options(
            client,
            555,
            "tok/en",
            "{\"type\":12}",
            2U,
            rest_cb,
            &seen
        ) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unexpected interaction callback with_response request\n");
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction callback type server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_type(
        client,
        555,
        "tok/en",
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/555/tok%2Fen/callback") != 0 ||
        strcmp(server.body, "{\"type\":5}") != 0 ||
        dcc_rest_interaction_response_create_type(
            client,
            555,
            "tok/en",
            (dcc_interaction_response_type_t)0,
            rest_cb,
            &seen
        ) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unexpected interaction callback type request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction callback pong server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_pong(client, 555, "tok/en", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/555/tok%2Fen/callback") != 0 ||
        strcmp(server.body, "{\"type\":1}") != 0) {
        fprintf(stderr, "unexpected interaction callback pong request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    dcc_message_builder_t interaction_message;
    dcc_message_builder_init(&interaction_message);
    st = dcc_message_builder_set_content(&interaction_message, "pong builder");
    if (st != DCC_OK) {
        fprintf(stderr, "failed to build interaction message: %s\n", dcc_status_string(st));
        return 1;
    }
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction callback builder server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_message_builder(
        client,
        555,
        "tok/en",
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        &interaction_message,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/555/tok%2Fen/callback") != 0 ||
        strcmp(server.body, "{\"type\":4,\"data\":{\"content\":\"pong builder\"}}") != 0) {
        fprintf(stderr, "unexpected interaction callback builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    dcc_component_builder_t text_input;
    dcc_component_builder_init(&text_input, DCC_COMPONENT_TEXT_INPUT);
    if (dcc_component_builder_set_custom_id(&text_input, "feedback") != DCC_OK ||
        dcc_component_builder_set_text_input_style(&text_input, DCC_TEXT_INPUT_PARAGRAPH) != DCC_OK ||
        dcc_component_builder_set_label(&text_input, "Feedback") != DCC_OK ||
        dcc_component_builder_set_min_length(&text_input, 3) != DCC_OK ||
        dcc_component_builder_set_max_length(&text_input, 1000) != DCC_OK ||
        dcc_component_builder_set_required(&text_input, 1) != DCC_OK ||
        dcc_component_builder_set_value(&text_input, "seed") != DCC_OK ||
        dcc_component_builder_set_placeholder(&text_input, "Tell us") != DCC_OK) {
        fprintf(stderr, "failed to build modal text input state\n");
        return 1;
    }

    dcc_component_builder_t modal_row;
    dcc_component_builder_init(&modal_row, DCC_COMPONENT_ACTION_ROW);
    if (dcc_component_builder_set_children(&modal_row, &text_input, 1) != DCC_OK) {
        fprintf(stderr, "failed to build modal action row state\n");
        return 1;
    }

    dcc_modal_builder_t modal;
    dcc_modal_builder_init(&modal);
    if (dcc_modal_builder_set_custom_id(&modal, "feedback-modal") != DCC_OK ||
        dcc_modal_builder_set_title(&modal, "Feedback") != DCC_OK ||
        dcc_modal_builder_set_components(&modal, &modal_row, 1) != DCC_OK) {
        fprintf(stderr, "failed to build modal builder state\n");
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start modal interaction callback server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_modal(client, 556, "tok/en", &modal, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/556/tok%2Fen/callback") != 0 ||
        strcmp(
            server.body,
            "{\"type\":9,\"data\":{\"custom_id\":\"feedback-modal\",\"title\":\"Feedback\","
            "\"components\":[{\"type\":1,\"components\":[{\"type\":4,\"custom_id\":\"feedback\","
            "\"style\":2,\"label\":\"Feedback\",\"min_length\":3,\"max_length\":1000,"
            "\"required\":true,\"value\":\"seed\",\"placeholder\":\"Tell us\"}]}]}}"
        ) != 0) {
        fprintf(stderr, "unexpected modal interaction callback request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    dcc_autocomplete_choice_t choices[3];
    dcc_autocomplete_choice_init(&choices[0], "Red");
    dcc_autocomplete_choice_init(&choices[1], "Limit");
    dcc_autocomplete_choice_init(&choices[2], "Ratio");
    if (dcc_autocomplete_choice_set_string_value(&choices[0], "red") != DCC_OK ||
        dcc_autocomplete_choice_set_name_localizations_json(&choices[0], "{\"ko\":\"red-ko\"}") != DCC_OK ||
        dcc_autocomplete_choice_set_integer_value(&choices[1], 42) != DCC_OK ||
        dcc_autocomplete_choice_set_number_value(&choices[2], 3.5) != DCC_OK) {
        fprintf(stderr, "failed to build autocomplete choices\n");
        return 1;
    }

    dcc_autocomplete_builder_t autocomplete;
    dcc_autocomplete_builder_init(&autocomplete);
    if (dcc_autocomplete_builder_set_choices(&autocomplete, choices, sizeof(choices) / sizeof(choices[0])) != DCC_OK) {
        fprintf(stderr, "failed to build autocomplete response state\n");
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start autocomplete interaction callback server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_response_create_autocomplete(client, 557, "tok/en", &autocomplete, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/interactions/557/tok%2Fen/callback") != 0 ||
        strcmp(
            server.body,
            "{\"type\":8,\"data\":{\"choices\":["
            "{\"name\":\"Red\",\"name_localizations\":{\"ko\":\"red-ko\"},\"value\":\"red\"},"
            "{\"name\":\"Limit\",\"value\":42},"
            "{\"name\":\"Ratio\",\"value\":3.5}"
            "]}}"
        ) != 0) {
        fprintf(stderr, "unexpected autocomplete interaction callback request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction followup server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_followup_get(client, 123, "tok/en", 999, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "GET") != 0 ||
        strcmp(server.path, "/webhooks/123/tok%2Fen/messages/999") != 0) {
        fprintf(stderr, "unexpected interaction followup request: st=%s method=%s path=%s\n",
                dcc_status_string(st),
                server.method,
                server.path);
        return 1;
    }

    return run_public_rest_wrapper_interaction_convenience_smoke(client);
}

#endif
