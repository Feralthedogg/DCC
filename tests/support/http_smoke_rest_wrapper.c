#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_wrapper_expect(
    dcc_client_t *client,
    const char *label,
    rest_wrapper_call_fn call,
    const char *expected_method,
    const char *expected_path,
    const char *expected_body
) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start %s server: %s\n", label, strerror(errno));
        return 1;
    }

    rest_seen_t seen;
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    dcc_status_t st = call(client, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);

    int body_ok = expected_body != NULL ? strcmp(server.body, expected_body) == 0 : server.body_len == 0;
    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.method, expected_method) != 0 ||
        strcmp(server.path, expected_path) != 0 ||
        !body_ok) {
        fprintf(stderr,
                "unexpected %s wrapper request: st=%s called=%d status=%u method=%s path=%s body=%s\n",
                label,
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.method,
                server.path,
                server.body);
        return 1;
    }
    return 0;
}

int run_public_rest_wrapper_smoke(void) {
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
    st = dcc_client_set_dm_channel(client, 444, 222);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_set_dm_channel failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start wrapper get server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_get_channel(client, 222, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.method, "GET") != 0 ||
        strcmp(server.path, "/channels/222") != 0) {
        fprintf(stderr, "unexpected get_channel wrapper request: st=%s method=%s path=%s\n",
                dcc_status_string(st),
                server.method,
                server.path);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start wrapper post server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_message(client, 222, "{\"content\":\"hi\"}", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/channels/222/messages") != 0 ||
        strcmp(server.body, "{\"content\":\"hi\"}") != 0) {
        fprintf(stderr, "unexpected create_message wrapper request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start wrapper search server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_guild_message_search(client, 333, "content=hello&limit=2", rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "GET") != 0 ||
        strcmp(server.path, "/guilds/333/messages/search?content=hello&limit=2") != 0) {
        fprintf(stderr, "unexpected guild_message_search wrapper request: st=%s method=%s path=%s\n",
                dcc_status_string(st),
                server.method,
                server.path);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start typed wrapper search server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    const dcc_snowflake_t search_author_ids[] = {444};
    const dcc_snowflake_t search_mentions[] = {555};
    const dcc_snowflake_t search_channel_ids[] = {222};
    const dcc_snowflake_t search_mention_role_ids[] = {777};
    const dcc_snowflake_t search_replied_to_user_ids[] = {888};
    const dcc_snowflake_t search_replied_to_message_ids[] = {9999};
    const char * const search_author_types[] = {"bot"};
    const dcc_message_search_has_t search_has[] = {
        DCC_MESSAGE_SEARCH_HAS_LINK,
        DCC_MESSAGE_SEARCH_HAS_POLL
    };
    const char * const search_embed_types[] = {"rich"};
    const char * const search_embed_providers[] = {"youtube"};
    const char * const search_link_hostnames[] = {"example.com"};
    const char * const search_attachment_filenames[] = {"report.pdf"};
    const char * const search_attachment_extensions[] = {"pdf"};
    const dcc_message_search_params_t search_params = {
        .size = sizeof(search_params),
        .content = "hello ops",
        .author_ids = search_author_ids,
        .author_id_count = 1,
        .mentions = search_mentions,
        .mention_count = 1,
        .channel_ids = search_channel_ids,
        .channel_id_count = 1,
        .mention_role_ids = search_mention_role_ids,
        .mention_role_id_count = 1,
        .replied_to_user_ids = search_replied_to_user_ids,
        .replied_to_user_id_count = 1,
        .replied_to_message_ids = search_replied_to_message_ids,
        .replied_to_message_id_count = 1,
        .author_types = search_author_types,
        .author_type_count = 1,
        .has = search_has,
        .has_count = 2,
        .embed_types = search_embed_types,
        .embed_type_count = 1,
        .embed_providers = search_embed_providers,
        .embed_provider_count = 1,
        .link_hostnames = search_link_hostnames,
        .link_hostname_count = 1,
        .attachment_filenames = search_attachment_filenames,
        .attachment_filename_count = 1,
        .attachment_extensions = search_attachment_extensions,
        .attachment_extension_count = 1,
        .max_id = 999,
        .min_id = 111,
        .has_pinned = 1,
        .pinned = 0,
        .has_mention_everyone = 1,
        .mention_everyone = 1,
        .has_include_nsfw = 1,
        .include_nsfw = 1,
        .has_limit = 1,
        .limit = 2,
        .has_offset = 1,
        .offset = 3,
        .has_slop = 1,
        .slop = 4,
        .has_sort_by = 1,
        .sort_by = DCC_MESSAGE_SEARCH_SORT_RELEVANCE,
        .has_sort_order = 1,
        .sort_order = DCC_MESSAGE_SEARCH_ORDER_ASC
    };
    st = dcc_rest_guild_message_search_params(client, 333, &search_params, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "GET") != 0 ||
        strcmp(
            server.path,
            "/guilds/333/messages/search?content=hello%20ops&author_id=444&mentions=555&channel_id=222&mentions_role_id=777&replied_to_user_id=888&replied_to_message_id=9999&author_type=bot&has=link&has=poll&embed_type=rich&embed_provider=youtube&link_hostname=example.com&attachment_filename=report.pdf&attachment_extension=pdf&max_id=999&min_id=111&pinned=false&mention_everyone=true&include_nsfw=true&limit=2&offset=3&slop=4&sort_by=relevance&sort_order=asc"
        ) != 0) {
        fprintf(stderr, "unexpected guild_message_search params request: st=%s method=%s path=%s\n",
                dcc_status_string(st),
                server.method,
                server.path);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start global command server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_global_command(
        client,
        123,
        "{\"name\":\"ping\",\"description\":\"pong\",\"type\":1}",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.body, "{\"name\":\"ping\",\"description\":\"pong\",\"type\":1}") != 0) {
        fprintf(stderr, "unexpected global command request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start command permission server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_guild_command_permissions(
        client,
        123,
        333,
        444,
        "{\"permissions\":[]}",
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands/444/permissions") != 0 ||
        strcmp(server.body, "{\"permissions\":[]}") != 0) {
        fprintf(stderr, "unexpected command permission request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (run_public_rest_wrapper_interactions_smoke(client) != 0) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (run_public_rest_wrapper_expect_channels_smoke(client) != 0 ||
        run_public_rest_wrapper_expect_guilds_smoke(client) != 0 ||
        run_public_rest_wrapper_expect_users_threads_smoke(client) != 0 ||
        run_public_rest_wrapper_expect_webhooks_apps_smoke(client) != 0) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}
#endif
