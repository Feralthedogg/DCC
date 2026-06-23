#include "package_consumer_checks.h"

#include <dcc/sugar.h>

static void dcc_package_consumer_message_id_cb(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_status_t status,
    void *user_data
) {
    (void)app;
    (void)response;
    (void)message_id;
    (void)status;
    (void)user_data;
}

static void dcc_package_consumer_message_thread_cb(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_status_t status,
    void *user_data
) {
    (void)app;
    (void)response;
    (void)message_id;
    (void)thread_id;
    (void)status;
    (void)user_data;
}

int dcc_package_consumer_check_app_message_api(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;

    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK || app == NULL) {
        return 0;
    }

    dcc_message_builder_t message = DCC_MESSAGE_TEXT("message");
    dcc_thread_params_t thread_params = {
        .size = sizeof(thread_params),
        .name = "package-thread",
    };

    int ok =
        dcc_app_send(NULL, 1U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_with_id(NULL, 1U, &message, dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_send_with_thread(
            NULL,
            1U,
            &message,
            &thread_params,
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_with_thread_name(
            NULL,
            1U,
            &message,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_text(app, 0U, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_text_with_id(app, 0U, "hello", dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_send_text_with_thread(
            app,
            0U,
            "hello",
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_thread_from_message(app, 0U, 1U, &thread_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_thread_from_message_name(app, 0U, 1U, "thread", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND(NULL, 1U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_ID(NULL, 1U, DCC_MESSAGE_TEXT("hello"), dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD(NULL, 1U, DCC_MESSAGE_TEXT("hello"), "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD_CB(
            NULL,
            1U,
            DCC_MESSAGE_TEXT("hello"),
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD_PARAMS(
            NULL,
            1U,
            DCC_MESSAGE_TEXT("hello"),
            &thread_params,
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_THREAD(NULL, 1U, "hello", "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_THREAD_CB(
            NULL,
            1U,
            "hello",
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2(NULL, 1U, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_ID(NULL, 1U, dcc_package_consumer_message_id_cb, NULL, DCC_V2_TEXT("hello")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_THREAD(NULL, 1U, "thread", DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_THREAD_CB(
            NULL,
            1U,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_V2_TEXT("hello")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI(NULL, 1U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_ID(NULL, 1U, dcc_package_consumer_message_id_cb, NULL, DCC_UI_TEXT("hello")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_THREAD(NULL, 1U, "thread", DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_THREAD_CB(
            NULL,
            1U,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_UI_TEXT("hello")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_CB(NULL, 1U, NULL, NULL, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send(NULL, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_with_id(NULL, &message, dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_with_thread(
            NULL,
            &message,
            &thread_params,
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_with_thread_name(
            NULL,
            &message,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_text(NULL, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_text_with_id(NULL, "hello", dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_text_with_thread(
            NULL,
            "hello",
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SEND(NULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_ID(NULL, DCC_MESSAGE_TEXT("send"), dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_THREAD(NULL, DCC_MESSAGE_TEXT("send"), "thread") == DCC_ERR_INVALID_ARG &&
        DCC_SEND_THREAD_CB(
            NULL,
            DCC_MESSAGE_TEXT("send"),
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_THREAD_PARAMS(
            NULL,
            DCC_MESSAGE_TEXT("send"),
            &thread_params,
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND(NULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_ID(NULL, DCC_MESSAGE_TEXT("send"), dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_THREAD(NULL, DCC_MESSAGE_TEXT("send"), "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_THREAD_CB(
            NULL,
            DCC_MESSAGE_TEXT("send"),
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_THREAD_PARAMS(
            NULL,
            DCC_MESSAGE_TEXT("send"),
            &thread_params,
            dcc_package_consumer_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT(NULL, "send") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT_ID(NULL, "send", dcc_package_consumer_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_TEXT_THREAD(NULL, "send", "thread") == DCC_ERR_INVALID_ARG &&
        DCC_SEND_TEXT_THREAD_CB(NULL, "send", "thread", dcc_package_consumer_message_thread_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT_THREAD(NULL, "send", "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT_THREAD_CB(NULL, "send", "thread", dcc_package_consumer_message_thread_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2(NULL, DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2_ID(NULL, dcc_package_consumer_message_id_cb, NULL, DCC_V2_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2_THREAD(NULL, "thread", DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2_THREAD_CB(
            NULL,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_V2_TEXT("send")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2(NULL, DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2_ID(NULL, dcc_package_consumer_message_id_cb, NULL, DCC_V2_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2_THREAD(NULL, "thread", DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2_THREAD_CB(
            NULL,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_V2_TEXT("send")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI(NULL, DCC_UI_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI_ID(NULL, dcc_package_consumer_message_id_cb, NULL, DCC_UI_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_UI_THREAD(NULL, "thread", DCC_UI_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_UI_THREAD_CB(
            NULL,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_UI_TEXT("send")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI_THREAD(NULL, "thread", DCC_UI_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI_THREAD_CB(
            NULL,
            "thread",
            dcc_package_consumer_message_thread_cb,
            NULL,
            DCC_UI_TEXT("send")
        ) == DCC_ERR_INVALID_ARG;

    dcc_app_destroy(app);
    return ok;
}
