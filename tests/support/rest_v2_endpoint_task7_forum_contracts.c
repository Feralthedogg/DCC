#include <dcc/client.h>
#include <dcc/message.h>
#include <dcc/rest.h>

#include "rest_v2_endpoint_smoke_support.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct task7_extended_attachment {
    dcc_message_attachment_t value;
    uint64_t future;
} task7_extended_attachment_t;

int endpoint_task7_forum_multipart_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    char thread_name[] = "owned forum";
    char content[] = "owned content";
    char field0[] = "files[0]";
    char field1[] = "files[1]";
    char filename0[] = "first.bin";
    char filename1[] = "second.bin";
    char type0[] = "application/octet-stream";
    char type1[] = "application/octet-stream";
    unsigned char data0[] = {'A', 0U, 'B'};
    unsigned char data1[] = {'C', 0U, 'D', 'E'};

    task7_extended_attachment_t attachments[2] = {
        {
            .value = DCC_MESSAGE_ATTACHMENT_INIT,
            .future = UINT64_C(0x1111111111111111),
        },
        {
            .value = DCC_MESSAGE_ATTACHMENT_INIT,
            .future = UINT64_C(0x2222222222222222),
        },
    };
    attachments[0].value.size = sizeof(attachments[0]);
    attachments[0].value.present = DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME;
    attachments[0].value.id = 0U;
    attachments[0].value.filename = filename0;
    attachments[1].value.size = sizeof(attachments[1]);
    attachments[1].value.present = DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME;
    attachments[1].value.id = 1U;
    attachments[1].value.filename = filename1;

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, content) != DCC_OK ||
        dcc_message_builder_set_attachments(
            &message,
            (const dcc_message_attachment_t *)attachments,
            2U
        ) != DCC_OK) {
        fprintf(stderr, "forum multipart builder setup failed\n");
        return 1;
    }
    dcc_rest_multipart_file_t files[2] = {
        {field0, filename0, type0, data0, sizeof(data0)},
        {field1, filename1, type1, data1, sizeof(data1)},
    };
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    payload.files = files;
    payload.file_count = 2U;
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_MESSAGE;
    params.name = thread_name;
    params.message = &payload;

    endpoint_capture_reset(capture, 0U);
    dcc_rest_request_t *request = NULL;
    dcc_status_t submit = dcc_rest_create_forum_thread(
        client, UINT64_C(707), &params, NULL, &request
    );
    memset(thread_name, 'x', sizeof(thread_name) - 1U);
    memset(content, 'x', sizeof(content) - 1U);
    memset(field0, 'x', sizeof(field0) - 1U);
    memset(field1, 'x', sizeof(field1) - 1U);
    memset(filename0, 'x', sizeof(filename0) - 1U);
    memset(filename1, 'x', sizeof(filename1) - 1U);
    memset(type0, 'x', sizeof(type0) - 1U);
    memset(type1, 'x', sizeof(type1) - 1U);
    memset(data0, 'x', sizeof(data0));
    memset(data1, 'x', sizeof(data1));
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);

    if (endpoint_completed_contract(
            "forum multipart owns two files",
            capture,
            submit,
            request,
            "POST",
            "/channels/707/threads",
            "multipart/form-data; boundary=",
            "name=\"payload_json\"",
            sizeof("name=\"payload_json\"") - 1U
        ) != 0 ||
        !endpoint_contains(
            capture->body, capture->body_len,
            "{\"name\":\"owned forum\",\"message\":{\"content\":\"owned content\",\"attachments\":[{\"id\":\"0\",\"filename\":\"first.bin\"},{\"id\":\"1\",\"filename\":\"second.bin\"}]}}",
            sizeof("{\"name\":\"owned forum\",\"message\":{\"content\":\"owned content\",\"attachments\":[{\"id\":\"0\",\"filename\":\"first.bin\"},{\"id\":\"1\",\"filename\":\"second.bin\"}]}}") - 1U
        ) ||
        !endpoint_contains(
            capture->body, capture->body_len,
            "name=\"files[0]\"; filename=\"first.bin\"\r\nContent-Type: application/octet-stream\r\n\r\nA\0B",
            sizeof("name=\"files[0]\"; filename=\"first.bin\"\r\nContent-Type: application/octet-stream\r\n\r\nA\0B") - 1U
        ) ||
        !endpoint_contains(
            capture->body, capture->body_len,
            "name=\"files[1]\"; filename=\"second.bin\"\r\nContent-Type: application/octet-stream\r\n\r\nC\0DE",
            sizeof("name=\"files[1]\"; filename=\"second.bin\"\r\nContent-Type: application/octet-stream\r\n\r\nC\0DE") - 1U
        )) {
        fprintf(stderr, "forum multipart body/ownership contract failed\n");
        return 1;
    }

    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "json-only") != DCC_OK)
        return 1;
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    params = (dcc_thread_params_t)DCC_THREAD_PARAMS_INIT;
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_MESSAGE;
    params.name = "zero files";
    params.message = &payload;
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    request = NULL;
    submit = dcc_rest_create_forum_thread(
        client, UINT64_C(708), &params, NULL, &request
    );
    if (endpoint_completed_contract(
            "forum JSON zero-file branch", capture, submit, request,
            "POST", "/channels/708/threads", "application/json",
            "{\"name\":\"zero files\",\"message\":{\"content\":\"json-only\"}}",
            sizeof("{\"name\":\"zero files\",\"message\":{\"content\":\"json-only\"}}") - 1U
        ) != 0) {
        return 1;
    }

    dcc_message_attachment_t attachment = DCC_MESSAGE_ATTACHMENT_INIT;
    attachment.present = DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME;
    attachment.id = 0U;
    attachment.filename = "payload.bin";
    if (dcc_message_builder_set_attachments(&message, &attachment, 1U) != DCC_OK)
        return 1;
    const unsigned char byte = 0U;
    dcc_rest_multipart_file_t bad_file = {
        "upload", "payload.bin", "application/octet-stream", &byte, 1U,
    };
    payload.files = &bad_file;
    payload.file_count = 1U;
    request = (dcc_rest_request_t *)(uintptr_t)1U;
    submit = dcc_rest_create_forum_thread(
        client, UINT64_C(709), &params, NULL, &request
    );
    if (submit != DCC_ERR_INVALID_ARG || request != NULL) {
        fprintf(stderr, "forum noncanonical multipart field was accepted\n");
        dcc_rest_request_destroy(request);
        return 1;
    }
    return 0;
}
