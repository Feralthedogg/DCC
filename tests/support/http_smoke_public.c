#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start local rest server: %s\n", strerror(errno));
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(server.fd);
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/rest", (unsigned)server.port);

    rest_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request(client, "GET", url, NULL, rest_cb, &seen);

    (void)pthread_join(thread, NULL);
    close(server.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_rest_request failed: %s\n", dcc_status_string(st));
        return 1;
    }
    if (!seen.called || seen.status != 200 || seen.body_len != 5 || memcmp(seen.body, "hello", 5) != 0) {
        fprintf(stderr, "unexpected rest callback\n");
        return 1;
    }
    if (strstr(server.headers, "\r\nAuthorization:") != NULL ||
        strstr(server.headers, "\r\nauthorization:") != NULL) {
        fprintf(stderr, "empty token REST request emitted Authorization header: %s\n", server.headers);
        return 1;
    }
    return 0;
}

int run_public_rest_raw_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start raw rest server: %s\n", strerror(errno));
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(server.fd);
        return 1;
    }

    set_api_base_for_server(&server);
    rest_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    const unsigned char body[] = { 'a', '\0', 'b' };
    st = dcc_rest_request_raw(
        client,
        "POST",
        "/binary",
        body,
        sizeof(body),
        "application/octet-stream",
        rest_cb,
        &seen
    );

    (void)pthread_join(thread, NULL);
    close(server.fd);
    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/binary") != 0 ||
        strcmp(server.content_type, "application/octet-stream") != 0 ||
        strstr(server.headers, "\r\nAuthorization:") != NULL ||
        server.body_len != sizeof(body) ||
        memcmp(server.body, body, sizeof(body)) != 0) {
        fprintf(stderr,
                "unexpected raw rest request: st=%s called=%d status=%u method=%s path=%s content_type=%s body_len=%llu headers=%s\n",
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.method,
                server.path,
                server.content_type,
                (unsigned long long)server.body_len,
                server.headers);
        return 1;
    }
    return 0;
}

