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

static int check_text_flags(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_TEXT_FLAGS(
            "quiet link",
            DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS | DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize text flags\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"quiet link\"") ||
        require_contains(json, "\"flags\":4100");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_generic_private_and_silent(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_SILENT(DCC_MESSAGE_PRIVATE(DCC_MESSAGE_TEXT("secret")));

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize private silent message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"secret\"") ||
        require_contains(json, "\"flags\":4160");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_embed_flags(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_SILENT_TEXT_EMBED(
            "status",
            DCC_EMBED_COLOR("Saved", "No notification sent.", DCC_COLOR_SUCCESS)
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize embed flags\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"status\"") ||
        require_contains(json, "\"title\":\"Saved\"") ||
        require_contains(json, "\"flags\":4096");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_components_v2_flags(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_SILENT_V2(DCC_V2_TEXT("Quiet panel"));

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize v2 flags\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"Quiet panel\"") ||
        require_contains(json, "\"flags\":36864");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_reply_alias_nulls(void) {
    return
        DCC_REPLY_PRIVATE(NULL, "private") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PRIVATE_CB(NULL, "private", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PRIVATE_EMBED(NULL, "Private", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PRIVATE_EMBED_CB(NULL, "Private", "Body", DCC_COLOR_INFO, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND(NULL, DCC_MESSAGE_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE(NULL, "private") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_SILENT(NULL, "quiet") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_NO_EMBEDS(NULL, "plain") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PUBLIC_EMBED(NULL, "Public", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE_EMBED(NULL, "Private", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_DEFER(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATE(NULL, "private") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATE_EMBED(NULL, "Private", "Body", DCC_COLOR_INFO) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_PUBLIC_EMBED(NULL, "Public", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_PRIVATE_EMBED(NULL, "Private", "Body", DCC_COLOR_INFO) == DCC_ERR_INVALID_ARG
            ? 0
            : 1;
}

static void compile_reply_aliases(dcc_ctx_t *ctx) {
    if (ctx != NULL) {
        (void)DCC_REPLY_PRIVATE(ctx, "private");
        (void)DCC_REPLY_PRIVATE_EMBED(ctx, "private", "body", DCC_COLOR_INFO);
        (void)DCC_REPLY_SILENT(ctx, "quiet");
        (void)DCC_REPLY_NO_EMBEDS(ctx, "https://example.com");
        (void)DCC_DEFER_PRIVATE(ctx);
        (void)DCC_RESPOND(ctx, DCC_MESSAGE_TEXT("respond"));
        (void)DCC_RESPOND_PRIVATE(ctx, "private respond");
        (void)DCC_RESPOND_PRIVATE_EMBED(ctx, "private respond", "body", DCC_COLOR_INFO);
        (void)DCC_RESPOND_SILENT(ctx, "quiet respond");
        (void)DCC_RESPOND_NO_EMBEDS(ctx, "plain respond");
        (void)DCC_RESPOND_DEFER_PRIVATE(ctx);
        (void)DCC_FOLLOWUP_PRIVATE(ctx, "private followup");
        (void)DCC_FOLLOWUP_SILENT(ctx, "quiet followup");
        (void)DCC_CTX_PRIVATE(ctx, "private ctx");
        (void)DCC_CTX_RESPOND_PRIVATE(ctx, "private ctx respond");
        (void)DCC_CTX_RESPOND_PRIVATE_EMBED(ctx, "private ctx respond", "body", DCC_COLOR_INFO);
        (void)DCC_CTX_PRIVATE_EMBED(ctx, "private ctx", "body", DCC_COLOR_INFO);
        (void)DCC_CTX_SILENT(ctx, "quiet ctx");
        (void)DCC_CTX_RESPOND_SILENT(ctx, "quiet ctx respond");
        (void)DCC_CTX_NO_EMBEDS(ctx, "plain link ctx");
        (void)DCC_CTX_RESPOND_NO_EMBEDS(ctx, "plain link ctx respond");
        (void)DCC_CTX_DEFER_PRIVATE(ctx);
        (void)DCC_CTX_RESPOND_DEFER_PRIVATE(ctx);
        (void)DCC_CTX_FOLLOWUP_PRIVATE(ctx, "private ctx followup");
        (void)DCC_CTX_FOLLOWUP_SILENT(ctx, "quiet ctx followup");
    }
}

int main(void) {
    compile_reply_aliases(NULL);
    if (check_text_flags() != 0) {
        return 1;
    }
    if (check_generic_private_and_silent() != 0) {
        return 1;
    }
    if (check_embed_flags() != 0) {
        return 1;
    }
    if (check_components_v2_flags() != 0) {
        return 1;
    }
    if (check_reply_alias_nulls() != 0) {
        return 1;
    }
    return 0;
}
