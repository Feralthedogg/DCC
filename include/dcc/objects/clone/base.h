#ifndef DCC_OBJECTS_CLONE_BASE_H
#define DCC_OBJECTS_CLONE_BASE_H

#include <dcc/objects/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_user_t *dcc_user_clone(const dcc_user_t *user);
DCC_API void dcc_user_free(dcc_user_t *user);
DCC_API dcc_attachment_t *dcc_attachment_clone(const dcc_attachment_t *attachment);
DCC_API void dcc_attachment_free(dcc_attachment_t *attachment);
DCC_API dcc_message_t *dcc_message_clone(const dcc_message_t *message);
DCC_API void dcc_message_free(dcc_message_t *message);

#ifdef __cplusplus
}
#endif

#endif
