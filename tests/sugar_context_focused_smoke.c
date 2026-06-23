#include <dcc/sugar/context.h>

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

int main(void) {
    if (check_modal_literals() != 0) {
        return 1;
    }
    if (check_null_context_aliases() != 0) {
        fprintf(stderr, "focused context sugar aliases failed\n");
        return 1;
    }
    return 0;
}
