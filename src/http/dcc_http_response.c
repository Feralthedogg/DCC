#include "internal/http/dcc_http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_http_parse_response(dcc_http_buffer_t *raw, dcc_http_response_t *out) {
    char *header_end = dcc_http_find_header_end(raw->data, raw->len);
    if (header_end == NULL) {
        return DCC_ERR_NETWORK;
    }

    char *line_end = memchr(raw->data, '\n', (size_t)(header_end - raw->data));
    if (line_end == NULL) {
        return DCC_ERR_NETWORK;
    }

    unsigned status = 0;
    if (sscanf(raw->data, "HTTP/%*u.%*u %u", &status) != 1 || status > UINT16_MAX) {
        return DCC_ERR_NETWORK;
    }

    size_t headers_len = (size_t)(header_end - raw->data);
    size_t body_offset = headers_len + 4;
    size_t body_len = raw->len - body_offset;

    char *headers = dcc_http_strndup(raw->data, headers_len);
    if (headers == NULL) {
        return DCC_ERR_NOMEM;
    }

    char *body = NULL;
    if (dcc_http_headers_token_contains(headers, headers_len, "Transfer-Encoding", "chunked")) {
        dcc_http_buffer_t decoded = {0};
        dcc_status_t decode_status = dcc_http_decode_chunked_body(raw->data + body_offset, body_len, &decoded);
        if (decode_status != DCC_OK) {
            dcc_http_buffer_deinit(&decoded);
            free(headers);
            return decode_status;
        }
        body = dcc_http_strndup(decoded.data != NULL ? decoded.data : "", decoded.len);
        body_len = decoded.len;
        dcc_http_buffer_deinit(&decoded);
    } else {
        body = dcc_http_strndup(raw->data + body_offset, body_len);
    }
    if (body == NULL) {
        free(headers);
        return DCC_ERR_NOMEM;
    }

    memset(out, 0, sizeof(*out));
    out->status = (uint16_t)status;
    out->headers = headers;
    out->headers_len = headers_len;
    out->body = body;
    out->body_len = body_len;
    return DCC_OK;
}

void dcc_http_response_deinit(dcc_http_response_t *response) {
    if (response == NULL) {
        return;
    }
    free(response->headers);
    free(response->body);
    memset(response, 0, sizeof(*response));
}
