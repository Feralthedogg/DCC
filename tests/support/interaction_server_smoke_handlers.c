#include "interaction_server_smoke_support.h"

#if LLAM_PLATFORM_POSIX

#include <string.h>
#include <unistd.h>

void interaction_health_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    interaction_seen_t *seen = (interaction_seen_t *)user_data;
    (void)server;
    seen->health_seen++;
    if (strcmp(dcc_interaction_request_method(request), "GET") != 0 ||
        strcmp(dcc_interaction_request_path(request), "/healthz") != 0 ||
        dcc_interaction_request_interaction(request) != NULL ||
        dcc_interaction_request_is_ping(request) != 0U ||
        dcc_interaction_request_reply_text(request, 200, "ok") != DCC_OK) {
        seen->failed = 1;
    }
}

void interaction_prefix_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    interaction_seen_t *seen = (interaction_seen_t *)user_data;
    (void)server;
    seen->prefix_seen++;
    if (strcmp(dcc_interaction_request_method(request), "GET") != 0 ||
        strcmp(dcc_interaction_request_path(request), "/ops/check") != 0 ||
        dcc_interaction_request_interaction(request) != NULL ||
        dcc_interaction_request_reply_text(request, 200, "ops") != DCC_OK) {
        seen->failed = 1;
    }
}

void interaction_shadow_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    interaction_seen_t *seen = (interaction_seen_t *)user_data;
    (void)server;
    (void)request;
    seen->shadow_seen++;
    if (dcc_interaction_request_reply_text(request, 200, "shadow") != DCC_OK) {
        seen->failed = 1;
    }
}

void interaction_slow_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    interaction_seen_t *seen = (interaction_seen_t *)user_data;
    (void)server;
    seen->slow_seen++;
    if (strcmp(dcc_interaction_request_method(request), "GET") != 0 ||
        strcmp(dcc_interaction_request_path(request), "/slow") != 0 ||
        dcc_interaction_request_interaction(request) != NULL) {
        seen->failed = 1;
    }
    atomic_store_explicit(&seen->slow_entered, 1U, memory_order_release);
    while (atomic_load_explicit(&seen->slow_release, memory_order_acquire) == 0U) {
        usleep(1000U);
    }
    if (dcc_interaction_request_reply_text(request, 200, "slow") != DCC_OK) {
        seen->failed = 1;
    }
}

static dcc_status_t interaction_reply_smoke_modal(dcc_interaction_request_t *request) {
    dcc_modal_builder_t modal;
    dcc_modal_builder_init(&modal);
    dcc_status_t status = dcc_modal_builder_set_custom_id(&modal, "feedback-modal");
    if (status == DCC_OK) {
        status = dcc_modal_builder_set_title(&modal, "Feedback");
    }
    if (status == DCC_OK) {
        status = dcc_modal_builder_set_components_json(&modal, "[]");
    }
    if (status == DCC_OK) {
        status = dcc_interaction_request_reply_modal(request, &modal);
    }
    return status;
}

static dcc_status_t interaction_reply_smoke_autocomplete(dcc_interaction_request_t *request) {
    dcc_autocomplete_choice_t choice;
    dcc_autocomplete_choice_init(&choice, "Red");
    dcc_status_t status = dcc_autocomplete_choice_set_string_value(&choice, "red");

    dcc_autocomplete_builder_t autocomplete;
    dcc_autocomplete_builder_init(&autocomplete);
    if (status == DCC_OK) {
        status = dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1);
    }
    if (status == DCC_OK) {
        status = dcc_interaction_request_reply_autocomplete(request, &autocomplete);
    }
    return status;
}