int run_public_rest_overload_smoke(void) {
    if (strcmp(dcc_rest_method_name(DCC_REST_GET), "GET") != 0 ||
        strcmp(dcc_rest_method_name(DCC_REST_POST), "POST") != 0 ||
        strcmp(dcc_rest_method_name(DCC_REST_PUT), "PUT") != 0 ||
        strcmp(dcc_rest_method_name(DCC_REST_PATCH), "PATCH") != 0 ||
        strcmp(dcc_rest_method_name(DCC_REST_DELETE), "DELETE") != 0 ||
        dcc_rest_method_name((dcc_rest_method_t)99) != NULL) {
        fprintf(stderr, "unexpected REST method name mapping\n");
        return 1;
    }

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

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;
    dcc_application_command_registration_options_t registration_probe;
    dcc_application_command_registration_options_init(&registration_probe);
    if (registration_probe.size != sizeof(registration_probe) || registration_probe.guild_id != 0 ||
        dcc_application_command_registration_options_set_global(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_application_command_registration_options_set_guild(NULL, 333) != DCC_ERR_INVALID_ARG ||
        dcc_application_command_registration_options_set_guild(&registration_probe, 0) != DCC_ERR_INVALID_ARG ||
        dcc_application_command_registration_options_set_guild(&registration_probe, 333) != DCC_OK ||
        registration_probe.size != sizeof(registration_probe) || registration_probe.guild_id != 333 ||
        dcc_application_command_registration_options_set_global(&registration_probe) != DCC_OK ||
        registration_probe.size != sizeof(registration_probe) || registration_probe.guild_id != 0) {
        fprintf(stderr, "unexpected application command registration option helper state\n");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start request method server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_method(client, DCC_REST_PUT, "/method", "{\"ok\":true}", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/method") != 0 ||
        strcmp(server.content_type, "application/json") != 0 ||
        strcmp(server.body, "{\"ok\":true}") != 0) {
        fprintf(stderr, "unexpected request method overload: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    const unsigned char raw_body[] = { 'm', '\0', 'r' };
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start request method raw server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_method_raw(
        client,
        DCC_REST_PATCH,
        "/method-raw",
        raw_body,
        sizeof(raw_body),
        "application/octet-stream",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PATCH") != 0 ||
        strcmp(server.path, "/method-raw") != 0 ||
        strcmp(server.content_type, "application/octet-stream") != 0 ||
        server.body_len != sizeof(raw_body) ||
        memcmp(server.body, raw_body, sizeof(raw_body)) != 0) {
        fprintf(stderr, "unexpected request method raw overload: st=%s method=%s path=%s len=%llu\n",
                dcc_status_string(st),
                server.method,
                server.path,
                (unsigned long long)server.body_len);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    const char file_data[] = "overload-file";
    dcc_rest_multipart_field_t fields[] = {
        { .name = "payload_json", .value = "{\"content\":\"overload\"}" },
    };
    dcc_rest_multipart_file_t file = {
        .field_name = "files[0]",
        .filename = "overload.txt",
        .content_type = "text/plain",
        .data = file_data,
        .data_len = sizeof(file_data) - 1U,
    };

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start request multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_multipart(
        client,
        "POST",
        "/multipart-overload",
        fields,
        sizeof(fields) / sizeof(fields[0]),
        &file,
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
            "/multipart-overload",
            NULL,
            "name=\"payload_json\"\r\n\r\n{\"content\":\"overload\"}",
            "name=\"files[0]\"; filename=\"overload.txt\"\r\nContent-Type: text/plain\r\n\r\noverload-file")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start request method multipart server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_method_multipart(
        client,
        DCC_REST_PATCH,
        "/multipart-method",
        fields,
        sizeof(fields) / sizeof(fields[0]),
        &file,
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
            "/multipart-method",
            NULL,
            "name=\"payload_json\"\r\n\r\n{\"content\":\"overload\"}",
            "name=\"files[0]\"; filename=\"overload.txt\"")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);

    http_server_t first;
    http_server_t second;
    pthread_t first_thread;
    pthread_t second_thread;
    if (start_server(&first, &first_thread) != 0) {
        fprintf(stderr, "failed to start request method async first server: %s\n", strerror(errno));
        return 1;
    }
    if (start_server(&second, &second_thread) != 0) {
        fprintf(stderr, "failed to start request method async second server: %s\n", strerror(errno));
        close(first.fd);
        return 1;
    }

    opts.rest_concurrency = 2;
    st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        return 1;
    }
    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char first_url[128];
    char second_url[128];
    snprintf(first_url, sizeof(first_url), "http://127.0.0.1:%u/method-async", (unsigned)first.port);
    snprintf(second_url, sizeof(second_url), "http://127.0.0.1:%u/method-async-priority", (unsigned)second.port);

    rest_async_seen_t async_seen;
    memset(&async_seen, 0, sizeof(async_seen));
    st = dcc_rest_request_method_async(client, DCC_REST_GET, first_url, NULL, async_rest_cb, &async_seen);
    if (st == DCC_OK) {
        st = dcc_rest_request_method_async_priority(
            client,
            DCC_REST_POST,
            second_url,
            "{\"p\":1}",
            DCC_REST_PRIORITY_HIGH,
            async_rest_cb,
            &async_seen
        );
    }
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    } else {
        (void)dcc_client_stop(client);
    }

    (void)pthread_join(first_thread, NULL);
    (void)pthread_join(second_thread, NULL);
    close(first.fd);
    close(second.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        async_seen.called != 2 ||
        async_seen.statuses[0] != 200 ||
        async_seen.statuses[1] != 200 ||
        strcmp(first.method, "GET") != 0 ||
        strcmp(first.path, "/method-async") != 0 ||
        strcmp(second.method, "POST") != 0 ||
        strcmp(second.path, "/method-async-priority") != 0 ||
        strcmp(second.body, "{\"p\":1}") != 0) {
        fprintf(stderr,
                "unexpected request method async overload: st=%s called=%d first=%s %s second=%s %s body=%s\n",
                dcc_status_string(st),
                async_seen.called,
                first.method,
                first.path,
                second.method,
                second.path,
                second.body);
        return 1;
    }

    return 0;
}

#endif
