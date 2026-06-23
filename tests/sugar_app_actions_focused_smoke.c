#include <dcc/sugar.h>

#include <stdio.h>

static dcc_component_v2_builder_t focused_text_component(void) {
    return (dcc_component_v2_builder_t){
        .type = DCC_COMPONENT_V2_TEXT_DISPLAY,
        .content = "hello"
    };
}

static void focused_message_id_cb(
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

static void focused_message_thread_cb(
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

static int check_message_literals(void) {
    dcc_message_builder_t text = DCC_MESSAGE_TEXT("hello");
    dcc_message_builder_t v2 = DCC_MESSAGE_COMPONENTS_V2(focused_text_component());
    dcc_message_builder_t v2_private =
        DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(focused_text_component());

    if (text.content == NULL || text.has_content != 1U) {
        fprintf(stderr, "DCC_MESSAGE_TEXT did not set content\n");
        return 1;
    }
    if (v2.components_v2_count != 1U ||
        (v2.flags & DCC_MESSAGE_FLAG_IS_COMPONENTS_V2) == 0U ||
        v2.has_flags != 1U) {
        fprintf(stderr, "DCC_MESSAGE_COMPONENTS_V2 did not set v2 fields\n");
        return 1;
    }
    if ((v2_private.flags & DCC_MESSAGE_FLAG_EPHEMERAL) == 0U ||
        (v2_private.flags & DCC_MESSAGE_FLAG_IS_COMPONENTS_V2) == 0U) {
        fprintf(stderr, "DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2 did not set flags\n");
        return 1;
    }
    return 0;
}

static int check_null_app_action_aliases(void) {
    dcc_thread_params_t thread_params = {
        .size = sizeof(dcc_thread_params_t),
        .channel_id = 333ULL,
        .message_id = 444ULL,
        .name = "focused-thread"
    };

    int ok =
        DCC_APP_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_CB(NULL, 333ULL, DCC_MESSAGE_TEXT("send"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_ID(NULL, 333ULL, DCC_MESSAGE_TEXT("send"), focused_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD(NULL, 333ULL, DCC_MESSAGE_TEXT("send"), "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD_CB(
            NULL,
            333ULL,
            DCC_MESSAGE_TEXT("send"),
            "thread",
            focused_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_THREAD_PARAMS(
            NULL,
            333ULL,
            DCC_MESSAGE_TEXT("send"),
            &thread_params,
            focused_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT(NULL, 333ULL, "send") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_CB(NULL, 333ULL, "send", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_ID(NULL, 333ULL, "send", focused_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_THREAD(NULL, 333ULL, "send", "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_THREAD_CB(NULL, 333ULL, "send", "thread", focused_message_thread_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_CB(NULL, 333ULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_ID(NULL, 333ULL, focused_message_id_cb, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_THREAD(NULL, 333ULL, "thread", focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_THREAD_CB(
            NULL,
            333ULL,
            "thread",
            focused_message_thread_cb,
            NULL,
            focused_text_component()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_EPHEMERAL_V2(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_ID(NULL, 333ULL, focused_message_id_cb, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_THREAD(NULL, 333ULL, "thread", focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_THREAD_CB(
            NULL,
            333ULL,
            "thread",
            focused_message_thread_cb,
            NULL,
            focused_text_component()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_EPHEMERAL_UI(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_CHANNEL(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_CHANNEL_CB(NULL, 333ULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL(NULL, 333ULL, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL_CB(NULL, 333ULL, "{\"name\":\"general\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_CHANNEL(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_CHANNEL_CB(NULL, 333ULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_TRIGGER_CHANNEL_TYPING(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_TRIGGER_CHANNEL_TYPING_CB(NULL, 333ULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_FOLLOW_NEWS_CHANNEL(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SET_CHANNEL_VOICE_STATUS(NULL, 333ULL, "online") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_ID(NULL, 333ULL, DCC_MESSAGE_TEXT("send"), focused_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_THREAD(NULL, 333ULL, DCC_MESSAGE_TEXT("send"), "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_THREAD_CB(
            NULL,
            333ULL,
            DCC_MESSAGE_TEXT("send"),
            "thread",
            focused_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_THREAD_PARAMS(
            NULL,
            333ULL,
            DCC_MESSAGE_TEXT("send"),
            &thread_params,
            focused_message_thread_cb,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT(NULL, 333ULL, "send") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT_ID(NULL, 333ULL, "send", focused_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT_THREAD(NULL, 333ULL, "send", "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT_THREAD_CB(NULL, 333ULL, "send", "thread", focused_message_thread_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_SAFE(NULL, 333ULL, "@everyone safe") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_V2(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_V2_ID(NULL, 333ULL, focused_message_id_cb, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_V2_THREAD(NULL, 333ULL, "thread", focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_V2_THREAD_CB(
            NULL,
            333ULL,
            "thread",
            focused_message_thread_cb,
            NULL,
            focused_text_component()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_UI(NULL, 333ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_UI_ID(NULL, 333ULL, focused_message_id_cb, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_UI_THREAD(NULL, 333ULL, "thread", focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_UI_THREAD_CB(
            NULL,
            333ULL,
            "thread",
            focused_message_thread_cb,
            NULL,
            focused_text_component()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_FETCH(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_GET(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_EDIT(NULL, 333ULL, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_DELETE(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_TYPING(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_TRIGGER_TYPING(NULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_FOLLOW(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SET_VOICE_STATUS(NULL, 333ULL, "online") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_MESSAGE(NULL, 333ULL, 444ULL, DCC_MESSAGE_TEXT("edit")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_MESSAGE_CB(NULL, 333ULL, 444ULL, DCC_MESSAGE_TEXT("edit"), NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_TEXT(NULL, 333ULL, 444ULL, "edit") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_TEXT_CB(NULL, 333ULL, 444ULL, "edit", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_V2(NULL, 333ULL, 444ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_V2_CB(NULL, 333ULL, 444ULL, NULL, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_UI(NULL, 333ULL, 444ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_MESSAGE(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_MESSAGE_CB(NULL, 333ULL, 444ULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT(NULL, 333ULL, 444ULL, DCC_MESSAGE_TEXT("edit")) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_TEXT(NULL, 333ULL, 444ULL, "edit") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_UI(NULL, 333ULL, 444ULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_DELETE(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CROSSPOST_MESSAGE(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_PIN_MESSAGE(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_UNPIN_MESSAGE(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ADD_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_OWN_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup") ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_USER_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup", 555ULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_CLEAR_MESSAGE_REACTIONS(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI(NULL, 333ULL, 444ULL, "thumbsup") ==
            DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CROSSPOST(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_PIN(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_UNPIN(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_REACT(NULL, 333ULL, 444ULL, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_UNREACT(NULL, 333ULL, 444ULL, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_REMOVE_REACTION(NULL, 333ULL, 444ULL, "thumbsup", 555ULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CLEAR_REACTIONS(NULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CLEAR_REACTION_EMOJI(NULL, 333ULL, 444ULL, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE(NULL, 333ULL, 444ULL, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE_CB(NULL, 333ULL, 444ULL, "thread", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE_PARAMS(NULL, 333ULL, 444ULL, &thread_params) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE_PARAMS_CB(NULL, 333ULL, 444ULL, &thread_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_THREAD(NULL, 333ULL, 444ULL, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_THREAD_PARAMS(NULL, 333ULL, 444ULL, &thread_params) == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_FROM_MESSAGE(NULL, 333ULL, 444ULL, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_FROM_MESSAGE_PARAMS(NULL, 333ULL, 444ULL, &thread_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ARCHIVE_THREAD(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_LOCK_THREAD(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_UNLOCK_THREAD(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_ARCHIVE(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_LOCK(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_UNLOCK(NULL, 555ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ADD_MEMBER_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REMOVE_MEMBER_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_ADD_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_REMOVE_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_ADD_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_REMOVE_ROLE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_FETCH(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_TIMEOUT(NULL, 222ULL, 333ULL, "2026-06-23T00:00:00Z") == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_CLEAR_TIMEOUT(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_MOVE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_KICK(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_FETCH(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_TIMEOUT(NULL, 222ULL, 333ULL, "2026-06-23T00:00:00Z") ==
            DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_CLEAR_TIMEOUT(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_MOVE(NULL, 222ULL, 333ULL, 444ULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_KICK(NULL, 222ULL, 333ULL) == DCC_ERR_INVALID_ARG;
    return ok ? 0 : 1;
}

int main(void) {
    if (check_message_literals() != 0) {
        return 1;
    }
    if (check_null_app_action_aliases() != 0) {
        fprintf(stderr, "focused app action sugar aliases failed\n");
        return 1;
    }
    return 0;
}
