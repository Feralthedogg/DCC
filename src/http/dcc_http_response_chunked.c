#include "internal/http/dcc_http.h"

static int dcc_http_hex_digit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

dcc_status_t dcc_http_decode_chunked_body(const char *body, size_t body_len, dcc_http_buffer_t *out) {
    size_t pos = 0;
    while (pos < body_len) {
        uint64_t chunk_len = 0;
        int saw_digit = 0;
        while (pos < body_len) {
            char c = body[pos++];
            if (c == ';') {
                while (pos < body_len && body[pos] != '\r' && body[pos] != '\n') {
                    pos++;
                }
                break;
            }
            if (c == '\r' || c == '\n') {
                pos--;
                break;
            }
            int h = dcc_http_hex_digit(c);
            if (h < 0 || chunk_len > (UINT64_MAX >> 4U)) {
                return DCC_ERR_NETWORK;
            }
            chunk_len = (chunk_len << 4U) | (uint64_t)h;
            saw_digit = 1;
        }
        if (!saw_digit || pos + 2U > body_len || body[pos] != '\r' || body[pos + 1] != '\n') {
            return DCC_ERR_NETWORK;
        }
        pos += 2U;

        if (chunk_len == 0) {
            while (pos + 1U < body_len) {
                if (body[pos] == '\r' && body[pos + 1] == '\n') {
                    return DCC_OK;
                }
                pos++;
            }
            return DCC_OK;
        }
        if (chunk_len > (uint64_t)(body_len - pos)) {
            return DCC_ERR_NETWORK;
        }
        if (dcc_http_buffer_append(out, body + pos, (size_t)chunk_len) != 0) {
            return DCC_ERR_NOMEM;
        }
        pos += (size_t)chunk_len;
        if (pos + 2U > body_len || body[pos] != '\r' || body[pos + 1] != '\n') {
            return DCC_ERR_NETWORK;
        }
        pos += 2U;
    }
    return DCC_ERR_NETWORK;
}
