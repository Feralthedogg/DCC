#include <dcc/sugar/context.h>
#include <dcc/sugar/context_binding.h>

#include <stdio.h>

static dcc_component_v2_builder_t focused_text_component(void) {
    return (dcc_component_v2_builder_t){
        .type = DCC_COMPONENT_V2_TEXT_DISPLAY,
        .content = "hello"
    };
}

static int check_modal_literals(void) {
    dcc_modal_builder_t modal =
        DCC_MODAL_V2("profile.edit", "Edit profile", DCC_MODAL_V2_FIELD_TEXT("name", "Name"));

    if (modal.custom_id == NULL || modal.title == NULL ||
        modal.components_v2_count != 1U ||
        modal.components_v2[0].type != DCC_COMPONENT_V2_LABEL) {
        fprintf(stderr, "DCC_MODAL_V2 focused literal failed\n");
        return 1;
    }
    return 0;
}

static int check_null_context_aliases(void) {
    int ok =
        DCC_REPLY_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_V2_CB(NULL, NULL, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_V2_CB(NULL, NULL, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB(NULL, NULL, NULL, focused_text_component()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATE_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_V2_CB(NULL, NULL, NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_EPHEMERAL_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_EPHEMERAL_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI(NULL, focused_text_component()) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V2(NULL, "profile.edit", "Edit profile", DCC_MODAL_V2_FIELD_TEXT("name", "Name")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V2_CB(
            NULL,
            "profile.edit",
            "Edit profile",
            NULL,
            NULL,
            DCC_MODAL_V2_FIELD_TEXT("name", "Name")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SHOW_MODAL_V2(NULL, "profile.edit", "Edit profile", DCC_MODAL_V2_FIELD_TEXT("name", "Name")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SHOW_MODAL_V2_CB(
            NULL,
            "profile.edit",
            "Edit profile",
            NULL,
            NULL,
            DCC_MODAL_V2_FIELD_TEXT("name", "Name")
        ) == DCC_ERR_INVALID_ARG;
    return ok ? 0 : 1;
}

typedef struct focused_bind_args {
    const char *query;
    int64_t limit;
    uint8_t visible;
    const char **values;
    size_t value_count;
} focused_bind_args_t;

static void check_bind_options_or_reply_null(dcc_ctx_t *ctx) {
    const char *query = NULL;
    int64_t limit = 0;

    DCC_CTX_BIND_OPTIONS_OR_REPLY(
        ctx,
        DCC_CTX_OPTION_REQUIRED_STRING("query", &query),
        DCC_CTX_OPTION_INT("limit", &limit, 10)
    );

    (void)query;
    (void)limit;
}

static void check_bind_option_fields_or_reply_null(dcc_ctx_t *ctx) {
    focused_bind_args_t args;

    DCC_BIND_OPTION_FIELDS_OR_REPLY(
        ctx,
        &args,
        DCC_CTX_OPTION_FIELD_REQUIRED_STRING(focused_bind_args_t, query, "query"),
        DCC_CTX_OPTION_FIELD_INT(focused_bind_args_t, limit, "limit", 10)
    );

    (void)args;
}

static void check_bind_form_or_reply_null(dcc_ctx_t *ctx) {
    const char *query = NULL;
    uint8_t visible = 0U;

    DCC_CTX_BIND_FORM_OR_REPLY(
        ctx,
        DCC_CTX_FORM_REQUIRED_STRING("query", &query),
        DCC_CTX_FORM_BOOL("visible", &visible, 1U)
    );

    (void)query;
    (void)visible;
}

static void check_bind_form_fields_or_reply_null(dcc_ctx_t *ctx) {
    focused_bind_args_t args;

    DCC_BIND_FORM_FIELDS_OR_REPLY(
        ctx,
        &args,
        DCC_CTX_FORM_FIELD_REQUIRED_STRING(focused_bind_args_t, query, "query"),
        DCC_CTX_FORM_FIELD_BOOL(focused_bind_args_t, visible, "visible", 1U),
        DCC_CTX_FORM_FIELD_REQUIRED_VALUES(
            focused_bind_args_t,
            values,
            value_count,
            "choices"
        )
    );

    (void)args;
}

static int check_bind_or_reply_aliases(void) {
    check_bind_options_or_reply_null(NULL);
    check_bind_option_fields_or_reply_null(NULL);
    check_bind_form_or_reply_null(NULL);
    check_bind_form_fields_or_reply_null(NULL);
    return 0;
}

int main(void) {
    if (check_modal_literals() != 0) {
        return 1;
    }
    if (check_null_context_aliases() != 0) {
        fprintf(stderr, "focused context sugar aliases failed\n");
        return 1;
    }
    if (check_bind_or_reply_aliases() != 0) {
        fprintf(stderr, "bind-or-reply aliases failed\n");
        return 1;
    }
    return 0;
}
