#include "internal/voice/dcc_voice_opus_internal.h"

#if defined(DCC_HAVE_OPUS)
dcc_status_t dcc_voice_status_from_opus(int status) {
    if (status >= 0) {
        return DCC_OK;
    }
    switch (status) {
        case OPUS_BAD_ARG:
        case OPUS_INVALID_PACKET:
            return DCC_ERR_INVALID_ARG;
        case OPUS_BUFFER_TOO_SMALL:
        case OPUS_ALLOC_FAIL:
            return DCC_ERR_NOMEM;
        default:
            return DCC_ERR_STATE;
    }
}
#endif
