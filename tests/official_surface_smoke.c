#include "support/http_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <signal.h>

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);
    return run_public_rest_official_surface_smoke();
}

#endif
