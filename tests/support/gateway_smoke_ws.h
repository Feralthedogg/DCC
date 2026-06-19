#ifndef DCC_GATEWAY_SMOKE_WS_H
#define DCC_GATEWAY_SMOKE_WS_H

#if !defined(_WIN32)

#include <stddef.h>
#include <stdint.h>

char *find_header(char *headers, const char *name);
int ws_accept_for_key(const char *key, char out[64]);
int read_http_upgrade(int client, char *buf, size_t cap);

int write_text_frame(int client, const char *text);
int read_client_frame(int client, unsigned char *opcode, char *out, size_t out_cap);
int read_text_frame(int client, char *out, size_t out_cap);
int read_text_frame_timeout(int client, char *out, size_t out_cap, int timeout_ms);

int write_dispatch(int client, const char *name, unsigned seq, const char *payload);
int write_close_frame(int client, uint16_t code, const char *reason);
void drain_client_close(int client);

#endif

#endif
