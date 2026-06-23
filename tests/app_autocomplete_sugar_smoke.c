#include <dcc/sugar.h>

#include "internal/app/dcc_app_internal.h"

#include <stdio.h>
#include <string.h>

static int autocomplete_sugar_smoke_expect(
    const char *label,
    int condition
) {
    if (!condition) {
        fprintf(stderr, "autocomplete sugar smoke failed: %s\n", label);
        return 0;
    }
    return 1;
}

int main(void) {
    dcc_interaction_option_t focused;
    memset(&focused, 0, sizeof(focused));
    focused.name = "query";
    focused.value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    focused.string_value = "fe";
    focused.focused = 1U;

    dcc_interaction_t interaction;
    memset(&interaction, 0, sizeof(interaction));
    interaction.options = &focused;
    interaction.options_count = 1U;
    interaction.focused_option = &focused;

    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.interaction = &interaction;

    dcc_autocomplete_choice_t choices[] = {
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feature", "feature"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Other", "other"),
        DCC_AUTOCOMPLETE_INTEGER_CHOICE("Count", 5),
    };
    dcc_autocomplete_choice_t filtered[DCC_AUTOCOMPLETE_MAX_CHOICES];
    size_t filtered_count = 99U;

    if (!autocomplete_sugar_smoke_expect(
            "focused option",
            DCC_CTX_FOCUSED_OPTION(&ctx) == &focused
        ) ||
        !autocomplete_sugar_smoke_expect(
            "focused name",
            strcmp(DCC_CTX_FOCUSED_NAME(&ctx), "query") == 0
        ) ||
        !autocomplete_sugar_smoke_expect(
            "focused string",
            strcmp(DCC_CTX_FOCUSED_STRING(&ctx, ""), "fe") == 0
        ) ||
        !autocomplete_sugar_smoke_expect(
            "null focused fallback",
            strcmp(DCC_CTX_FOCUSED_STRING(NULL, "fallback"), "fallback") == 0
        )) {
        return 1;
    }

    dcc_status_t status = dcc_ctx_autocomplete_filter_choices(
        &ctx,
        choices,
        sizeof(choices) / sizeof(choices[0]),
        filtered,
        DCC_AUTOCOMPLETE_MAX_CHOICES,
        &filtered_count
    );
    if (!autocomplete_sugar_smoke_expect("filter status", status == DCC_OK) ||
        !autocomplete_sugar_smoke_expect("filter count", filtered_count == 2U) ||
        !autocomplete_sugar_smoke_expect(
            "filter first",
            strcmp(filtered[0].name, "Feral") == 0
        ) ||
        !autocomplete_sugar_smoke_expect(
            "filter second",
            strcmp(filtered[1].value_string, "feature") == 0
        )) {
        return 1;
    }

    filtered_count = 99U;
    status = dcc_ctx_autocomplete_filter_choices(
        &ctx,
        choices,
        sizeof(choices) / sizeof(choices[0]),
        filtered,
        1U,
        &filtered_count
    );
    if (!autocomplete_sugar_smoke_expect("limited filter status", status == DCC_OK) ||
        !autocomplete_sugar_smoke_expect("limited filter count", filtered_count == 1U) ||
        !autocomplete_sugar_smoke_expect(
            "limited filter first",
            strcmp(filtered[0].name, "Feral") == 0
        )) {
        return 1;
    }

    focused.string_value = "";
    filtered_count = 99U;
    status = dcc_ctx_autocomplete_filter_choices(
        &ctx,
        choices,
        sizeof(choices) / sizeof(choices[0]),
        filtered,
        DCC_AUTOCOMPLETE_MAX_CHOICES,
        &filtered_count
    );
    if (!autocomplete_sugar_smoke_expect("empty query status", status == DCC_OK) ||
        !autocomplete_sugar_smoke_expect("empty query returns all", filtered_count == 4U)) {
        return 1;
    }

    if (!autocomplete_sugar_smoke_expect(
            "invalid output",
            dcc_ctx_autocomplete_filter_choices(
                &ctx,
                choices,
                sizeof(choices) / sizeof(choices[0]),
                NULL,
                1U,
                &filtered_count
            ) == DCC_ERR_INVALID_ARG
        ) ||
        !autocomplete_sugar_smoke_expect(
            "invalid max reply",
            dcc_ctx_reply_autocomplete_matching(
                &ctx,
                choices,
                sizeof(choices) / sizeof(choices[0]),
                DCC_AUTOCOMPLETE_MAX_CHOICES + 1U,
                NULL,
                NULL
            ) == DCC_ERR_INVALID_ARG
        )) {
        return 1;
    }

    return 0;
}
