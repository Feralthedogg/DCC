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

static int check_yes_no_poll_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_TEXT_POLL("vote", DCC_POLL_YES_NO("Deploy now?"));

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize yes/no poll message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"vote\"") ||
        require_contains(json, "\"poll\":{\"question\":{\"text\":\"Deploy now?\"}") ||
        require_contains(json, "{\"poll_media\":{\"text\":\"Yes\"}}") ||
        require_contains(json, "{\"poll_media\":{\"text\":\"No\"}}") ||
        require_contains(json, "\"duration\":24") ||
        require_contains(json, "\"allow_multiselect\":false") ||
        require_contains(json, "\"layout_type\":1");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_multi_poll_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_POLL(
            DCC_POLL_MULTI(
                "Pick environments",
                2U,
                DCC_POLL_ANSWER_EMOJI("Production", DCC_COMPONENT_EMOJI_NAME("rocket")),
                DCC_POLL_ANSWER("Staging")
            )
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize multi poll message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"question\":{\"text\":\"Pick environments\"}") ||
        require_contains(json, "\"text\":\"Production\",\"emoji\":{\"name\":\"rocket\"}") ||
        require_contains(json, "\"text\":\"Staging\"") ||
        require_contains(json, "\"duration\":2") ||
        require_contains(json, "\"allow_multiselect\":true");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_array_poll_and_raw_json(void) {
    dcc_poll_answer_t answers[] = {
        DCC_POLL_ANSWER("A"),
        DCC_POLL_ANSWER("B")
    };
    dcc_poll_builder_t poll =
        DCC_POLL_ARRAY("Choose one", answers, sizeof(answers) / sizeof(answers[0]), 1U);

    char *poll_json = NULL;
    if (dcc_message_poll_builder_build_json(&poll, &poll_json) != DCC_OK) {
        fprintf(stderr, "failed to serialize array poll\n");
        return 1;
    }

    dcc_message_builder_t message = DCC_MESSAGE_TEXT_POLL_JSON("raw", poll_json);
    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        dcc_message_poll_builder_json_free(poll_json);
        fprintf(stderr, "failed to serialize raw poll message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"raw\"") ||
        require_contains(json, "\"poll\":{\"question\":{\"text\":\"Choose one\"}") ||
        require_contains(json, "\"duration\":1");
    dcc_message_builder_json_free(json);
    dcc_message_poll_builder_json_free(poll_json);
    return failed;
}

static int check_poll_validation(void) {
    dcc_poll_builder_t invalid = DCC_POLL_YES_NO_FOR("Too long", 169U);
    char *json = NULL;
    if (dcc_message_poll_builder_build_json(&invalid, &json) != DCC_ERR_INVALID_ARG) {
        dcc_message_poll_builder_json_free(json);
        return 1;
    }
    return 0;
}

int main(void) {
    if (check_yes_no_poll_message() != 0) {
        return 1;
    }
    if (check_multi_poll_message() != 0) {
        return 1;
    }
    if (check_array_poll_and_raw_json() != 0) {
        return 1;
    }
    if (check_poll_validation() != 0) {
        return 1;
    }
    return 0;
}
