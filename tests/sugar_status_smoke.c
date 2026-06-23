#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

DCC_FRIENDLY_ERROR_FN(sugar_status_friendly_error)
DCC_SIMPLE_ERROR_FN(sugar_status_simple_error)
DCC_VERBOSE_ERROR_FN(sugar_status_verbose_error)

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int check_status_message(void) {
    dcc_message_builder_t message = DCC_MESSAGE_SUCCESS("Saved.");

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize success status message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"flags\":64") ||
        require_contains(json, "\"embeds\":[{\"title\":\"Done\"") ||
        require_contains(json, "\"description\":\"Saved.\"") ||
        require_contains(json, "\"color\":5763719") ||
        require_contains(json, "\"allowed_mentions\":{\"parse\":[]}");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_public_status_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_STATUS("Status", "Gateway online", DCC_COLOR_INFO);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize public status message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"title\":\"Status\"") ||
        require_contains(json, "\"description\":\"Gateway online\"") ||
        require_contains(json, "\"color\":5793266") ||
        (strstr(json, "\"flags\"") != NULL);
    dcc_message_builder_json_free(json);
    return failed ? 1 : 0;
}

static int check_private_and_silent_status_messages(void) {
    dcc_message_builder_t private_message =
        DCC_MESSAGE_PRIVATE_STATUS("Private", "Only you can see this.", DCC_COLOR_INFO);
    dcc_message_builder_t silent_message =
        DCC_MESSAGE_SILENT_STATUS("Silent", "No push notification.", DCC_COLOR_WARNING);

    char *json = NULL;
    if (dcc_message_builder_build_json(&private_message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize private status message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"title\":\"Private\"") ||
        require_contains(json, "\"flags\":64");
    dcc_message_builder_json_free(json);
    if (failed) {
        return 1;
    }

    json = NULL;
    if (dcc_message_builder_build_json(&silent_message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize silent status message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"title\":\"Silent\"") ||
        require_contains(json, "\"flags\":4096");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_friendly_status_aliases(void) {
    dcc_message_builder_t warning = DCC_MESSAGE_WARNING("Check the input.");
    dcc_message_builder_t info = DCC_MESSAGE_PUBLIC_INFO("Gateway online.");
    char *json = NULL;

    if (dcc_message_builder_build_json(&warning, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize warning status message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"title\":\"Warning\"") ||
        require_contains(json, "\"description\":\"Check the input.\"") ||
        require_contains(json, "\"color\":16705372") ||
        require_contains(json, "\"flags\":64");
    dcc_message_builder_json_free(json);
    if (failed) {
        return 1;
    }

    json = NULL;
    if (dcc_message_builder_build_json(&info, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize public info status message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"title\":\"Info\"") ||
        require_contains(json, "\"description\":\"Gateway online.\"") ||
        require_contains(json, "\"color\":5793266") ||
        (strstr(json, "\"flags\"") != NULL);
    dcc_message_builder_json_free(json);
    return failed ? 1 : 0;
}

static int check_safe_text_message(void) {
    dcc_message_builder_t message = DCC_MESSAGE_NO_MENTIONS("@everyone safe");

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize safe text message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"@everyone safe\"") ||
        require_contains(json, "\"allowed_mentions\":{\"parse\":[]}");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_null_context_helpers(void) {
    return
        DCC_REPLY_SAFE(NULL, "safe") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_SAFE(NULL, "safe") == DCC_ERR_INVALID_ARG &&
        DCC_SEND_SAFE(NULL, "safe") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_STATUS(NULL, "Status", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_OK(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_WARNING(NULL, "warning") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_WARN(NULL, "warning") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_INFO(NULL, "info") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC_OK(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_WARNING(NULL, "warning") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_WARN(NULL, "warning") == DCC_ERR_INVALID_ARG &&
        DCC_INFO(NULL, "info") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_SAFE(NULL, 1ULL, "safe") == DCC_ERR_INVALID_ARG
            ? 0
            : 1;
}

static int check_error_handler_sugar(void) {
    int failed =
        strcmp(DCC_ERROR_DESCRIPTION(DCC_ERR_INVALID_ARG, "custom"), "custom") != 0 ||
        strcmp(DCC_ERROR_DESCRIPTION(DCC_ERR_INVALID_ARG, NULL), dcc_status_string(DCC_ERR_INVALID_ARG)) != 0 ||
        strcmp(
            DCC_FRIENDLY_ERROR_DESCRIPTION(DCC_ERR_INVALID_ARG, NULL),
            "The request was invalid. Check the input and try again."
        ) != 0 ||
        strcmp(
            DCC_FRIENDLY_ERROR_DESCRIPTION(DCC_ERR_NOT_FOUND, NULL),
            "Required input was not found. Check the command or form fields and try again."
        ) != 0 ||
        strcmp(
            DCC_FRIENDLY_ERROR_DESCRIPTION(DCC_ERR_STATE, NULL),
            "This action cannot be used right now."
        ) != 0 ||
        strcmp(DCC_FRIENDLY_ERROR_DESCRIPTION(DCC_ERR_STATE, "custom"), "custom") != 0 ||
        strcmp(
            DCC_SIMPLE_ERROR_DESCRIPTION(DCC_ERR_STATE, "custom"),
            "Please try again in a moment."
        ) != 0;
    if (failed) {
        return 1;
    }

    if (DCC_CTX_REPLY_APP_ERROR(NULL, DCC_ERR_INVALID_ARG, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_REPLY_VERBOSE_APP_ERROR(NULL, DCC_ERR_STATE, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_REPLY_SIMPLE_APP_ERROR(NULL, DCC_ERR_STATE, NULL) != DCC_ERR_INVALID_ARG) {
        return 1;
    }

    sugar_status_friendly_error(NULL, DCC_ERR_INVALID_ARG, NULL, NULL);
    sugar_status_simple_error(NULL, DCC_ERR_STATE, "custom", NULL);
    sugar_status_verbose_error(NULL, DCC_OK, NULL, NULL);
    return 0;
}

int main(void) {
    if (check_status_message() != 0) {
        return 1;
    }
    if (check_public_status_message() != 0) {
        return 1;
    }
    if (check_private_and_silent_status_messages() != 0) {
        return 1;
    }
    if (check_friendly_status_aliases() != 0) {
        return 1;
    }
    if (check_safe_text_message() != 0) {
        return 1;
    }
    if (check_null_context_helpers() != 0) {
        return 1;
    }
    if (check_error_handler_sugar() != 0) {
        return 1;
    }
    return 0;
}
