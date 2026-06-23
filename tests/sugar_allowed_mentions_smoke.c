#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int check_no_mentions_message(void) {
    dcc_message_builder_t message = DCC_MESSAGE_NO_MENTIONS("@everyone safe");

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize no-mentions message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"@everyone safe\"") ||
        require_contains(json, "\"allowed_mentions\":{\"parse\":[]}");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_user_mentions_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_MENTION_USERS("hi <@111> <@222>", 111ULL, 222ULL);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize user mentions message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"allowed_mentions\":{\"parse\":[],\"users\":[\"111\",\"222\"]}");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_role_and_reply_mentions_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_EPHEMERAL_MENTIONS(
            "role ping",
            DCC_ALLOWED_MENTIONS_ROLES_REPLY(1U, 333ULL)
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize role mentions message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"flags\":64") ||
        require_contains(json, "\"roles\":[\"333\"]") ||
        require_contains(json, "\"replied_user\":true");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_raw_and_typed_mentions_are_rejected(void) {
    dcc_allowed_mentions_builder_t mentions = DCC_ALLOWED_MENTIONS_NONE();
    dcc_message_builder_t message = DCC_MESSAGE_ALLOWED_MENTIONS_JSON("mixed", "{\"parse\":[]}");
    message.allowed_mentions = &mentions;

    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(&message, &json);
    dcc_message_builder_json_free(json);
    return status == DCC_ERR_INVALID_ARG ? 0 : 1;
}

static int check_parse_and_explicit_users_are_rejected(void) {
    dcc_snowflake_t user = 444ULL;
    dcc_allowed_mentions_builder_t mentions = {
        .users = &user,
        .user_count = 1U,
        .parse_users = 1U
    };
    dcc_message_builder_t message = DCC_MESSAGE_MENTIONS("invalid", mentions);

    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(&message, &json);
    dcc_message_builder_json_free(json);
    return status == DCC_ERR_INVALID_ARG ? 0 : 1;
}

int main(void) {
    if (check_no_mentions_message() != 0) {
        return 1;
    }
    if (check_user_mentions_message() != 0) {
        return 1;
    }
    if (check_role_and_reply_mentions_message() != 0) {
        return 1;
    }
    if (check_raw_and_typed_mentions_are_rejected() != 0) {
        return 1;
    }
    if (check_parse_and_explicit_users_are_rejected() != 0) {
        return 1;
    }
    return 0;
}
