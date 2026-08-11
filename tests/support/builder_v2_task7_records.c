#include <dcc/message.h>

#include <stdio.h>
#include <string.h>

#define TASK7_CHECK(condition_, label_) do { \
    if (!(condition_)) { \
        fprintf(stderr, "builder Task 7 record check failed: %s\n", (label_)); \
        return 1; \
    } \
} while (0)

static int task7_json_has(const char *json, const char *needle) {
    return json != NULL && strstr(json, needle) != NULL;
}

int builder_task7_typed_records_contract(void) {
    dcc_message_reference_t reference = DCC_MESSAGE_REFERENCE_INIT;
    dcc_message_attachment_t attachments[2] = {
        DCC_MESSAGE_ATTACHMENT_INIT,
        DCC_MESSAGE_ATTACHMENT_INIT,
    };
    TASK7_CHECK(reference.size == sizeof(reference) &&
              reference.version == DCC_MESSAGE_REFERENCE_VERSION &&
              reference.present == 0U &&
              attachments[0].size == sizeof(attachments[0]) &&
              attachments[0].version == DCC_MESSAGE_ATTACHMENT_VERSION,
          "typed message record constant initializers");
    dcc_message_reference_init(NULL);
    dcc_message_attachment_init(NULL);

    reference.present = DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID |
        DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS;
    reference.message_id = 42U;
    reference.fail_if_not_exists = 0U;
    attachments[0].id = 0U;
    attachments[0].present = DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME |
        DCC_MESSAGE_ATTACHMENT_PRESENT_DESCRIPTION;
    attachments[0].filename = "voice.ogg";
    attachments[0].description = "clip";
    attachments[1].id = 77U;
    attachments[1].present = DCC_MESSAGE_ATTACHMENT_PRESENT_TITLE |
        DCC_MESSAGE_ATTACHMENT_PRESENT_IS_SPOILER;
    attachments[1].title = "retained";
    attachments[1].is_spoiler = 1U;

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    TASK7_CHECK(dcc_message_builder_set_message_reference(&message, &reference) == DCC_OK &&
              dcc_message_builder_set_attachments(&message, attachments, 2U) == DCC_OK,
          "typed message setters accept valid records");
    char *json = NULL;
    TASK7_CHECK(dcc_message_builder_build_json(&message, &json) == DCC_OK &&
              task7_json_has(json, "\"message_reference\":{\"message_id\":\"42\",\"fail_if_not_exists\":false}") &&
              task7_json_has(json, "\"attachments\":[{\"id\":\"0\",\"filename\":\"voice.ogg\",\"description\":\"clip\"}") &&
              task7_json_has(json, "{\"id\":\"77\",\"title\":\"retained\",\"is_spoiler\":true}"),
          "typed message records serialize exact present fields");
    dcc_message_builder_json_free(json);

    TASK7_CHECK(dcc_message_builder_set_message_reference_json(&message, "{}") == DCC_OK &&
              dcc_message_builder_build_json(&message, &json) == DCC_ERR_INVALID_ARG &&
              json == NULL,
          "typed and raw message references are mutually exclusive");
    TASK7_CHECK(dcc_message_builder_set_message_reference_json(&message, NULL) == DCC_OK &&
              dcc_message_builder_set_attachments_json(&message, "[]") == DCC_OK &&
              dcc_message_builder_build_json(&message, &json) == DCC_ERR_INVALID_ARG &&
              json == NULL,
          "typed and raw attachment metadata are mutually exclusive");
    TASK7_CHECK(dcc_message_builder_set_attachments_json(&message, NULL) == DCC_OK,
          "clear raw attachment metadata");

    attachments[1].id = 0U;
    TASK7_CHECK(dcc_message_builder_build_json(&message, &json) == DCC_ERR_INVALID_ARG,
          "duplicate attachment ids rejected");
    attachments[1].id = 77U;
    attachments[1].is_spoiler = 2U;
    TASK7_CHECK(dcc_message_builder_build_json(&message, &json) == DCC_ERR_INVALID_ARG,
          "attachment booleans are strict");
    attachments[1].is_spoiler = 1U;

    reference.present = DCC_MESSAGE_REFERENCE_PRESENT_TYPE |
        DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID |
        DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID;
    reference.type = DCC_MESSAGE_REFERENCE_FORWARD;
    reference.message_id = 42U;
    reference.channel_id = 84U;
    TASK7_CHECK(dcc_message_builder_set_content(&message, "not allowed") == DCC_OK &&
              dcc_message_builder_build_json(&message, &json) == DCC_ERR_INVALID_ARG,
          "forward reference rejects ordinary payload fields");
    TASK7_CHECK(dcc_message_builder_set_content(&message, NULL) == DCC_OK &&
              dcc_message_builder_set_attachments(&message, NULL, 0U) == DCC_OK &&
              dcc_message_builder_build_json(&message, &json) == DCC_OK &&
              task7_json_has(json, "\"type\":1") &&
              task7_json_has(json, "\"channel_id\":\"84\""),
          "forward reference serializes after restricted fields clear");
    dcc_message_builder_json_free(json);
    return 0;
}