void interaction_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    interaction_seen_t *seen = (interaction_seen_t *)user_data;
    (void)server;
    seen->called++;

    if (strcmp(dcc_interaction_request_method(request), "POST") != 0 ||
        strcmp(dcc_interaction_request_path(request), "/interactions") != 0 ||
        strcmp(dcc_interaction_request_header(request, "x-test"), "yes") != 0 ||
        dcc_interaction_request_body(request) == NULL ||
        dcc_interaction_request_body_len(request) == 0U) {
        seen->failed = 1;
        return;
    }

    size_t header_count = 0;
    const dcc_interaction_http_header_t *headers = dcc_interaction_request_headers(request, &header_count);
    if (headers == NULL || header_count == 0U) {
        seen->failed = 1;
        return;
    }

    const dcc_interaction_t *interaction = dcc_interaction_request_interaction(request);
    if (interaction == NULL) {
        seen->failed = 1;
        return;
    }

    if (dcc_interaction_request_is_ping(request)) {
        if (interaction->type != 1U) {
            seen->failed = 1;
        }
        seen->ping_seen = 1;
        if (dcc_interaction_request_reply_type(request, (dcc_interaction_response_type_t)0) !=
                DCC_ERR_INVALID_ARG ||
            dcc_interaction_request_reply_pong(request) != DCC_OK) {
            seen->failed = 1;
        }
        return;
    }

    if (dcc_interaction_request_event_type(request) == DCC_EVENT_BUTTON_CLICK) {
        if (interaction->id != 561U ||
            interaction->application_id != 123U ||
            interaction->type != 3U ||
            interaction->component_type != 2U ||
            interaction->token == NULL ||
            strcmp(interaction->token, "tok/en") != 0 ||
            interaction->custom_id == NULL ||
            strcmp(interaction->custom_id, "go") != 0) {
            seen->failed = 1;
            return;
        }
        seen->button_seen = 1;
        if (dcc_interaction_request_reply_update_message_text(request, NULL) != DCC_ERR_INVALID_ARG ||
            dcc_interaction_request_reply_update_message_text(request, "updated") != DCC_OK) {
            seen->failed = 1;
        }
        return;
    }

    if (dcc_interaction_request_event_type(request) == DCC_EVENT_AUTOCOMPLETE) {
        if (interaction->id != 560U ||
            interaction->application_id != 123U ||
            interaction->type != 4U ||
            interaction->token == NULL ||
            strcmp(interaction->token, "tok/en") != 0) {
            seen->failed = 1;
            return;
        }
        seen->autocomplete_seen = 1;
        if (dcc_interaction_request_reply_autocomplete(request, NULL) != DCC_ERR_INVALID_ARG ||
            interaction_reply_smoke_autocomplete(request) != DCC_OK) {
            seen->failed = 1;
        }
        return;
    }

    if (dcc_interaction_request_event_type(request) != DCC_EVENT_SLASH_COMMAND ||
        interaction->application_id != 123U ||
        interaction->type != 2U ||
        interaction->command_type != 1U ||
        interaction->token == NULL ||
        strcmp(interaction->token, "tok/en") != 0 ||
        interaction->name == NULL) {
        seen->failed = 1;
        return;
    }

    if (strcmp(interaction->name, "secret") == 0) {
        if (interaction->id != 562U || interaction->command_id != 780U) {
            seen->failed = 1;
            return;
        }
        seen->ephemeral_seen = 1;
        if (dcc_interaction_request_reply_ephemeral_message_text(request, NULL) != DCC_ERR_INVALID_ARG ||
            dcc_interaction_request_reply_ephemeral_message_text(request, "secret") != DCC_OK) {
            seen->failed = 1;
        }
        return;
    }

    if (strcmp(interaction->name, "modal") == 0) {
        if (interaction->id != 559U || interaction->command_id != 778U) {
            seen->failed = 1;
            return;
        }
        seen->modal_seen = 1;
        if (dcc_interaction_request_reply_modal(request, NULL) != DCC_ERR_INVALID_ARG ||
            interaction_reply_smoke_modal(request) != DCC_OK) {
            seen->failed = 1;
        }
        return;
    }

    if (interaction->id != 558U ||
        interaction->command_id != 777U ||
        strcmp(interaction->name, "ping") != 0) {
        seen->failed = 1;
        return;
    }
    seen->slash_seen = 1;

    if (dcc_interaction_request_reply_message_builder(
            request,
            (dcc_interaction_response_type_t)0,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_request_reply_message_text(request, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_request_reply_message_text(request, "pong") != DCC_OK) {
        seen->failed = 1;
    }
}

void *interaction_wait_thread(void *arg) {
    interaction_wait_state_t *state = (interaction_wait_state_t *)arg;
    state->status = dcc_interaction_server_wait(state->server);
    return NULL;
}

void *interaction_slow_client_thread(void *arg) {
    interaction_slow_client_state_t *state = (interaction_slow_client_state_t *)arg;
    state->ok = interaction_send_request(state->port, "GET", "/slow", "", NULL, NULL, &state->response);
    return NULL;
}

#endif
