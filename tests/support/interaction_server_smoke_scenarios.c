#include "interaction_server_smoke_support.h"

#if LLAM_PLATFORM_POSIX

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int interaction_configure_smoke_routes(dcc_interaction_server_t *server, interaction_seen_t *seen) {
    dcc_interaction_route_t health_route = {
        .size = sizeof(health_route),
        .method = "GET",
        .path = "/healthz",
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = interaction_health_callback,
        .user_data = seen,
    };
    dcc_interaction_route_t prefix_route = {
        .size = sizeof(prefix_route),
        .method = "GET",
        .path = "/ops/",
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED | DCC_INTERACTION_ROUTE_F_PREFIX,
        .callback = interaction_prefix_callback,
        .user_data = seen,
    };
    dcc_interaction_route_t shadow_route = {
        .size = sizeof(shadow_route),
        .method = "POST",
        .path = "/inter",
        .flags = DCC_INTERACTION_ROUTE_F_PREFIX,
        .callback = interaction_shadow_callback,
        .user_data = seen,
    };
    dcc_interaction_route_t interaction_route = {
        .size = sizeof(interaction_route),
        .method = "POST",
        .path = "/interactions",
        .flags = 0,
        .callback = interaction_callback,
        .user_data = seen,
    };
    dcc_interaction_route_t slow_route = {
        .size = sizeof(slow_route),
        .method = "GET",
        .path = "/slow",
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = interaction_slow_callback,
        .user_data = seen,
    };
    dcc_interaction_route_t invalid_route = {
        .size = 0,
        .method = "GET",
        .path = "/bad",
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = interaction_health_callback,
        .user_data = seen,
    };
    if (dcc_interaction_server_add_route(NULL, &health_route) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_add_health_route(NULL, "/readyz") != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_add_route(server, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_add_route(server, &invalid_route) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_add_route(server, &health_route) != DCC_OK ||
        dcc_interaction_server_add_health_route(server, "/readyz") != DCC_OK ||
        dcc_interaction_server_add_route(server, &prefix_route) != DCC_OK ||
        dcc_interaction_server_add_route(server, &shadow_route) != DCC_OK ||
        dcc_interaction_server_add_route(server, &interaction_route) != DCC_OK ||
        dcc_interaction_server_add_route(server, &slow_route) != DCC_OK) {
        fprintf(stderr, "interaction route setup failed\n");
        return 0;
    }
    return 1;
}

int interaction_validate_start_state(dcc_interaction_server_t *server, uint16_t port) {
    dcc_interaction_server_state_t server_state = {
        .size = sizeof(server_state),
    };
    if (dcc_interaction_server_get_state(NULL, &server_state) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_get_state(server, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_get_state(server, &server_state) != DCC_OK ||
        server_state.size != sizeof(server_state) ||
        server_state.started != 1U ||
        server_state.stopping != 0U ||
        server_state.listening != 1U ||
        server_state.draining != 0U ||
        server_state.requested_port != 0U ||
        server_state.bound_port != port ||
        server_state.backlog != 128U ||
        server_state.route_count != 6U ||
        server_state.max_header_size != 1024U ||
        server_state.max_body_size != 128U ||
        server_state.accepted_connections != 0U ||
        server_state.active_requests != 0U ||
        server_state.completed_requests != 0U) {
        fprintf(
            stderr,
            "interaction start state mismatch: started=%u stopping=%u listening=%u draining=%u "
            "requested=%u bound=%u routes=%zu active=%llu completed=%llu\n",
            (unsigned)server_state.started,
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned)server_state.requested_port,
            (unsigned)server_state.bound_port,
            server_state.route_count,
            (unsigned long long)server_state.active_requests,
            (unsigned long long)server_state.completed_requests
        );
        return 0;
    }
    return 1;
}

static int expect_response(
    const char *label,
    const interaction_http_response_t *response,
    int status,
    const char *body
) {
    if (response->status == status && strcmp(response->body, body) == 0) {
        return 1;
    }
    fprintf(stderr, "%s response mismatch: %d %s\n", label, response->status, response->body);
    return 0;
}

static int expect_response_contains(
    const char *label,
    const interaction_http_response_t *response,
    int status,
    const char *fragment
) {
    if (response->status == status && strstr(response->body, fragment) != NULL) {
        return 1;
    }
    fprintf(stderr, "%s response mismatch: %d %s\n", label, response->status, response->body);
    return 0;
}

int interaction_run_request_smoke(
    dcc_interaction_server_t *server,
    EVP_PKEY *key,
    const char *public_key_hex,
    uint16_t port,
    interaction_seen_t *seen
) {
    interaction_http_response_t response;
    const char *timestamp = "1700000000";
    const char *ping_body = "{\"type\":1}";

    if (!interaction_send_request(port, "GET", "/healthz", "", NULL, NULL, &response) ||
        !expect_response("health route", &response, 200, "ok")) {
        return 0;
    }
    if (!interaction_send_request(port, "GET", "/readyz", "", NULL, NULL, &response) ||
        response.status != 200 ||
        strstr(response.body, "\"health\":\"ready\"") == NULL ||
        strstr(response.body, "\"ok\":true") == NULL) {
        fprintf(stderr, "built-in health route response mismatch: %d %s\n", response.status, response.body);
        return 0;
    }
    if (!interaction_send_request(port, "GET", "/ops/check", "", NULL, NULL, &response) ||
        !expect_response("prefix route", &response, 200, "ops")) {
        return 0;
    }
    if (!interaction_send_request(port, "GET", "/interactions", "", NULL, NULL, &response) ||
        !expect_response_contains("method rejection", &response, 405, "Method not allowed")) {
        return 0;
    }
    if (!interaction_send_interaction_request(port, ping_body, NULL, NULL, &response) ||
        !expect_response_contains("missing signature", &response, 401, "Unsigned requests are not allowed")) {
        return 0;
    }

    char bad_signature[129];
    memset(bad_signature, '0', sizeof(bad_signature) - 1U);
    bad_signature[sizeof(bad_signature) - 1U] = '\0';
    if (!interaction_send_interaction_request(port, ping_body, timestamp, bad_signature, &response) ||
        !expect_response_contains("invalid signature", &response, 401, "Access denied")) {
        return 0;
    }

    const char *large_body =
        "{\"type\":1,\"padding\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"}";
    if (!interaction_send_interaction_request(port, large_body, NULL, NULL, &response) ||
        !expect_response_contains("body limit", &response, 413, "Payload too large")) {
        return 0;
    }
    if (!interaction_send_oversized_header(port, &response) ||
        !expect_response_contains("header limit", &response, 413, "Payload too large")) {
        return 0;
    }

    char malformed_length_request[256];
    int malformed_length_len = snprintf(
        malformed_length_request,
        sizeof(malformed_length_request),
        "POST /interactions HTTP/1.1\r\n"
        "Host: 127.0.0.1:%u\r\n"
        "Content-Length: nope\r\n"
        "\r\n",
        (unsigned)port
    );
    if (malformed_length_len < 0 ||
        (size_t)malformed_length_len >= sizeof(malformed_length_request) ||
        !interaction_send_raw_request(port, malformed_length_request, &response) ||
        !expect_response_contains("malformed content-length", &response, 400, "Bad request")) {
        return 0;
    }

    char signature[129];
    if (!interaction_sign(key, timestamp, ping_body, signature) ||
        !dcc_interaction_verify_signature(public_key_hex, timestamp, ping_body, strlen(ping_body), signature) ||
        !interaction_send_interaction_request(port, ping_body, timestamp, signature, &response) ||
        !expect_response("ping", &response, 200, "{\"type\":1}")) {
        return 0;
    }
    if (!interaction_send_interaction_request(port, ping_body, timestamp, signature, &response) ||
        !expect_response_contains("replayed ping", &response, 409, "replayed")) {
        return 0;
    }

    const char *slash_body =
        "{\"id\":\"558\",\"application_id\":\"123\",\"type\":2,\"token\":\"tok/en\","
        "\"data\":{\"id\":\"777\",\"type\":1,\"name\":\"ping\"}}";
    if (!interaction_sign(key, timestamp, slash_body, signature) ||
        !interaction_send_interaction_request(port, slash_body, timestamp, signature, &response) ||
        !expect_response("slash", &response, 200, "{\"type\":4,\"data\":{\"content\":\"pong\"}}")) {
        return 0;
    }

    const char *secret_body =
        "{\"id\":\"562\",\"application_id\":\"123\",\"type\":2,\"token\":\"tok/en\","
        "\"data\":{\"id\":\"780\",\"type\":1,\"name\":\"secret\"}}";
    if (!interaction_sign(key, timestamp, secret_body, signature) ||
        !interaction_send_interaction_request(port, secret_body, timestamp, signature, &response) ||
        !expect_response("ephemeral", &response, 200, "{\"type\":4,\"data\":{\"content\":\"secret\",\"flags\":64}}")) {
        return 0;
    }

    const char *button_body =
        "{\"id\":\"561\",\"application_id\":\"123\",\"type\":3,\"token\":\"tok/en\","
        "\"data\":{\"component_type\":2,\"custom_id\":\"go\"}}";
    if (!interaction_sign(key, timestamp, button_body, signature) ||
        !interaction_send_interaction_request(port, button_body, timestamp, signature, &response) ||
        !expect_response("button update", &response, 200, "{\"type\":7,\"data\":{\"content\":\"updated\"}}")) {
        return 0;
    }

    const char *modal_body =
        "{\"id\":\"559\",\"application_id\":\"123\",\"type\":2,\"token\":\"tok/en\","
        "\"data\":{\"id\":\"778\",\"type\":1,\"name\":\"modal\"}}";
    if (!interaction_sign(key, timestamp, modal_body, signature) ||
        !interaction_send_interaction_request(port, modal_body, timestamp, signature, &response) ||
        !expect_response(
            "modal",
            &response,
            200,
            "{\"type\":9,\"data\":{\"custom_id\":\"feedback-modal\",\"title\":\"Feedback\","
            "\"components\":[]}}"
        )) {
        return 0;
    }

    const char *autocomplete_body =
        "{\"id\":\"560\",\"application_id\":\"123\",\"type\":4,\"token\":\"tok/en\","
        "\"data\":{\"id\":\"779\",\"type\":1,\"name\":\"choose\"}}";
    if (!interaction_sign(key, timestamp, autocomplete_body, signature) ||
        !interaction_send_interaction_request(port, autocomplete_body, timestamp, signature, &response) ||
        !expect_response(
            "autocomplete",
            &response,
            200,
            "{\"type\":8,\"data\":{\"choices\":[{\"name\":\"Red\",\"value\":\"red\"}]}}"
        )) {
        return 0;
    }

    const char *bad_json_body = "{\"type\":";
    if (!interaction_sign(key, timestamp, bad_json_body, signature) ||
        !interaction_send_interaction_request(port, bad_json_body, timestamp, signature, &response) ||
        !expect_response_contains("malformed json", &response, 400, "Invalid interaction payload")) {
        return 0;
    }
    if (!interaction_sign(key, timestamp, ping_body, signature) ||
        !interaction_send_request(port, "POST", "/wrong", ping_body, timestamp, signature, &response) ||
        !expect_response_contains("unknown signed route", &response, 404, "Interaction route not found")) {
        return 0;
    }

    if (seen->failed || seen->called != 6 || seen->health_seen != 1 || seen->prefix_seen != 1 ||
        seen->shadow_seen != 0 || seen->slow_seen != 0 || !seen->ping_seen || !seen->slash_seen ||
        !seen->modal_seen || !seen->autocomplete_seen || !seen->ephemeral_seen || !seen->button_seen) {
        fprintf(
            stderr,
            "callback state mismatch: failed=%d called=%d health=%d prefix=%d shadow=%d "
            "slow=%d ping=%d slash=%d modal=%d autocomplete=%d ephemeral=%d button=%d\n",
            seen->failed,
            seen->called,
            seen->health_seen,
            seen->prefix_seen,
            seen->shadow_seen,
            seen->slow_seen,
            seen->ping_seen,
            seen->slash_seen,
            seen->modal_seen,
            seen->autocomplete_seen,
            seen->ephemeral_seen,
            seen->button_seen
        );
        return 0;
    }

    dcc_interaction_server_state_t server_state = {
        .size = sizeof(server_state),
    };
    dcc_interaction_server_protection_stats_t protection = {
        .size = sizeof(protection),
    };
    if (dcc_interaction_server_get_state(server, &server_state) != DCC_OK ||
        dcc_interaction_server_protection_stats(server, &protection) != DCC_OK ||
        server_state.started != 1U ||
        server_state.stopping != 0U ||
        server_state.listening != 1U ||
        server_state.draining != 0U ||
        server_state.accepted_connections != 18U ||
        server_state.active_requests != 0U ||
        server_state.completed_requests != 18U ||
        protection.max_active_requests != 1024U ||
        protection.response_deadline_ms != 2800U ||
        protection.replay_window_ms != 300000U) {
        fprintf(
            stderr,
            "interaction steady state mismatch: started=%u stopping=%u listening=%u draining=%u "
            "accepted=%llu active=%llu completed=%llu\n",
            (unsigned)server_state.started,
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned long long)server_state.accepted_connections,
            (unsigned long long)server_state.active_requests,
            (unsigned long long)server_state.completed_requests
        );
        return 0;
    }

    return 1;
}

#endif
