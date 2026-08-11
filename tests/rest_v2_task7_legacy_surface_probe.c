#include <dcc/rest.h>

/*
 * This unit intentionally compiles only on the Task 6 baseline. The RED
 * wrapper requires all reviewed aliases to remain visible while the canonical
 * Task 7 unit fails. Once Task 7 is implemented this unit must fail and the
 * wrapper is replaced by the normal endpoint smoke registration.
 */
int main(void) {
    (void)&dcc_rest_add_thread_member_params;
    (void)&dcc_rest_create_channel_invite_params;
    (void)&dcc_rest_create_dm_channel_params;
    (void)&dcc_rest_create_forum_thread_params;
    (void)&dcc_rest_create_guild_channel_params;
    (void)&dcc_rest_create_thread_params;
    (void)&dcc_rest_create_thread_from_message_params;
    (void)&dcc_rest_get_thread;
    (void)&dcc_rest_get_invite_full;
    (void)&dcc_rest_get_joined_private_archived_threads_page;
    (void)&dcc_rest_get_private_archived_threads_page;
    (void)&dcc_rest_get_public_archived_threads_page;
    (void)&dcc_rest_modify_channel_params;
    (void)&dcc_rest_modify_thread;
    (void)&dcc_rest_modify_thread_params;
    (void)&dcc_rest_modify_channel_permission_params;
    (void)&dcc_rest_modify_guild_channel_positions_params;
    (void)&dcc_rest_set_channel_voice_status_params;
    return 0;
}
