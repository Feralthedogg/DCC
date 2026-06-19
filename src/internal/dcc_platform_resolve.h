#ifndef DCC_PLATFORM_RESOLVE_H
#define DCC_PLATFORM_RESOLVE_H

#include "dcc/error.h"

#include <llam/io.h>

#if LLAM_PLATFORM_POSIX
#include <netdb.h>

dcc_status_t dcc_platform_getaddrinfo(
    const char *node,
    const char *service,
    const struct addrinfo *hints,
    struct addrinfo **out
);
void dcc_platform_freeaddrinfo(struct addrinfo *result);
#endif

#endif
