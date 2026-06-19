#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int multipart_body_has(const http_server_t *server, const char *needle) {
    return strstr(server->body, needle) != NULL;
}

int expect_multipart_request(
    const http_server_t *server,
    const char *method,
    const char *path,
    const char *payload_fragment,
    const char *field_fragment,
    const char *file_fragment
) {
    if (strcmp(server->method, method) != 0 ||
        strcmp(server->path, path) != 0 ||
        strcmp(server->content_type, "multipart/form-data; boundary=dcc-boundary-7MA4YWxkTrZu0gW") != 0 ||
        !multipart_body_has(server, "--dcc-boundary-7MA4YWxkTrZu0gW\r\n") ||
        !multipart_body_has(server, "--dcc-boundary-7MA4YWxkTrZu0gW--\r\n") ||
        (payload_fragment != NULL && !multipart_body_has(server, payload_fragment)) ||
        (field_fragment != NULL && !multipart_body_has(server, field_fragment)) ||
        (file_fragment != NULL && !multipart_body_has(server, file_fragment))) {
        fprintf(stderr,
                "unexpected multipart request: method=%s path=%s content_type=%s body=%s\n",
                server->method,
                server->path,
                server->content_type,
                server->body);
        return 0;
    }
    return 1;
}

int run_public_rest_multipart_smoke(void) {
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

    const char file_data[] = "hello-file";
    dcc_rest_multipart_file_t message_file = {
        .field_name = "files[0]",
        .filename = "hello.txt",
        .content_type = "text/plain",
        .data = file_data,
        .data_len = sizeof(file_data) - 1U,
    };

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message multipart server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_message_multipart(
        client,
        222,
        "{\"content\":\"hi\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "POST",
            "/channels/222/messages",
            "{\"content\":\"hi\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"\r\nContent-Type: text/plain\r\n\r\nhello-file")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start message edit multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_message_multipart(
        client,
        222,
        333,
        "{\"content\":\"edited\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/channels/222/messages/333",
            "{\"content\":\"edited\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start webhook multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_execute_webhook_multipart(
        client,
        666,
        "tok/en",
        "wait=true",
        "{\"content\":\"hook\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "POST",
            "/webhooks/666/tok%2Fen?wait=true",
            "{\"content\":\"hook\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start webhook multipart options server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_execute_webhook_multipart_options(
        client,
        666,
        "tok/en",
        1,
        888,
        1,
        "{\"content\":\"hook\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "POST",
            "/webhooks/666/tok%2Fen?wait=true&thread_id=888&with_components=true",
            "{\"content\":\"hook\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start webhook message edit multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_modify_webhook_message_multipart(
        client,
        666,
        "tok/en",
        777,
        "thread_id=888",
        "{\"content\":\"hook-edited\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
            "{\"content\":\"hook-edited\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start webhook message edit multipart thread server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_modify_webhook_message_multipart_thread(
        client,
        666,
        "tok/en",
        777,
        888,
        "{\"content\":\"hook-edited\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
            "{\"content\":\"hook-edited\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start interaction multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_followup_create_multipart(
        client,
        123,
        "tok/en",
        "{\"content\":\"follow\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "POST",
            "/webhooks/123/tok%2Fen",
            "{\"content\":\"follow\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start original response multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_original_response_edit_multipart(
        client,
        123,
        "tok/en",
        "{\"content\":\"original\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/@original",
            "{\"content\":\"original\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start followup edit multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_interaction_followup_edit_multipart(
        client,
        123,
        "tok/en",
        999,
        "{\"content\":\"follow-edited\"}",
        &message_file,
        1,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/999",
            "{\"content\":\"follow-edited\"}",
            "name=\"payload_json\"",
            "name=\"files[0]\"; filename=\"hello.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    const char sticker_data[] = "png-data";
    dcc_rest_multipart_field_t sticker_fields[] = {
        { .name = "name", .value = "slap" },
        { .name = "description", .value = "slap desc" },
        { .name = "tags", .value = "slap" },
    };
    dcc_rest_multipart_file_t sticker_file = {
        .field_name = "file",
        .filename = "slap.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data) - 1U,
    };

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start sticker multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_guild_sticker_multipart(
        client,
        333,
        sticker_fields,
        sizeof(sticker_fields) / sizeof(sticker_fields[0]),
        &sticker_file,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !multipart_body_has(&server, "name=\"name\"\r\n\r\nslap") ||
        !multipart_body_has(&server, "name=\"description\"\r\n\r\nslap desc") ||
        !expect_multipart_request(
            &server,
            "POST",
            "/guilds/333/stickers",
            NULL,
            "name=\"tags\"\r\n\r\nslap",
            "name=\"file\"; filename=\"slap.png\"\r\nContent-Type: image/png\r\n\r\npng-data")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    const dcc_guild_sticker_params_t sticker_params = {
        .size = sizeof(sticker_params),
        .guild_id = 333,
        .name = "slap",
        .description = "slap desc",
        .tags = "slap",
        .filename = "slap.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data) - 1U,
    };
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start sticker params server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_guild_sticker_params(client, &sticker_params, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        !multipart_body_has(&server, "name=\"name\"\r\n\r\nslap") ||
        !multipart_body_has(&server, "name=\"description\"\r\n\r\nslap desc") ||
        !expect_multipart_request(
            &server,
            "POST",
            "/guilds/333/stickers",
            NULL,
            "name=\"tags\"\r\n\r\nslap",
            "name=\"file\"; filename=\"slap.png\"\r\nContent-Type: image/png\r\n\r\npng-data")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}

#endif
