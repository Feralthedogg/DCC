#include <dcc/app/legacy.h>
#include <dcc/app/lifecycle.h>
#include <dcc/rest/guilds/integrations.h>
#include <dcc/rest/types/emojis_stickers.h>
#include <dcc/rest/types/onboarding.h>
#include <dcc/rest/types/scheduled_events.h>

#include "internal/rest/dcc_rest_intercept_internal.h"

#include <llam/runtime.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>

typedef struct legacy_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} legacy_wait_state_t;

static void *legacy_wait_main(void *user_data) {
    legacy_wait_state_t *state = (legacy_wait_state_t *)user_data;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

typedef struct legacy_rest_capture {
    unsigned calls;
    char method[16];
    char path[256];
    char body[4096];
    size_t body_len;
    unsigned callback_calls;
    uint16_t callback_status;
    dcc_status_t callback_error;
} legacy_rest_capture_t;

static dcc_status_t legacy_rest_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb callback,
    void *callback_user_data,
    void *intercept_user_data
) {
    (void)content_type;
    legacy_rest_capture_t *capture =
        (legacy_rest_capture_t *)intercept_user_data;
    if (capture == NULL || method == NULL || path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    ++capture->calls;
    (void)snprintf(capture->method, sizeof(capture->method), "%s", method);
    (void)snprintf(capture->path, sizeof(capture->path), "%s", path);
    capture->body_len = body_len < sizeof(capture->body) - 1U
        ? body_len : sizeof(capture->body) - 1U;
    if (capture->body_len != 0U && body != NULL) {
        memcpy(capture->body, body, capture->body_len);
    }
    capture->body[capture->body_len] = '\0';

    if (callback != NULL) {
        static const char response_body[] = "{}";
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = 200U,
            .error = DCC_OK,
            .body = response_body,
            .body_len = sizeof(response_body) - 1U,
        };
        callback(client, &response, callback_user_data);
    }
    return DCC_OK;
}

static void legacy_rest_callback(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    legacy_rest_capture_t *capture = (legacy_rest_capture_t *)user_data;
    if (capture == NULL || response == NULL) {
        return;
    }
    ++capture->callback_calls;
    capture->callback_status = response->status;
    capture->callback_error = response->error;
}

static int expect_request(
    legacy_rest_capture_t *capture,
    dcc_status_t status,
    const char *label,
    const char *method,
    const char *path,
    const char *body_fragment
) {
    for (unsigned i = 0U; i < 500U && capture->callback_calls == 0U; ++i) {
        (void)llam_sleep_ns(1000000ULL);
    }
    int body_ok = body_fragment == NULL
        ? capture->body_len == 0U
        : strstr(capture->body, body_fragment) != NULL;
    if (status == DCC_OK && capture->callback_calls == 1U &&
        capture->callback_status == 200U && capture->callback_error == DCC_OK &&
        strcmp(capture->method, method) == 0 &&
        strcmp(capture->path, path) == 0 && body_ok) {
        return 1;
    }
    fprintf(
        stderr,
        "%s failed: status=%s callbacks=%u response=%u/%s method=%s path=%s body=%s\n",
        label,
        dcc_status_string(status),
        capture->callback_calls,
        capture->callback_status,
        dcc_status_string(capture->callback_error),
        capture->method,
        capture->path,
        capture->body
    );
    return 0;
}

#define EXPECT_REQUEST(label_, method_, path_, body_, call_) do { \
    memset(&capture, 0, sizeof(capture)); \
    dcc_status_t request_status_ = (call_); \
    if (!expect_request(&capture, request_status_, (label_), (method_), \
                        (path_), (body_))) { \
        dcc_rest_set_interceptor(dcc_app_client(app), NULL, NULL); \
        (void)dcc_client_stop(dcc_app_client(app)); \
        (void)pthread_join(wait_thread, NULL); \
        (void)dcc_app_destroy(app); \
        return 1; \
    } \
} while (0)

