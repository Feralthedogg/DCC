#ifndef DCC_PLATFORM_RESOLVE_H
#define DCC_PLATFORM_RESOLVE_H

#include "dcc/error.h"

#include <llam/io.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

dcc_status_t dcc_platform_getaddrinfo(
    const char *node,
    const char *service,
    const struct addrinfo *hints,
    struct addrinfo **out
);
void dcc_platform_freeaddrinfo(struct addrinfo *result);

#endif
