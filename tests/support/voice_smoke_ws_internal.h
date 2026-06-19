#ifndef DCC_TEST_VOICE_SMOKE_WS_INTERNAL_H
#define DCC_TEST_VOICE_SMOKE_WS_INTERNAL_H

#if !defined(_WIN32)

#include <stddef.h>
#include <stdint.h>

int voice_ws_read_exact_fd(int fd, void *buf, size_t len);
char *voice_ws_find_header(char *headers, const char *name);
int voice_ws_accept_for_key(const char *key, char out[64]);
int voice_ws_read_http_upgrade(int client, char *buf, size_t cap);
int voice_ws_read_client_text_frame(int client, char *out, size_t out_cap);
int voice_ws_read_client_text_frame_timeout(int client, char *out, size_t out_cap, unsigned timeout_ms);
int voice_ws_write_server_text_frame(int client, const char *text);
int voice_ws_write_server_close_frame_reason(int client, uint16_t code, const char *reason);
int voice_ws_write_server_close_frame(int client, uint16_t code);
int voice_ws_accept_upgrade(int server_fd);

#endif

#endif