int main(void) {
    dcc_app_options_t app_options;
    dcc_app_options_init(&app_options);
    app_options.client.token = "";
    dcc_app_t *app = NULL;
    if (dcc_app_create(&app_options, &app) != DCC_OK || app == NULL) {
        fprintf(stderr, "failed to create app\n");
        return 1;
    }

    legacy_rest_capture_t capture;
    memset(&capture, 0, sizeof(capture));
    dcc_rest_set_interceptor(
        dcc_app_client(app), legacy_rest_intercept, &capture
    );
    if (dcc_client_start(dcc_app_client(app)) != DCC_OK) {
        dcc_rest_set_interceptor(dcc_app_client(app), NULL, NULL);
        (void)dcc_app_destroy(app);
        fprintf(stderr, "failed to start app client\n");
        return 1;
    }
    legacy_wait_state_t wait_state = {
        .client = dcc_app_client(app),
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, legacy_wait_main, &wait_state) != 0) {
        dcc_rest_set_interceptor(dcc_app_client(app), NULL, NULL);
        (void)dcc_client_stop(dcc_app_client(app));
        (void)dcc_app_destroy(app);
        fprintf(stderr, "failed to start client wait thread\n");
        return 1;
    }

    EXPECT_REQUEST(
        "current voice state get", "GET", "/guilds/123/voice-states/@me", NULL,
        dcc_app_get_current_user_voice_state(app, 123U, legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "current voice state modify", "PATCH", "/guilds/123/voice-states/@me", "channel_id",
        dcc_app_modify_current_user_voice_state(app, 123U, "{\"channel_id\":\"456\"}", legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "user voice state get", "GET", "/guilds/123/voice-states/456", NULL,
        dcc_app_get_user_voice_state(app, 123U, 456U, legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "user voice state modify", "PATCH", "/guilds/123/voice-states/456", "channel_id",
        dcc_app_modify_user_voice_state(app, 123U, 456U, "{\"channel_id\":\"789\"}", legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "current user dms", "GET", "/users/@me/channels", NULL,
        dcc_app_get_current_user_dms(app, legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "integration modify", "PATCH", "/guilds/123/integrations/456", "expire_behavior",
        dcc_app_modify_guild_integration(app, 123U, 456U, "{\"expire_behavior\":0}", legacy_rest_callback, &capture)
    );
    EXPECT_REQUEST(
        "integration sync", "POST", "/guilds/123/integrations/456", NULL,
        dcc_app_sync_guild_integration(app, 123U, 456U, legacy_rest_callback, &capture)
    );

    dcc_onboarding_params_t onboarding = {
        .size = sizeof(onboarding),
        .guild_id = 123U,
        .enabled = 1U,
        .mode = DCC_ONBOARDING_MODE_DEFAULT,
    };
    EXPECT_REQUEST(
        "onboarding params", "PUT", "/guilds/123/onboarding", "\"enabled\":true",
        dcc_app_modify_guild_onboarding_params(app, &onboarding, legacy_rest_callback, &capture)
    );

    dcc_emoji_params_t emoji = DCC_EMOJI_PARAMS_INIT;
    emoji.present = DCC_EMOJI_PARAMS_PRESENT_NAME;
    emoji.name = "wave";
    emoji.emoji_id = 456U;
    EXPECT_REQUEST(
        "emoji params", "PATCH", "/guilds/123/emojis/456", "\"name\":\"wave\"",
        dcc_app_modify_guild_emoji_params(app, 123U, &emoji, legacy_rest_callback, &capture)
    );

    static const unsigned char sticker_data[] = {0x89U, 0x50U, 0x4eU, 0x47U};
    dcc_guild_sticker_params_t sticker = DCC_GUILD_STICKER_PARAMS_INIT;
    sticker.present = DCC_GUILD_STICKER_PARAMS_PRESENT_NAME |
        DCC_GUILD_STICKER_PARAMS_PRESENT_TAGS |
        DCC_GUILD_STICKER_PARAMS_PRESENT_FILE;
    sticker.name = "wave";
    sticker.tags = "wave";
    sticker.guild_id = 123U;
    sticker.file.field_name = "file";
    sticker.file.filename = "wave.png";
    sticker.file.content_type = "image/png";
    sticker.file.data = sticker_data;
    sticker.file.data_len = sizeof(sticker_data);
    EXPECT_REQUEST(
        "sticker params", "POST", "/guilds/123/stickers", "name\"",
        dcc_app_create_guild_sticker_params(app, &sticker, legacy_rest_callback, &capture)
    );

    dcc_scheduled_event_params_t event = DCC_SCHEDULED_EVENT_PARAMS_INIT;
    event.guild_id = 123U;
    event.present = DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID |
        DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
        DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
        DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
        DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE;
    event.channel_id = 456U;
    event.name = "release";
    event.scheduled_start_time = "2026-09-07T00:00:00Z";
    EXPECT_REQUEST(
        "scheduled event create params", "POST", "/guilds/123/scheduled-events", "\"name\":\"release\"",
        dcc_app_create_guild_scheduled_event_params(app, &event, legacy_rest_callback, &capture)
    );
    event.event_id = 789U;
    EXPECT_REQUEST(
        "scheduled event modify params", "PATCH", "/guilds/123/scheduled-events/789", "\"name\":\"release\"",
        dcc_app_modify_guild_scheduled_event_params(app, &event, legacy_rest_callback, &capture)
    );

    dcc_rest_set_interceptor(dcc_app_client(app), NULL, NULL);
    (void)dcc_client_stop(dcc_app_client(app));
    (void)pthread_join(wait_thread, NULL);
    return dcc_app_destroy(app) == DCC_OK ? 0 : 1;
}

#undef EXPECT_REQUEST
