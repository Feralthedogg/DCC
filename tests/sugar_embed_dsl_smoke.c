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

static int check_rich_embed_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_TEXT_EMBED(
            "ready",
            DCC_EMBED_RICH_FIELDS(
                "Runtime",
                "Gateway online",
                0x57F287U,
                "DCC",
                "https://example.com/footer.png",
                "attachment://status.png",
                "https://example.com/thumb.png",
                "runtime",
                "https://example.com/runtime",
                "https://example.com/runtime.png",
                DCC_EMBED_FIELD_INLINE("Shard", "0"),
                DCC_EMBED_FIELD("Mode", "production")
            )
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize rich embed message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"ready\"") ||
        require_contains(json, "\"embeds\":[{\"title\":\"Runtime\"") ||
        require_contains(json, "\"description\":\"Gateway online\"") ||
        require_contains(json, "\"color\":5763719") ||
        require_contains(json, "\"footer\":{\"text\":\"DCC\"") ||
        require_contains(json, "\"icon_url\":\"https://example.com/footer.png\"") ||
        require_contains(json, "\"image\":{\"url\":\"attachment://status.png\"") ||
        require_contains(json, "\"thumbnail\":{\"url\":\"https://example.com/thumb.png\"") ||
        require_contains(json, "\"author\":{\"name\":\"runtime\"") ||
        require_contains(json, "\"url\":\"https://example.com/runtime\"") ||
        require_contains(json, "\"fields\":[{\"name\":\"Shard\"") ||
        require_contains(json, "\"inline\":true") ||
        require_contains(json, "\"name\":\"Mode\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_ephemeral_embed_message(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_EPHEMERAL_TEXT_EMBED(
            "private",
            DCC_EMBED_COLOR_FOOTER_FIELDS(
                "Settings",
                "Only you can see this.",
                0x5865F2U,
                "private",
                NULL,
                DCC_EMBED_FIELD("Scope", "user")
            )
        );

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize ephemeral embed message\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"private\"") ||
        require_contains(json, "\"flags\":64") ||
        require_contains(json, "\"title\":\"Settings\"") ||
        require_contains(json, "\"footer\":{\"text\":\"private\"") ||
        require_contains(json, "\"name\":\"Scope\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_optional_rich_parts(void) {
    dcc_embed_builder_t embed =
        DCC_EMBED_RICH(
            "Minimal",
            "Only color should be rich",
            0x5865F2U,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        );

    char *json = NULL;
    if (dcc_embed_builder_build_json(&embed, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize minimal rich embed\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"title\":\"Minimal\"") ||
        require_contains(json, "\"color\":5793266") ||
        (strstr(json, "\"footer\"") != NULL) ||
        (strstr(json, "\"image\"") != NULL) ||
        (strstr(json, "\"thumbnail\"") != NULL) ||
        (strstr(json, "\"author\"") != NULL);
    dcc_embed_builder_json_free(json);
    return failed ? 1 : 0;
}

int main(void) {
    if (check_rich_embed_message() != 0) {
        return 1;
    }
    if (check_ephemeral_embed_message() != 0) {
        return 1;
    }
    if (check_optional_rich_parts() != 0) {
        return 1;
    }
    return 0;
}
