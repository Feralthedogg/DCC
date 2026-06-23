#ifndef DCC_SUGAR_CONTEXT_ACTIONS_H
#define DCC_SUGAR_CONTEXT_ACTIONS_H

#include <dcc/sugar/context/require.h>

#define DCC_CTX_ADD_MEMBER_ROLE(ctx_, user_id_, role_id_) \
    dcc_ctx_add_member_role((ctx_), (user_id_), (role_id_), NULL, NULL)
#define DCC_CTX_ADD_MEMBER_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    dcc_ctx_add_member_role((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_REMOVE_MEMBER_ROLE(ctx_, user_id_, role_id_) \
    dcc_ctx_remove_member_role((ctx_), (user_id_), (role_id_), NULL, NULL)
#define DCC_CTX_REMOVE_MEMBER_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    dcc_ctx_remove_member_role((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_ADD_AUTHOR_ROLE(ctx_, role_id_) \
    dcc_ctx_add_author_role((ctx_), (role_id_), NULL, NULL)
#define DCC_CTX_ADD_AUTHOR_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    dcc_ctx_add_author_role((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_REMOVE_AUTHOR_ROLE(ctx_, role_id_) \
    dcc_ctx_remove_author_role((ctx_), (role_id_), NULL, NULL)
#define DCC_CTX_REMOVE_AUTHOR_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    dcc_ctx_remove_author_role((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_MEMBER_ADD_ROLE(ctx_, user_id_, role_id_) \
    DCC_CTX_ADD_MEMBER_ROLE((ctx_), (user_id_), (role_id_))
#define DCC_CTX_MEMBER_ADD_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    DCC_CTX_ADD_MEMBER_ROLE_CB((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_MEMBER_REMOVE_ROLE(ctx_, user_id_, role_id_) \
    DCC_CTX_REMOVE_MEMBER_ROLE((ctx_), (user_id_), (role_id_))
#define DCC_CTX_MEMBER_REMOVE_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    DCC_CTX_REMOVE_MEMBER_ROLE_CB((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_AUTHOR_ADD_ROLE(ctx_, role_id_) DCC_CTX_ADD_AUTHOR_ROLE((ctx_), (role_id_))
#define DCC_AUTHOR_ADD_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    DCC_CTX_ADD_AUTHOR_ROLE_CB((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_AUTHOR_REMOVE_ROLE(ctx_, role_id_) DCC_CTX_REMOVE_AUTHOR_ROLE((ctx_), (role_id_))
#define DCC_AUTHOR_REMOVE_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    DCC_CTX_REMOVE_AUTHOR_ROLE_CB((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_THREAD_FROM_MESSAGE(ctx_, name_) \
    dcc_ctx_create_thread_from_message_name((ctx_), (name_), NULL, NULL)
#define DCC_CTX_THREAD_FROM_MESSAGE_CB(ctx_, name_, cb_, user_data_) \
    dcc_ctx_create_thread_from_message_name((ctx_), (name_), (cb_), (user_data_))
#define DCC_CTX_THREAD_FROM_MESSAGE_PARAMS(ctx_, params_) \
    dcc_ctx_create_thread_from_message((ctx_), (params_), NULL, NULL)
#define DCC_CTX_THREAD_FROM_MESSAGE_PARAMS_CB(ctx_, params_, cb_, user_data_) \
    dcc_ctx_create_thread_from_message((ctx_), (params_), (cb_), (user_data_))
#define DCC_CTX_CREATE_THREAD(ctx_, name_) DCC_CTX_THREAD_FROM_MESSAGE((ctx_), (name_))
#define DCC_CTX_CREATE_THREAD_CB(ctx_, name_, cb_, user_data_) \
    DCC_CTX_THREAD_FROM_MESSAGE_CB((ctx_), (name_), (cb_), (user_data_))
#define DCC_CTX_CREATE_THREAD_PARAMS(ctx_, params_) \
    DCC_CTX_THREAD_FROM_MESSAGE_PARAMS((ctx_), (params_))
#define DCC_CTX_CREATE_THREAD_PARAMS_CB(ctx_, params_, cb_, user_data_) \
    DCC_CTX_THREAD_FROM_MESSAGE_PARAMS_CB((ctx_), (params_), (cb_), (user_data_))
#define DCC_CTX_ARCHIVE_THREAD(ctx_) dcc_ctx_archive_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_ARCHIVE_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_archive_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_LOCK_THREAD(ctx_) dcc_ctx_lock_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_LOCK_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_lock_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_UNLOCK_THREAD(ctx_) dcc_ctx_unlock_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_UNLOCK_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_unlock_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_CLOSE_THREAD(ctx_) DCC_CTX_ARCHIVE_THREAD((ctx_))
#define DCC_CTX_CLOSE_THREAD_CB(ctx_, cb_, user_data_) \
    DCC_CTX_ARCHIVE_THREAD_CB((ctx_), (cb_), (user_data_))


#endif /* DCC_SUGAR_CONTEXT_ACTIONS_H */
