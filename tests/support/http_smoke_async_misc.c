#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_rate_limit_smoke(void) {
  http_server_t server;
  pthread_t thread;
  if (start_rate_limited_server(&server, &thread) != 0) {
    fprintf(stderr, "failed to start local rate limit server: %s\n",
            strerror(errno));
    return 1;
  }

  dcc_client_t *client = NULL;
  dcc_client_options_t opts = {
      .size = sizeof(opts),
      .token = "",
      .intents = DCC_INTENT_GUILDS,
  };
  dcc_status_t st = dcc_client_create(&opts, &client);
  if (st == DCC_OK) {
    st = rest_activate_client(client);
  }
  if (st != DCC_OK) {
    fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
    close(server.fd);
    return 1;
  }

  rest_seen_t seen;
  memset(&seen, 0, sizeof(seen));
  (void)dcc_client_on(client, DCC_EVENT_RATE_LIMITED, rate_limited_cb, &seen,
                      NULL);

  char url[128];
  snprintf(url, sizeof(url), "http://127.0.0.1:%u/rate", (unsigned)server.port);
  st = dcc_rest_request(client, "POST", url, "{\"x\":1}", rest_cb, &seen);

  (void)pthread_join(thread, NULL);
  close(server.fd);
  dcc_client_destroy(client);

  if (st != DCC_OK) {
    fprintf(stderr, "dcc_rest_request rate limit failed: %s\n",
            dcc_status_string(st));
    return 1;
  }
  if (!seen.called || !seen.rate_limited_called || seen.status != 200 ||
      seen.error != DCC_OK || seen.retry_after < 0.019 ||
      seen.retry_after > 0.021 || !seen.global || server.request_count != 2U ||
      strcmp(seen.method, "POST") != 0 || strcmp(seen.path, url) != 0 ||
      seen.body_len != 5 || strcmp(seen.body, "hello") != 0) {
    fprintf(stderr,
            "unexpected rate limit retry result: called=%d event=%d status=%u "
            "error=%s retry=%f global=%u requests=%u method=%s path=%s "
            "body_len=%llu body=%s\n",
            seen.called, seen.rate_limited_called, seen.status,
            dcc_status_string(seen.error), seen.retry_after, seen.global,
            server.request_count, seen.method, seen.path,
            (unsigned long long)seen.body_len, seen.body);
    return 1;
  }
  return 0;
}

int run_public_rest_direct_message_smoke(void) {
  dcc_client_t *client = NULL;
  dcc_client_options_t opts = {
      .size = sizeof(opts),
      .token = "",
      .intents = DCC_INTENT_GUILDS,
  };
  dcc_status_t st = dcc_client_create(&opts, &client);
  if (st == DCC_OK) {
    st = rest_activate_client(client);
  }
  if (st != DCC_OK) {
    fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
    return 1;
  }

  http_server_t server;
  pthread_t thread;
  rest_seen_t seen;

  if (start_direct_message_server(&server, &thread) != 0) {
    fprintf(stderr, "failed to start direct message flow server: %s\n",
            strerror(errno));
    dcc_client_destroy(client);
    return 1;
  }
  set_api_base_for_server(&server);
  memset(&seen, 0, sizeof(seen));
  dcc_dm_channel_params_t dm = DCC_DM_CHANNEL_PARAMS_INIT;
  dm.recipient_id = 444U;
  dcc_rest_request_t *request = NULL;
  st = dcc_rest_create_dm_channel(client, &dm, NULL, &request);
  const dcc_rest_result_t *result = NULL;
  if (st == DCC_OK) {
    st = dcc_rest_request_wait(request, 0U, &result);
  }
  dcc_snowflake_t channel_id = 0U;
  if (st == DCC_OK && dcc_rest_result_status(result) == DCC_OK) {
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = result->http_status,
        .error = DCC_OK,
        .body = result->body,
        .body_len = result->body_len,
    };
    st = dcc_rest_response_snowflake_field(&response, "id", &channel_id);
  }
  dcc_rest_request_destroy(request);
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  message.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
  message.content = "hi";
  dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
  dcc_rest_message_payload_init(&payload, &message);
  dcc_rest_call_options_t call_options =
      rest_call_options_from_legacy(rest_cb, &seen);
  if (st == DCC_OK) {
    st = dcc_rest_create_message(client, channel_id, &payload, &call_options,
                                 NULL);
  }
  st = rest_await_submission(client, st);
  (void)pthread_join(thread, NULL);
  close(server.fd);
  if (st != DCC_OK || !seen.called || seen.status != 200 ||
      server.request_count != 2 || strcmp(server.methods[0], "POST") != 0 ||
      strcmp(server.paths[0], "/users/@me/channels") != 0 ||
      strcmp(server.bodies[0], "{\"recipient_id\":\"444\"}") != 0 ||
      strcmp(server.methods[1], "POST") != 0 ||
      strcmp(server.paths[1], "/channels/222/messages") != 0 ||
      strcmp(server.bodies[1], "{\"content\":\"hi\"}") != 0) {
    fprintf(stderr,
            "unexpected direct message flow: st=%s called=%d status=%u "
            "requests=%u first=%s %s %s second=%s %s %s cached=%llu\n",
            dcc_status_string(st), seen.called, seen.status,
            server.request_count, server.methods[0], server.paths[0],
            server.bodies[0], server.methods[1], server.paths[1],
            server.bodies[1], (unsigned long long)channel_id);
    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 1;
  }

  (void)unsetenv("DCC_DISCORD_API_BASE");
  dcc_client_destroy(client);
  return 0;
}

#endif
