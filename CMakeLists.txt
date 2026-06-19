cmake_minimum_required(VERSION 3.20)

project(dcc
    VERSION 0.1.0
    DESCRIPTION "Discord C Client"
    LANGUAGES C
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CheckCSourceCompiles)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DCC_PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DCC_PRIVATE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(DCC_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

option(DCC_BUILD_SHARED "Build DCC as a shared library" OFF)
option(DCC_BUILD_EXAMPLES "Build DCC examples" ON)
option(DCC_BUILD_TESTS "Build DCC smoke tests" ON)
option(DCC_BUILD_TOOLS "Build and install DCC command-line tools" ON)
option(DCC_BUILD_BENCHMARKS "Build DCC micro benchmarks" OFF)
option(DCC_BUILD_FUZZERS "Build libFuzzer harnesses" OFF)
option(DCC_ENABLE_SANITIZERS "Build DCC targets with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(DCC_LLAM_USE_SUBDIRECTORY "Build LLAM from source through add_subdirectory" OFF)
option(DCC_BUNDLE_LLAM "Install LLAM runtime files with DCC packages when LLAM is built as a subdirectory" ON)
option(DCC_WITH_OPUS "Build the optional libopus voice codec backend when libopus is available" ON)
set(DCC_LLAM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../LLAM" CACHE PATH "Path to the LLAM runtime source tree")
set(DCC_LLAM_LIBRARY "${DCC_LLAM_ROOT}/libllam_runtime.a" CACHE FILEPATH "Path to a prebuilt LLAM static library")
set(DCC_OPUS_ROOT "" CACHE PATH "Path to a libopus installation")

if(DCC_BUILD_FUZZERS AND NOT DCC_ENABLE_SANITIZERS)
    set(DCC_ENABLE_SANITIZERS ON CACHE BOOL "Build DCC targets with AddressSanitizer and UndefinedBehaviorSanitizer" FORCE)
endif()

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

if(DCC_WITH_OPUS)
    find_path(DCC_OPUS_INCLUDE_DIR
        NAMES opus/opus.h
        HINTS "${DCC_OPUS_ROOT}" ENV OPUS_ROOT /opt/homebrew /usr/local
        PATH_SUFFIXES include
    )
    find_library(DCC_OPUS_LIBRARY
        NAMES opus
        HINTS "${DCC_OPUS_ROOT}" ENV OPUS_ROOT /opt/homebrew /usr/local
        PATH_SUFFIXES lib
    )
    if(DCC_OPUS_INCLUDE_DIR AND DCC_OPUS_LIBRARY)
        add_library(dcc_opus UNKNOWN IMPORTED GLOBAL)
        set_target_properties(dcc_opus PROPERTIES
            IMPORTED_LOCATION "${DCC_OPUS_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DCC_OPUS_INCLUDE_DIR}"
        )
        message(STATUS "DCC optional Opus backend enabled: ${DCC_OPUS_LIBRARY}")
    else()
        message(STATUS "DCC optional Opus backend disabled: libopus was not found")
    endif()
endif()

if(NOT TARGET llam::runtime)
    if(DCC_LLAM_USE_SUBDIRECTORY)
        if(NOT EXISTS "${DCC_LLAM_ROOT}/CMakeLists.txt")
            message(FATAL_ERROR "DCC_LLAM_ROOT does not point to a LLAM source tree: ${DCC_LLAM_ROOT}")
        endif()
        add_subdirectory("${DCC_LLAM_ROOT}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/llam" EXCLUDE_FROM_ALL)
    else()
        if(NOT EXISTS "${DCC_LLAM_LIBRARY}")
            message(FATAL_ERROR "Prebuilt LLAM library not found: ${DCC_LLAM_LIBRARY}. Build LLAM first or configure with -DDCC_LLAM_USE_SUBDIRECTORY=ON.")
        endif()
        if(NOT EXISTS "${DCC_LLAM_ROOT}/include/llam/runtime.h")
            message(FATAL_ERROR "DCC_LLAM_ROOT does not contain LLAM headers: ${DCC_LLAM_ROOT}")
        endif()

        add_library(llam_runtime_prebuilt STATIC IMPORTED GLOBAL)
        add_library(llam::runtime ALIAS llam_runtime_prebuilt)
        set_target_properties(llam_runtime_prebuilt PROPERTIES
            IMPORTED_LOCATION "${DCC_LLAM_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DCC_LLAM_ROOT}/include"
            INTERFACE_LINK_LIBRARIES Threads::Threads
        )

        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            find_library(DCC_URING_LIBRARY uring REQUIRED)
            set_property(TARGET llam_runtime_prebuilt APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES "${DCC_URING_LIBRARY};m"
            )
        elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
            set_property(TARGET llam_runtime_prebuilt APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES rt
            )
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
            set_property(TARGET llam_runtime_prebuilt APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ws2_32 mswsock synchronization advapi32
            )
        endif()
    endif()
endif()

if(APPLE)
    add_compile_definitions(_DARWIN_C_SOURCE)
elseif(UNIX)
    add_compile_definitions(_DEFAULT_SOURCE _POSIX_C_SOURCE=200809L)
endif()

set(DCC_LIBRARY_TYPE STATIC)
if(DCC_BUILD_SHARED)
    set(DCC_LIBRARY_TYPE SHARED)
endif()

add_library(dcc ${DCC_LIBRARY_TYPE})
add_library(dcc::dcc ALIAS dcc)
set_target_properties(dcc PROPERTIES
    EXPORT_NAME dcc
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden
)

function(dcc_target_enable_sanitizers target)
    if(NOT DCC_ENABLE_SANITIZERS)
        return()
    endif()
    if(MSVC)
        message(FATAL_ERROR "DCC_ENABLE_SANITIZERS is only wired for Clang/GCC-style compilers")
    endif()
    target_compile_definitions(${target} PRIVATE DCC_ENABLE_SANITIZERS=1)
    target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${target} PRIVATE -fsanitize=address,undefined)
endfunction()

function(dcc_target_use_private_headers target)
    target_include_directories(${target}
        PRIVATE
            ${DCC_PRIVATE_INCLUDE_DIR}
    )
endfunction()

function(dcc_target_use_test_headers target)
    target_include_directories(${target}
        PRIVATE
            ${DCC_TEST_INCLUDE_DIR}
    )
endfunction()

target_sources(dcc
    PRIVATE
        src/cache/cache.c
        src/cache/cache_merge_channels.c
        src/cache/cache_merge_guild_collections.c
        src/cache/cache_merge_guild_flags.c
        src/cache/cache_merge_guild_identity.c
        src/cache/cache_merge_guild_limits.c
        src/cache/cache_merge_guild_strings.c
        src/cache/cache_merge_guilds.c
        src/cache/cache_merge_messages.c
        src/cache/cache_merge_objects_channels.c
        src/cache/cache_merge_objects_guilds.c
        src/cache/cache_merge_objects_messages.c
        src/cache/cache_merge_objects_users.c
        src/cache/cache_merge_objects_voice.c
        src/cache/cache_merge_users.c
        src/cache/cache_merge_voice.c
        src/cache/cache_objects_channels.c
        src/cache/cache_objects_guilds.c
        src/cache/cache_objects_members.c
        src/cache/cache_objects_messages.c
        src/cache/cache_objects_roles.c
        src/cache/cache_objects_users.c
        src/cache/cache_objects_voice.c
        src/cache/cache_policy.c
        src/cache/cache_table.c
        src/cache/cache_table_lifecycle.c
        src/cache/cache_table_resize.c
        src/cache/cache_table_sequence.c
        src/component_session/component_session_button.c
        src/component_session/component_session_crypto.c
        src/component_session/component_session_custom_id.c
        src/component_session/component_session_listener.c
        src/component_session/component_session_options.c
        src/component_session/component_session_state.c
        src/component_session/component_session_store.c
        src/component_session/component_session_verify.c
        src/command_registry/command_registry_action.c
        src/command_registry/command_registry_add.c
        src/command_registry/command_registry_apply.c
        src/command_registry/command_registry_apply_ops.c
        src/command_registry/command_registry_fetch.c
        src/command_registry/command_registry_format_buffer.c
        src/command_registry/command_registry_identity.c
        src/command_registry/command_registry_json_value.c
        src/command_registry/command_registry_options.c
        src/command_registry/command_registry_plan.c
        src/command_registry/command_registry_plan_format.c
        src/command_registry/command_registry_plan_lifecycle.c
        src/command_registry/command_registry_plan_push.c
        src/command_registry/command_registry_registry.c
        src/command_registry/command_registry_remote.c
        src/command_registry/command_registry_snapshot.c
        src/command_registry/command_registry_snapshot_entry.c
        src/client/cluster_config.c
        src/client/cluster_create_plan.c
        src/client/cluster_create_shards.c
        src/client/cluster_create_state.c
        src/client/cluster_destroy.c
        src/client/cluster_events.c
        src/client/cluster_health_events.c
        src/client/cluster_gateway.c
        src/client/cluster_gateway_admission.c
        src/client/cluster_gateway_info.c
        src/client/cluster_gateway_info_copy.c
        src/client/cluster_health_control.c
        src/client/cluster_health_json_loop_result.c
        src/client/cluster_health_json_plan.c
        src/client/cluster_health_json_result.c
        src/client/cluster_health_json_shard.c
        src/client/cluster_health_json_status.c
        src/client/cluster_health_json_summary.c
        src/client/cluster_health_state.c
        src/client/cluster_health.c
        src/client/cluster_json_buffer.c
        src/client/cluster_json_string.c
        src/client/cluster_lifecycle.c
        src/client/cluster_operations.c
        src/client/cluster_operations_json.c
        src/client/cluster_operations_plan.c
        src/client/cluster_operations_plan_helpers.c
        src/client/cluster_operations_run.c
        src/client/cluster_recovery.c
        src/client/cluster_recovery_action.c
        src/client/cluster_remediation.c
        src/client/cluster_remediation_result.c
        src/client/cluster_remediation_loop.c
        src/client/cluster_remediation_loop_delay.c
        src/client/cluster_remediation_loop_options.c
        src/client/cluster_remediation_loop_result.c
        src/client/cluster_restart_policy.c
        src/client/cluster_sidecar_route.c
        src/client/cluster_shards.c
        src/client/cluster_start.c
        src/client/cluster_start_identify_budget.c
        src/client/cluster_start_runtimes.c
        src/client/cluster_strings.c
        src/client/cluster_supervisor.c
        src/client/cluster_supervisor_apply.c
        src/client/cluster_supervisor_apply_error.c
        src/client/cluster_supervisor_decide.c
        src/client/cluster_supervisor_decision_callback.c
        src/client/cluster_supervisor_decision_json.c
        src/client/cluster_supervisor_policy.c
        src/client/cluster_supervisor_restart_result.c
        src/client/cluster_supervisor_runtime.c
        src/client/cluster_supervisor_status.c
        src/client/cluster_wait_health.c
        src/client/cluster_wait_health_options.c
        src/client/cluster_wait_health_runtime.c
        src/client/cluster_wait_ready.c
        src/client/cluster.c
        src/client/client_dm_cache.c
        src/client/client_events.c
        src/client/client_gateway_control.c
        src/client/client_lifecycle.c
        src/client/client_utils.c
        src/client/client.c
        src/events/event_accessors.c
        src/events/event_accessors_channel.c
        src/events/event_accessors_gateway.c
        src/events/event_accessors_guild.c
        src/events/event_accessors_guild_resources.c
        src/events/event_accessors_interactions.c
        src/events/event_accessors_message.c
        src/events/event_accessors_moderation.c
        src/events/event_accessors_misc_resources.c
        src/events/event_accessors_resources.c
        src/events/event_accessors_voice.c
        src/events/event_bus_dispatch.c
        src/events/event_bus_listeners.c
        src/events/event_bus_lock.c
        src/events/event_waiter_gateway.c
        src/events/event_waiter_gateway_policy.c
        src/events/event_waiter_gateway_sets.c
        src/events/event_waiter_interaction.c
        src/events/event_waiter_match.c
        src/events/event_waiter_match_count_setup.c
        src/events/event_waiter_match_helpers.c
        src/events/event_waiter_match_setup.c
        src/events/event_waiter_owned_free.c
        src/events/event_waiter_owned.c
        src/events/event_waiter_owned_objects.c
        src/events/event_waiter_policy_count.c
        src/events/event_waiter_policy.c
        src/events/event_waiter_policy_init.c
        src/events/event_waiter_policy_init_threshold.c
        src/events/event_waiter_policy_runtime.c
        src/events/event_waiter_policy_sequence.c
        src/events/event_waiter_runtime.c
        src/events/event_waiter_timing.c
        src/events/event_waiter_voice.c
        src/events/event_object_waiters.c
        src/events/event_object_waiters_any_channel.c
        src/events/event_object_waiters_any_core.c
        src/events/event_object_waiters_any_guild.c
        src/events/event_object_waiters_any_member.c
        src/events/event_object_waiters_any_role.c
        src/events/event_object_waiters_any_user.c
        src/events/event_object_waiters_any_voice.c
        src/events/event_object_waiters_single_core.c
        src/events/event_object_waiters_single_channel.c
        src/events/event_object_waiters_single_guild.c
        src/events/event_object_waiters_single_member.c
        src/events/event_object_waiters_single_role.c
        src/events/event_object_waiters_single_user.c
        src/events/event_object_waiters_single_voice.c
        src/events/event_types.c
        src/events/event_waiters.c
        src/events/event_waiters_any.c
        src/events/event_waiters_any_snapshot.c
        src/events/events.c
        src/events/interaction_listener_core.c
        src/events/interaction_listener_match.c
        src/events/interaction_listener_remove.c
        src/events/interaction_listener_state.c
        src/events/interaction_listeners.c
        src/events/interaction_listeners_components.c
        src/events/interaction_listeners_named.c
        src/gateway/gateway.c
        src/gateway/gateway_bot.c
        src/gateway/gateway_bot_response.c
        src/gateway/gateway_bot_url.c
        src/gateway/gateway_cache.c
        src/gateway/gateway_cache_channels.c
        src/gateway/gateway_cache_guilds.c
        src/gateway/gateway_cache_helpers.c
        src/gateway/gateway_cache_members.c
        src/gateway/gateway_cache_messages.c
        src/gateway/gateway_cache_user_voice.c
        src/gateway/gateway_close_policy.c
        src/gateway/gateway_dispatch.c
        src/gateway/gateway_dispatch_misc.c
        src/gateway/gateway_dispatch_objects.c
        src/gateway/gateway_dispatch_resources.c
        src/gateway/gateway_dispatch_runtime.c
        src/gateway/gateway_dispatch_session.c
        src/gateway/gateway_emit_channel_object.c
        src/gateway/gateway_emit_commerce_resources.c
        src/gateway/gateway_emit_core.c
        src/gateway/gateway_emit_guild_object.c
        src/gateway/gateway_emit_guild_resources.c
        src/gateway/gateway_emit_integration_resources.c
        src/gateway/gateway_emit_interactions.c
        src/gateway/gateway_emit_invite_resources.c
        src/gateway/gateway_emit_member_object.c
        src/gateway/gateway_emit_message_runtime.c
        src/gateway/gateway_emit_message_object.c
        src/gateway/gateway_emit_misc_resources.c
        src/gateway/gateway_emit_moderation_runtime.c
        src/gateway/gateway_emit_presence_events.c
        src/gateway/gateway_emit_role_object.c
        src/gateway/gateway_emit_scheduled_resources.c
        src/gateway/gateway_emit_session_events.c
        src/gateway/gateway_emit_soundboard_resources.c
        src/gateway/gateway_emit_stage_resources.c
        src/gateway/gateway_emit_thread_runtime.c
        src/gateway/gateway_emit_user_object.c
        src/gateway/gateway_emit_voice_runtime.c
        src/gateway/gateway_event_names.c
        src/gateway/gateway_events.c
        src/gateway/gateway_heartbeat.c
        src/gateway/gateway_identify.c
        src/gateway/gateway_payload.c
        src/gateway/gateway_payload_buffer.c
        src/gateway/gateway_payload_buffer_json.c
        src/gateway/gateway_payload_heartbeat.c
        src/gateway/gateway_resume.c
        src/gateway/gateway_runtime.c
        src/gateway/gateway_runtime_loop.c
        src/gateway/gateway_runtime_retry.c
        src/gateway/gateway_session.c
        src/gateway/gateway_session_hello.c
        src/gateway/gateway_session_lifecycle.c
        src/gateway/gateway_session_receive_finish.c
        src/gateway/gateway_session_opcode.c
        src/gateway/gateway_session_receive.c
        src/gateway/gateway_timing.c
        src/gateway/gateway_voice_state_update.c
        src/gateway/gateway_ws_control.c
        src/hot_reload/hot_reload_accessors.c
        src/hot_reload/hot_reload_file_sig.c
        src/hot_reload/hot_reload_health.c
        src/hot_reload/hot_reload_health_json.c
        src/hot_reload/hot_reload_health_json_buffer.c
        src/hot_reload/hot_reload_health_json_string.c
        src/hot_reload/hot_reload_health_state.c
        src/hot_reload/hot_reload_sidecar_route.c
        src/hot_reload/hot_reload_loader.c
        src/hot_reload/hot_reload_loader_file.c
        src/hot_reload/hot_reload_worker_child.c
        src/hot_reload/hot_reload_worker_child_loop.c
        src/hot_reload/hot_reload_worker_child_capture.c
        src/hot_reload/hot_reload_worker_child_result.c
        src/hot_reload/hot_reload_worker_capture_deinit.c
        src/hot_reload/hot_reload_worker_capture_store.c
        src/hot_reload/hot_reload_worker_canary.c
        src/hot_reload/hot_reload_worker_dispatch.c
        src/hot_reload/hot_reload_worker_dispatch_recovery.c
        src/hot_reload/hot_reload_worker_dispatch_temp_error.c
        src/hot_reload/hot_reload_worker_dispatch_try.c
        src/hot_reload/hot_reload_worker_healthcheck.c
        src/hot_reload/hot_reload_worker_ipc.c
        src/hot_reload/hot_reload_worker_lifecycle.c
        src/hot_reload/hot_reload_worker_parent.c
        src/hot_reload/hot_reload_worker_process_posix.c
        src/hot_reload/hot_reload_worker_process_spawn_posix.c
        src/hot_reload/hot_reload_worker_process_windows.c
        src/hot_reload/hot_reload_worker_process_spawn_windows.c
        src/hot_reload/hot_reload_worker_process_path_posix.c
        src/hot_reload/hot_reload_worker_process_lifecycle_posix.c
        src/hot_reload/hot_reload_worker_process_lifecycle_windows.c
        src/hot_reload/hot_reload_worker_process_stop_posix.c
        src/hot_reload/hot_reload_worker_process_stop_windows.c
        src/hot_reload/hot_reload_worker_process_ready.c
        src/hot_reload/hot_reload_worker_process_ready_frame.c
        src/hot_reload/hot_reload_worker_protocol.c
        src/hot_reload/hot_reload_worker_protocol_posix.c
        src/hot_reload/hot_reload_worker_protocol_windows.c
        src/hot_reload/hot_reload_worker_result_deinit.c
        src/hot_reload/hot_reload_worker_result_read.c
        src/hot_reload/hot_reload_worker_supervisor.c
        src/hot_reload/hot_reload_worker_supervisor_cycle.c
        src/hot_reload/hot_reload_worker_supervisor_worker.c
        src/hot_reload/hot_reload_worker_options.c
        src/hot_reload/hot_reload_module_context.c
        src/hot_reload/hot_reload_module_loader.c
        src/hot_reload/hot_reload_module_trampoline.c
        src/hot_reload/hot_reload_options.c
        src/hot_reload/hot_reload_poll.c
        src/hot_reload/hot_reload_reload.c
        src/hot_reload/hot_reload_reload_rollback.c
        src/hot_reload/hot_reload_runtime.c
        src/hot_reload/hot_reload_run.c
        src/hot_reload/hot_reload_run_env.c
        src/hot_reload/hot_reload_run_inputs.c
        src/hot_reload/hot_reload_run_options.c
        src/hot_reload/hot_reload_state.c
        src/hot_reload/hot_reload_wait_generation.c
        src/hot_reload/hot_reload_watch.c
        src/hot_reload/hot_reload_watch_task.c
        src/hot_reload/hot_reload_attach.c
        src/hot_reload/hot_reload.c
        src/hot_reload/canary/hot_reload_canary.c
        src/interaction_flow/interaction_flow_auto_defer.c
        src/interaction_flow/interaction_flow_core.c
        src/interaction_flow/interaction_flow_defer.c
        src/interaction_flow/interaction_flow_messages.c
        src/interaction_flow/interaction_flow_modal.c
        src/http/dcc_http_buffer.c
        src/http/dcc_http_request.c
        src/http/dcc_http_response.c
        src/http/dcc_http_response_chunked.c
        src/http/dcc_http_response_headers.c
        src/http/dcc_http_url.c
        src/http/dcc_http_exchange.c
        src/http/dcc_http.c
        src/interactions/interaction_server_accept.c
        src/interactions/interaction_server_bind.c
        src/interactions/interaction_server_client_task.c
        src/interactions/interaction_server_crypto.c
        src/interactions/interaction_server_dispatch.c
        src/interactions/interaction_server_dispatch_helpers.c
        src/interactions/interaction_server_error.c
        src/interactions/interaction_server_health.c
        src/interactions/interaction_server_health_json.c
        src/interactions/interaction_server_health_route.c
        src/interactions/interaction_server_json_escape.c
        src/interactions/interaction_server_http_header_scan.c
        src/interactions/interaction_server_http_request.c
        src/interactions/interaction_server_http_request_buffer.c
        src/interactions/interaction_server_http_request_parser.c
        src/interactions/interaction_server_http_response.c
        src/interactions/interaction_server_lifecycle.c
        src/interactions/interaction_server_lifecycle_control.c
        src/interactions/interaction_server_request_api.c
        src/interactions/interaction_server_request_builder_replies.c
        src/interactions/interaction_server_request_message_replies.c
        src/interactions/interaction_server_request_message_text.c
        src/interactions/interaction_server_request_response.c
        src/interactions/interaction_server_request_replies.c
        src/interactions/interaction_server_route_lookup.c
        src/interactions/interaction_server_route_storage.c
        src/interactions/interaction_server_response.c
        src/interactions/interaction_server_routes.c
        src/interactions/interaction_server.c
        src/interactions/interaction_server_state.c
        src/interactions/interaction_server_stats.c
        src/interactions/interaction_server_text.c
        src/interactions/interaction_server_wait.c
        src/json/dcc_json_accessors.c
        src/json/dcc_json_arena.c
        src/json/dcc_json_dom.c
        src/json/dcc_json_dom_alloc.c
        src/json/dcc_json_dom_array.c
        src/json/dcc_json_dom_object.c
        src/json/dcc_json_dom_value.c
        src/json/dcc_json_gateway_activity_array_finish.c
        src/json/dcc_json_gateway_activity_array_items.c
        src/json/dcc_json_gateway_activity_array_loop.c
        src/json/dcc_json_gateway_activity_arrays.c
        src/json/dcc_json_gateway_activity_buffers.c
        src/json/dcc_json_gateway_activity_emoji.c
        src/json/dcc_json_gateway_activity_emoji_fields.c
        src/json/dcc_json_gateway_activity_object_dispatch.c
        src/json/dcc_json_gateway_activity_object_fields.c
        src/json/dcc_json_gateway_activity_objects.c
        src/json/dcc_json_gateway_activity_scalar_fields.c
        src/json/dcc_json_gateway_activity_text_fields.c
        src/json/dcc_json_gateway_application_permission_fields.c
        src/json/dcc_json_gateway_application_permission_object.c
        src/json/dcc_json_gateway_application_permission_object_fields.c
        src/json/dcc_json_gateway_actions.c
        src/json/dcc_json_gateway_activities.c
        src/json/dcc_json_gateway_application_permissions.c
        src/json/dcc_json_gateway_client_status.c
        src/json/dcc_json_gateway_client_status_entries.c
        src/json/dcc_json_gateway_client_status_fields.c
        src/json/dcc_json_gateway_d.c
        src/json/dcc_json_gateway_d_auto_moderation_text.c
        src/json/dcc_json_gateway_d_channel_invite_text.c
        src/json/dcc_json_gateway_d_collections.c
        src/json/dcc_json_gateway_d_channel_numeric_scalar_apply.c
        src/json/dcc_json_gateway_d_channel_numeric_scalars.c
        src/json/dcc_json_gateway_d_context_text.c
        src/json/dcc_json_gateway_d_member_context_text.c
        src/json/dcc_json_gateway_d_core.c
        src/json/dcc_json_gateway_d_core_application_id.c
        src/json/dcc_json_gateway_d_core_application_type.c
        src/json/dcc_json_gateway_d_core_type.c
        src/json/dcc_json_gateway_d_core_guild_bools.c
        src/json/dcc_json_gateway_d_core_guild_capacity_limit_keys.c
        src/json/dcc_json_gateway_d_core_guild_capacity_limits.c
        src/json/dcc_json_gateway_d_core_guild_limit_values.c
        src/json/dcc_json_gateway_d_core_guild_limits.c
        src/json/dcc_json_gateway_d_core_guild_policy_apply.c
        src/json/dcc_json_gateway_d_core_guild_policy_limits.c
        src/json/dcc_json_gateway_d_core_guild_user.c
        src/json/dcc_json_gateway_d_core_channel_id.c
        src/json/dcc_json_gateway_d_core_guild_id.c
        src/json/dcc_json_gateway_d_core_guild_id_fields.c
        src/json/dcc_json_gateway_d_core_guild_id_flags.c
        src/json/dcc_json_gateway_d_core_id.c
        src/json/dcc_json_gateway_d_core_identity.c
        src/json/dcc_json_gateway_d_core_interaction.c
        src/json/dcc_json_gateway_d_core_interaction_objects.c
        src/json/dcc_json_gateway_d_core_interaction_scalars.c
        src/json/dcc_json_gateway_d_core_interaction_text.c
        src/json/dcc_json_gateway_d_core_message.c
        src/json/dcc_json_gateway_d_core_message_author.c
        src/json/dcc_json_gateway_d_core_message_bool.c
        src/json/dcc_json_gateway_d_core_message_flags.c
        src/json/dcc_json_gateway_d_core_message_scheduled.c
        src/json/dcc_json_gateway_d_core_message_scheduled_object.c
        src/json/dcc_json_gateway_d_core_message_scheduled_text.c
        src/json/dcc_json_gateway_d_core_message_text.c
        src/json/dcc_json_gateway_d_core_member_count.c
        src/json/dcc_json_gateway_d_core_name.c
        src/json/dcc_json_gateway_d_core_session.c
        src/json/dcc_json_gateway_d_core_user.c
        src/json/dcc_json_gateway_d_core_webhook_id.c
        src/json/dcc_json_gateway_d_bool_scalar_fields.c
        src/json/dcc_json_gateway_d_bool_scalars.c
        src/json/dcc_json_gateway_d_bulk_id_collections.c
        src/json/dcc_json_gateway_d_channel_id_collections.c
        src/json/dcc_json_gateway_d_entitlement_ids.c
        src/json/dcc_json_gateway_d_entitlement_nullable_ids.c
        src/json/dcc_json_gateway_d_entitlement_values.c
        src/json/dcc_json_gateway_d_emoji_voice_text.c
        src/json/dcc_json_gateway_d_event_ids.c
        src/json/dcc_json_gateway_d_moderation_event_ids.c
        src/json/dcc_json_gateway_d_resource_event_ids.c
        src/json/dcc_json_gateway_d_scheduled_event_ids.c
        src/json/dcc_json_gateway_d_guild_avatar.c
        src/json/dcc_json_gateway_d_guild_media_scalars.c
        src/json/dcc_json_gateway_d_guild_channel_id_apply.c
        src/json/dcc_json_gateway_d_guild_icon_banner.c
        src/json/dcc_json_gateway_d_guild_media_text.c
        src/json/dcc_json_gateway_d_guild_channel_ids.c
        src/json/dcc_json_gateway_d_id_collections.c
        src/json/dcc_json_gateway_d_ids.c
        src/json/dcc_json_gateway_d_invite_numeric_scalars.c
        src/json/dcc_json_gateway_d_invite_thread_time_scalars.c
        src/json/dcc_json_gateway_d_guild_collections.c
        src/json/dcc_json_gateway_d_member_array_collections.c
        src/json/dcc_json_gateway_d_member_collections.c
        src/json/dcc_json_gateway_d_member_id_collections.c
        src/json/dcc_json_gateway_d_member_numeric_scalars.c
        src/json/dcc_json_gateway_d_member_time_scalars.c
        src/json/dcc_json_gateway_d_member_voice_time_scalars.c
        src/json/dcc_json_gateway_d_media_collections.c
        src/json/dcc_json_gateway_d_media_ids.c
        src/json/dcc_json_gateway_d_message_timestamp.c
        src/json/dcc_json_gateway_d_message_timestamp_text.c
        src/json/dcc_json_gateway_d_message_time_scalars.c
        src/json/dcc_json_gateway_d_moderation_numeric_scalars.c
        src/json/dcc_json_gateway_d_numeric_scalars.c
        src/json/dcc_json_gateway_d_object_collections.c
        src/json/dcc_json_gateway_d_permission_collections.c
        src/json/dcc_json_gateway_d_primary_message_ids.c
        src/json/dcc_json_gateway_d_primary_ids.c
        src/json/dcc_json_gateway_d_primary_resource_ids.c
        src/json/dcc_json_gateway_d_primary_user_id.c
        src/json/dcc_json_gateway_d_role_id_collections.c
        src/json/dcc_json_gateway_d_schedule_entitlement_time_scalars.c
        src/json/dcc_json_gateway_d_scheduled_event_numeric_scalars.c
        src/json/dcc_json_gateway_d_scalars.c
        src/json/dcc_json_gateway_d_subscription_id_collections.c
        src/json/dcc_json_gateway_d_subscription_time_scalars.c
        src/json/dcc_json_gateway_d_status_text.c
        src/json/dcc_json_gateway_d_text_scalars.c
        src/json/dcc_json_gateway_d_time_scalars.c
        src/json/dcc_json_gateway_d_voice_flags.c
        src/json/dcc_json_gateway_d_voice_presence_double.c
        src/json/dcc_json_gateway_d_voice_presence_numeric_scalars.c
        src/json/dcc_json_gateway_d_voice_presence_time.c
        src/json/dcc_json_gateway_d_voice_presence_u32.c
        src/json/dcc_json_gateway_d_voice_time_scalars.c
        src/json/dcc_json_gateway_emoji_apply.c
        src/json/dcc_json_gateway_emoji.c
        src/json/dcc_json_gateway_emoji_fields.c
        src/json/dcc_json_gateway_emoji_object_fields.c
        src/json/dcc_json_gateway_entitlement_array_finish.c
        src/json/dcc_json_gateway_entitlement_array_items.c
        src/json/dcc_json_gateway_entitlement_ids.c
        src/json/dcc_json_gateway_entitlement_object_dispatch.c
        src/json/dcc_json_gateway_entitlement_object_fields.c
        src/json/dcc_json_gateway_entitlement_objects.c
        src/json/dcc_json_gateway_entitlement_scalars.c
        src/json/dcc_json_gateway_entitlement_times.c
        src/json/dcc_json_gateway_entitlements.c
        src/json/dcc_json_gateway_entity_metadata.c
        src/json/dcc_json_gateway_entity_metadata_fields.c
        src/json/dcc_json_gateway_event_guild_id_aliases.c
        src/json/dcc_json_gateway_event_id_aliases.c
        src/json/dcc_json_gateway_event_names.c
        src/json/dcc_json_gateway_events.c
        src/json/dcc_json_gateway_guild_channel_array_finish.c
        src/json/dcc_json_gateway_guild_channel_array_items.c
        src/json/dcc_json_gateway_guilds.c
        src/json/dcc_json_gateway_interaction_data_fields.c
        src/json/dcc_json_gateway_interaction_data_id_fields.c
        src/json/dcc_json_gateway_interaction_data_object_fields.c
        src/json/dcc_json_gateway_interaction_data_scalar_fields.c
        src/json/dcc_json_gateway_interaction_data_text_fields.c
        src/json/dcc_json_gateway_interaction_owner_entries.c
        src/json/dcc_json_gateway_interaction_owners.c
        src/json/dcc_json_gateway_interaction_option_array_loop.c
        src/json/dcc_json_gateway_interaction_option_arrays.c
        src/json/dcc_json_gateway_interaction_option_array_items.c
        src/json/dcc_json_gateway_interaction_option_array_lists.c
        src/json/dcc_json_gateway_interaction_option_child_fields.c
        src/json/dcc_json_gateway_interaction_option_fields.c
        src/json/dcc_json_gateway_interaction_option_finalize.c
        src/json/dcc_json_gateway_interaction_option_meta_fields.c
        src/json/dcc_json_gateway_interaction_option_number_text.c
        src/json/dcc_json_gateway_interaction_option_object_fields.c
        src/json/dcc_json_gateway_interaction_option_objects.c
        src/json/dcc_json_gateway_interaction_option_value_bool.c
        src/json/dcc_json_gateway_interaction_option_value_fields.c
        src/json/dcc_json_gateway_interaction_option_value_null.c
        src/json/dcc_json_gateway_interaction_option_value_number.c
        src/json/dcc_json_gateway_interaction_option_value_parse_bool.c
        src/json/dcc_json_gateway_interaction_option_value_parse_number.c
        src/json/dcc_json_gateway_interaction_option_value_parse_string.c
        src/json/dcc_json_gateway_interaction_option_value_string.c
        src/json/dcc_json_gateway_interaction_option_value_utils.c
        src/json/dcc_json_gateway_interaction_option_values.c
        src/json/dcc_json_gateway_interaction_options.c
        src/json/dcc_json_gateway_interaction_values.c
        src/json/dcc_json_gateway_interactions.c
        src/json/dcc_json_gateway_list_emoji_arrays.c
        src/json/dcc_json_gateway_list_emoji_bools.c
        src/json/dcc_json_gateway_list_emoji_buffers.c
        src/json/dcc_json_gateway_list_emoji_ids.c
        src/json/dcc_json_gateway_list_emoji_array_items.c
        src/json/dcc_json_gateway_list_emoji_object_dispatch.c
        src/json/dcc_json_gateway_list_emoji_object_fields.c
        src/json/dcc_json_gateway_list_emoji_objects.c
        src/json/dcc_json_gateway_list_emoji_text_fields.c
        src/json/dcc_json_gateway_list_emoji_user_fields.c
        src/json/dcc_json_gateway_list_objects.c
        src/json/dcc_json_gateway_list_soundboard_array_items.c
        src/json/dcc_json_gateway_list_soundboard_arrays.c
        src/json/dcc_json_gateway_list_soundboard.c
        src/json/dcc_json_gateway_list_soundboard_fields.c
        src/json/dcc_json_gateway_list_soundboard_id_fields.c
        src/json/dcc_json_gateway_list_soundboard_object_fields.c
        src/json/dcc_json_gateway_list_soundboard_objects.c
        src/json/dcc_json_gateway_list_soundboard_scalar_fields.c
        src/json/dcc_json_gateway_list_soundboard_text_fields.c
        src/json/dcc_json_gateway_list_soundboard_user_fields.c
        src/json/dcc_json_gateway_list_sticker_array_items.c
        src/json/dcc_json_gateway_list_sticker_arrays.c
        src/json/dcc_json_gateway_list_sticker_buffers.c
        src/json/dcc_json_gateway_list_sticker_fields.c
        src/json/dcc_json_gateway_list_sticker_id_fields.c
        src/json/dcc_json_gateway_list_sticker_object_fields.c
        src/json/dcc_json_gateway_list_sticker_objects.c
        src/json/dcc_json_gateway_list_sticker_scalar_fields.c
        src/json/dcc_json_gateway_list_sticker_text_fields.c
        src/json/dcc_json_gateway_list_sticker_user_fields.c
        src/json/dcc_json_gateway_list_stickers.c
        src/json/dcc_json_gateway_list_thread_array_finish.c
        src/json/dcc_json_gateway_list_thread_member_arrays.c
        src/json/dcc_json_gateway_list_thread_member_fields.c
        src/json/dcc_json_gateway_list_thread_member_ids.c
        src/json/dcc_json_gateway_list_thread_member_objects.c
        src/json/dcc_json_gateway_list_thread_member_scalars.c
        src/json/dcc_json_gateway_list_thread_member_times.c
        src/json/dcc_json_gateway_list_thread_fields.c
        src/json/dcc_json_gateway_list_thread_ids.c
        src/json/dcc_json_gateway_list_thread_members.c
        src/json/dcc_json_gateway_list_thread_object_fields.c
        src/json/dcc_json_gateway_list_thread_objects.c
        src/json/dcc_json_gateway_list_thread_scalars.c
        src/json/dcc_json_gateway_list_thread_text_fields.c
        src/json/dcc_json_gateway_list_threads.c
        src/json/dcc_json_gateway_member_list_arrays.c
        src/json/dcc_json_gateway_member_list_buffers.c
        src/json/dcc_json_gateway_member_list_entries.c
        src/json/dcc_json_gateway_member_list_fields.c
        src/json/dcc_json_gateway_member_list_object_fields.c
        src/json/dcc_json_gateway_member_list_object_parse.c
        src/json/dcc_json_gateway_member_list_object_prepare.c
        src/json/dcc_json_gateway_member_list_scalar_fields.c
        src/json/dcc_json_gateway_member_list_objects.c
        src/json/dcc_json_gateway_member_list_text_fields.c
        src/json/dcc_json_gateway_member_list_time_fields.c
        src/json/dcc_json_gateway_member_list_user_fields.c
        src/json/dcc_json_gateway_member_lists.c
        src/json/dcc_json_gateway_member_fields.c
        src/json/dcc_json_gateway_member_finish.c
        src/json/dcc_json_gateway_member_scalar_fields.c
        src/json/dcc_json_gateway_member_text_fields.c
        src/json/dcc_json_gateway_member_time_fields.c
        src/json/dcc_json_gateway_member_user_fields.c
        src/json/dcc_json_gateway_members.c
        src/json/dcc_json_gateway_normalize.c
        src/json/dcc_json_gateway_normalize_collections.c
        src/json/dcc_json_gateway_normalize_identity.c
        src/json/dcc_json_gateway_normalize_message_reactions.c
        src/json/dcc_json_gateway_normalize_messages.c
        src/json/dcc_json_gateway_normalize_moderation.c
        src/json/dcc_json_gateway_normalize_poll_votes.c
        src/json/dcc_json_gateway_normalize_resource_commerce.c
        src/json/dcc_json_gateway_normalize_resource_invite_stage.c
        src/json/dcc_json_gateway_normalize_resource_scheduled.c
        src/json/dcc_json_gateway_normalize_resource_soundboard.c
        src/json/dcc_json_gateway_normalize_resources.c
        src/json/dcc_json_gateway_normalize_threads.c
        src/json/dcc_json_gateway_normalize_update_channel.c
        src/json/dcc_json_gateway_normalize_update_core.c
        src/json/dcc_json_gateway_normalize_update_guild.c
        src/json/dcc_json_gateway_normalize_updates.c
        src/json/dcc_json_gateway_nullable_string_fields.c
        src/json/dcc_json_gateway_payload_fields.c
        src/json/dcc_json_gateway_payload_parse.c
        src/json/dcc_json_gateway_presence.c
        src/json/dcc_json_gateway_presence_client_status.c
        src/json/dcc_json_gateway_presence_client_status_object_fields.c
        src/json/dcc_json_gateway_presence_client_status_fields.c
        src/json/dcc_json_gateway_presence_list.c
        src/json/dcc_json_gateway_presence_list_activity_fields.c
        src/json/dcc_json_gateway_presence_list_arrays.c
        src/json/dcc_json_gateway_presence_list_buffers.c
        src/json/dcc_json_gateway_presence_list_entries.c
        src/json/dcc_json_gateway_presence_list_fields.c
        src/json/dcc_json_gateway_presence_list_identity_fields.c
        src/json/dcc_json_gateway_presence_list_object_fields.c
        src/json/dcc_json_gateway_presence_list_object_parse.c
        src/json/dcc_json_gateway_presence_list_objects.c
        src/json/dcc_json_gateway_presence_list_status_fields.c
        src/json/dcc_json_gateway_resolved.c
        src/json/dcc_json_gateway_resolved_fields.c
        src/json/dcc_json_gateway_resolved_attachment_arrays.c
        src/json/dcc_json_gateway_resolved_attachment_entries.c
        src/json/dcc_json_gateway_resolved_attachment_ids.c
        src/json/dcc_json_gateway_resolved_attachment_fields.c
        src/json/dcc_json_gateway_resolved_attachment_media_fields.c
        src/json/dcc_json_gateway_resolved_attachment_maps.c
        src/json/dcc_json_gateway_resolved_attachment_numeric_fields.c
        src/json/dcc_json_gateway_resolved_attachment_object_fields.c
        src/json/dcc_json_gateway_resolved_attachment_string_fields.c
        src/json/dcc_json_gateway_resolved_attachments.c
        src/json/dcc_json_gateway_resolved_channel_arrays.c
        src/json/dcc_json_gateway_resolved_channel_entries.c
        src/json/dcc_json_gateway_resolved_channel_maps.c
        src/json/dcc_json_gateway_resolved_member_arrays.c
        src/json/dcc_json_gateway_resolved_member_entries.c
        src/json/dcc_json_gateway_resolved_member_map_items.c
        src/json/dcc_json_gateway_resolved_members.c
        src/json/dcc_json_gateway_resolved_message_arrays.c
        src/json/dcc_json_gateway_resolved_message_author_fields.c
        src/json/dcc_json_gateway_resolved_message_entries.c
        src/json/dcc_json_gateway_resolved_message_fields.c
        src/json/dcc_json_gateway_resolved_message_id_fields.c
        src/json/dcc_json_gateway_resolved_message_object_fields.c
        src/json/dcc_json_gateway_resolved_message_objects.c
        src/json/dcc_json_gateway_resolved_message_scalar_fields.c
        src/json/dcc_json_gateway_resolved_message_text_fields.c
        src/json/dcc_json_gateway_resolved_messages.c
        src/json/dcc_json_gateway_resolved_object_map_items.c
        src/json/dcc_json_gateway_resolved_object_maps.c
        src/json/dcc_json_gateway_resolved_permissions.c
        src/json/dcc_json_gateway_resolved_role_arrays.c
        src/json/dcc_json_gateway_resolved_role_entries.c
        src/json/dcc_json_gateway_resolved_role_maps.c
        src/json/dcc_json_gateway_resolved_user_arrays.c
        src/json/dcc_json_gateway_resolved_user_entries.c
        src/json/dcc_json_gateway_resolved_users.c
        src/json/dcc_json_gateway_role_array_finish.c
        src/json/dcc_json_gateway_role_array_items.c
        src/json/dcc_json_gateway_role_arrays.c
        src/json/dcc_json_gateway_role_bool_fields.c
        src/json/dcc_json_gateway_role_fields.c
        src/json/dcc_json_gateway_role_id_fields.c
        src/json/dcc_json_gateway_role_object_fields.c
        src/json/dcc_json_gateway_role_object_parse_fields.c
        src/json/dcc_json_gateway_role_objects.c
        src/json/dcc_json_gateway_role_scalar_fields.c
        src/json/dcc_json_gateway_role_text_fields.c
        src/json/dcc_json_gateway_roles.c
        src/json/dcc_json_gateway_scheduled_event_array_finish.c
        src/json/dcc_json_gateway_scheduled_event_array_items.c
        src/json/dcc_json_gateway_scheduled_event_arrays.c
        src/json/dcc_json_gateway_scheduled_event_buffers.c
        src/json/dcc_json_gateway_scheduled_event_creator_fields.c
        src/json/dcc_json_gateway_scheduled_event_ids.c
        src/json/dcc_json_gateway_scheduled_event_identity_text_fields.c
        src/json/dcc_json_gateway_scheduled_event_numeric_fields.c
        src/json/dcc_json_gateway_scheduled_event_object_dispatch.c
        src/json/dcc_json_gateway_scheduled_event_object_fields.c
        src/json/dcc_json_gateway_scheduled_event_text_fields.c
        src/json/dcc_json_gateway_scheduled_event_time_text_fields.c
        src/json/dcc_json_gateway_scheduled_events.c
        src/json/dcc_json_gateway_stage_instance_array_finish.c
        src/json/dcc_json_gateway_stage_instance_array_items.c
        src/json/dcc_json_gateway_stage_instance_buffers.c
        src/json/dcc_json_gateway_stage_instance_id_fields.c
        src/json/dcc_json_gateway_stage_instance_object_dispatch.c
        src/json/dcc_json_gateway_stage_instance_object_fields.c
        src/json/dcc_json_gateway_stage_instance_objects.c
        src/json/dcc_json_gateway_stage_instance_scalar_fields.c
        src/json/dcc_json_gateway_stage_instance_text_fields.c
        src/json/dcc_json_gateway_stage_instances.c
        src/json/dcc_json_gateway_user_buffers.c
        src/json/dcc_json_gateway_user_flag_fields.c
        src/json/dcc_json_gateway_user_id_fields.c
        src/json/dcc_json_gateway_user_object_dispatch.c
        src/json/dcc_json_gateway_user_object_fields.c
        src/json/dcc_json_gateway_user_objects.c
        src/json/dcc_json_gateway_user_text_fields.c
        src/json/dcc_json_gateway_users.c
        src/json/dcc_json_interaction_payload_parse.c
        src/json/dcc_json_keys.c
        src/json/dcc_json_keys_extended.c
        src/json/dcc_json_keys_extended_tail.c
        src/json/dcc_json_keys_len3.c
        src/json/dcc_json_keys_len4.c
        src/json/dcc_json_keys_len5.c
        src/json/dcc_json_keys_len6.c
        src/json/dcc_json_keys_len7.c
        src/json/dcc_json_keys_len8.c
        src/json/dcc_json_keys_len9.c
        src/json/dcc_json_keys_len10.c
        src/json/dcc_json_keys_len11.c
        src/json/dcc_json_keys_len12.c
        src/json/dcc_json_keys_len13.c
        src/json/dcc_json_keys_len14.c
        src/json/dcc_json_keys_len15.c
        src/json/dcc_json_keys_len16.c
        src/json/dcc_json_keys_len17.c
        src/json/dcc_json_keys_len18.c
        src/json/dcc_json_keys_long.c
        src/json/dcc_json_keys_medium.c
        src/json/dcc_json_keys_medium_tail.c
        src/json/dcc_json_keys_short.c
        src/json/dcc_json_payload.c
        src/json/dcc_json_parse_core.c
        src/json/dcc_json_parse_nullable_strings.c
        src/json/dcc_json_parse_number_ascii.c
        src/json/dcc_json_parse_number_direct.c
        src/json/dcc_json_parse_number_span.c
        src/json/dcc_json_parse_nested_user_id.c
        src/json/dcc_json_parse_object_id.c
        src/json/dcc_json_parse_object_ids.c
        src/json/dcc_json_parse_skip.c
        src/json/dcc_json_parse_skip_array.c
        src/json/dcc_json_parse_skip_object.c
        src/json/dcc_json_parse_snowflake_arrays.c
        src/json/dcc_json_parse_tokens.c
        src/json/dcc_json_parse_utils.c
        src/json/dcc_json_stage1.c
        src/json/dcc_json_stage1_marks.c
        src/json/dcc_json_stage1_mask.c
        src/json/dcc_json_stage1_validate.c
        src/json/dcc_json_string_decode.c
        src/json/dcc_json_string_escape.c
        src/json/dcc_json_string_parse.c
        src/json/dcc_json_string.c
        src/json/dcc_json_string_utf8.c
        src/json/dcc_json_string_utf8_append.c
        src/net/dcc_conn.c
        src/net/dcc_conn_io.c
        src/net/dcc_platform_datagram.c
        src/net/dcc_platform_resolve.c
        src/net/dcc_conn_tcp.c
        src/net/dcc_conn_tls.c
        src/objects/application_command_builder_buffer.c
        src/objects/application_command_builder_escape.c
        src/objects/application_command_builder_json.c
        src/objects/application_command_builder_members.c
        src/objects/application_command_builder_numbers.c
        src/objects/application_command_builder_payload_setters.c
        src/objects/application_command_builder_permission_setters.c
        src/objects/application_command_builder_serialize.c
        src/objects/application_command_builder_serialize_fields.c
        src/objects/application_command_builder_serialize_payload.c
        src/objects/application_command_builder_setters.c
        src/objects/autocomplete_builder_choice_json.c
        src/objects/autocomplete_builder_json.c
        src/objects/autocomplete_builder_setters.c
        src/objects/autocomplete_json_buffer.c
        src/objects/autocomplete_json_escape.c
        src/objects/autocomplete_json_members.c
        src/objects/autocomplete_json_numbers.c
        src/objects/component_builder_build.c
        src/objects/component_builder_core.c
        src/objects/component_builder_serialize.c
        src/objects/component_builder_serialize_action_row.c
        src/objects/component_builder_serialize_button.c
        src/objects/component_builder_serialize_emoji.c
        src/objects/component_builder_serialize_select.c
        src/objects/component_builder_serialize_text_input.c
        src/objects/component_builder_setters.c
        src/objects/component_builder_setters_select.c
        src/objects/component_builder_setters_state.c
        src/objects/component_builder_setters_text_input.c
        src/objects/component_builder_validate.c
        src/objects/component_json_buffer.c
        src/objects/component_json_escape.c
        src/objects/component_json_members.c
        src/objects/embed_builder_json.c
        src/objects/embed_builder_serialize.c
        src/objects/embed_builder_serialize_author.c
        src/objects/embed_builder_serialize_fields.c
        src/objects/embed_builder_serialize_footer.c
        src/objects/embed_builder_serialize_media.c
        src/objects/embed_builder_rich_setters.c
        src/objects/embed_builder_setters.c
        src/objects/embed_builder_validate.c
        src/objects/embed_json_buffer.c
        src/objects/embed_json_escape.c
        src/objects/embed_json_members.c
        src/objects/embed_json_numbers.c
        src/objects/interaction_accessors.c
        src/objects/interaction_option_values.c
        src/objects/interaction_resolved_accessors.c
        src/objects/interaction_resolved_accessors_misc.c
        src/objects/interaction_resolved_accessors_resources.c
        src/objects/interaction_subcommand_accessors.c
        src/objects/message_builder.c
        src/objects/message_builder_poll.c
        src/objects/message_builder_poll_media.c
        src/objects/message_builder_poll_validate.c
        src/objects/message_builder_serialize.c
        src/objects/message_builder_serialize_basic.c
        src/objects/message_builder_serialize_rich.c
        src/objects/message_builder_serialize_validate.c
        src/objects/message_builder_setters.c
        src/objects/message_builder_setters_collections.c
        src/objects/message_builder_setters_json.c
        src/objects/message_json_buffer.c
        src/objects/message_json_escape.c
        src/objects/message_json_members.c
        src/objects/modal_builder.c
        src/objects/modal_builder_serialize.c
        src/objects/modal_json_buffer.c
        src/objects/modal_json_escape.c
        src/objects/modal_json_members.c
        src/objects/objects_channel_clone.c
        src/objects/objects_channel_fields.c
        src/objects/objects_common.c
        src/objects/objects_events.c
        src/objects/objects_events_application_command_permissions.c
        src/objects/objects_events_channel_updates.c
        src/objects/objects_events_guild_bans.c
        src/objects/objects_events_guild_id_lists.c
        src/objects/objects_events_guild_id_lists_free.c
        src/objects/objects_events_guild_join_request.c
        src/objects/objects_events_guild_updates.c
        src/objects/objects_events_integrations.c
        src/objects/objects_events_message_delete.c
        src/objects/objects_events_message_reaction.c
        src/objects/objects_events_moderation.c
        src/objects/objects_events_poll_vote.c
        src/objects/objects_events_presence.c
        src/objects/objects_events_guild_members_chunk.c
        src/objects/objects_events_threads.c
        src/objects/objects_events_thread_list_sync.c
        src/objects/objects_events_voice_channel.c
        src/objects/objects_events_voice_server.c
        src/objects/objects_events_voice_state.c
        src/objects/objects_events_voice.c
        src/objects/objects_gateway.c
        src/objects/objects_gateway_collection_activities.c
        src/objects/objects_gateway_collection_members.c
        src/objects/objects_gateway_collection_snowflakes.c
        src/objects/objects_gateway_collections_clone.c
        src/objects/objects_gateway_collections_free.c
        src/objects/objects_gateway_strings_clear.c
        src/objects/objects_gateway_strings_free.c
        src/objects/objects_gateway_strings.c
        src/objects/objects_guild.c
        src/objects/objects_guild_collections.c
        src/objects/objects_guild_collections_core.c
        src/objects/objects_guild_collections_free.c
        src/objects/objects_guild_collections_resources.c
        src/objects/objects_guild_strings.c
        src/objects/objects_interaction_clone_collections.c
        src/objects/objects_interaction_clone_options.c
        src/objects/objects_interaction_clone_strings.c
        src/objects/objects_interaction_clone_values.c
        src/objects/objects_interactions.c
        src/objects/objects_interaction_options.c
        src/objects/objects_interaction_resolved_clone.c
        src/objects/objects_interaction_resolved_clone_files.c
        src/objects/objects_interaction_resolved_clone_people.c
        src/objects/objects_interaction_resolved_clone_resources.c
        src/objects/objects_interaction_resolved_free.c
        src/objects/objects_attachment_fields.c
        src/objects/objects_commerce.c
        src/objects/objects_emojis.c
        src/objects/objects_invites.c
        src/objects/objects_member_clone.c
        src/objects/objects_member_fields.c
        src/objects/objects_message.c
        src/objects/objects_message_fields.c
        src/objects/objects_emoji_fields.c
        src/objects/objects_presence_fields.c
        src/objects/objects_role_clone.c
        src/objects/objects_role_fields.c
        src/objects/objects_scheduled_events.c
        src/objects/objects_soundboard.c
        src/objects/objects_stage_instances.c
        src/objects/objects_sticker_fields.c
        src/objects/objects_stickers.c
        src/objects/objects_subscription.c
        src/objects/objects_thread_member_fields.c
        src/objects/objects.c
        src/replay/replay_json.c
        src/replay/replay_kind.c
        src/replay/replay_line.c
        src/replay/replay_parse.c
        src/replay/replay_player.c
        src/replay/replay_recorder.c
        src/rest/rest_application_command_builders.c
        src/rest/rest_application_command_permission_builders.c
        src/rest/rest_application_command_permission_json.c
        src/rest/rest_application_command_permissions.c
        src/rest/rest_application_command_permissions_params.c
        src/rest/rest_application_commands.c
        src/rest/rest_application_commands_global_bulk.c
        src/rest/rest_application_commands_global_write.c
        src/rest/rest_application_commands_guild_create.c
        src/rest/rest_application_commands_guild_delete_bulk.c
        src/rest/rest_application_commands_guild_edit.c
        src/rest/rest_application_commands_guild_fetch.c
        src/rest/rest_application_commands_registration.c
        src/rest/rest_application_commands_registration_options.c
        src/rest/rest_async_active_queue.c
        src/rest/rest_async_cancel.c
        src/rest/rest_async_cancel_active.c
        src/rest/rest_async_cancel_pending.c
        src/rest/rest_async_drain.c
        src/rest/rest_async_pending_queue.c
        src/rest/rest_async_queue.c
        src/rest/rest_async_routes.c
        src/rest/rest_async_retry.c
        src/rest/rest_async_request.c
        src/rest/rest_async_request_new.c
        src/rest/rest_async_signal.c
        src/rest/rest_async_status.c
        src/rest/rest_async_status_snapshot.c
        src/rest/rest_async_submit.c
        src/rest/rest_async_wait.c
        src/rest/rest_async_worker_lifecycle.c
        src/rest/rest_async_worker.c
        src/rest/rest_async.c
        src/rest/rest_auto_moderation_action_item.c
        src/rest/rest_auto_moderation_actions.c
        src/rest/rest_auto_moderation_builders.c
        src/rest/rest_auto_moderation_create.c
        src/rest/rest_auto_moderation_metadata.c
        src/rest/rest_auto_moderation_modify.c
        src/rest/rest_auto_moderation.c
        src/rest/rest_buffer.c
        src/rest/rest_buffer_json_string.c
        src/rest/rest_capture.c
        src/rest/rest_channel_builder_base_json.c
        src/rest/rest_channel_builder_base_identity.c
        src/rest/rest_channel_builder_base_limits.c
        src/rest/rest_channel_builder_base_permissions.c
        src/rest/rest_channel_builder_context.c
        src/rest/rest_channel_builder_context_validate.c
        src/rest/rest_channel_builder_forum_details.c
        src/rest/rest_channel_builder_forum_settings.c
        src/rest/rest_channel_builder_json.c
        src/rest/rest_channel_builder_tail_json.c
        src/rest/rest_channel_actions.c
        src/rest/rest_channel_builders.c
        src/rest/rest_channel_forum_builders.c
        src/rest/rest_channel_invites.c
        src/rest/rest_channel_permissions.c
        src/rest/rest_channel_permission_builders.c
        src/rest/rest_channel_positions.c
        src/rest/rest_channels.c
        src/rest/rest_direct_messages.c
        src/rest/rest_direct_messages_json.c
        src/rest/rest_application_emojis.c
        src/rest/rest_application_emojis_params.c
        src/rest/rest_emoji_builders.c
        src/rest/rest_entitlements.c
        src/rest/rest_entitlements_test.c
        src/rest/rest_entitlements_test_body.c
        src/rest/rest_guild_emojis.c
        src/rest/rest_guild_emojis_params.c
        src/rest/rest_future_lifecycle.c
        src/rest/rest_future_request.c
        src/rest/rest_future_response.c
        src/rest/rest_future_wait.c
        src/rest/rest_future_wait_channel.c
        src/rest/rest_gateway.c
        src/rest/rest_group_dms.c
        src/rest/rest_header_parse.c
        src/rest/rest_headers.c
        src/rest/rest_interaction_followup_create.c
        src/rest/rest_interaction_followup_interaction_create.c
        src/rest/rest_interaction_followup_interaction_message.c
        src/rest/rest_interaction_followup_interaction_multipart.c
        src/rest/rest_interaction_followup_message.c
        src/rest/rest_interaction_followup_multipart.c
        src/rest/rest_interaction_helpers.c
        src/rest/rest_interaction_original.c
        src/rest/rest_interaction_original_edit.c
        src/rest/rest_interaction_original_interaction.c
        src/rest/rest_interaction_response_body.c
        src/rest/rest_interaction_response_builders.c
        src/rest/rest_interaction_response_named.c
        src/rest/rest_interaction_response_named_wrappers.c
        src/rest/rest_interaction_response_create.c
        src/rest/rest_interaction_response_wrappers.c
        src/rest/rest_intercept.c
        src/rest/firewall/rest_firewall_client.c
        src/rest/firewall/rest_firewall_options.c
        src/rest/firewall/rest_firewall_snapshot_json.c
        src/rest/firewall/rest_firewall_standalone.c
        src/rest/firewall/rest_firewall_state.c
        src/rest/rest_invites.c
        src/rest/rest_json.c
        src/rest/rest_json_array_members.c
        src/rest/rest_json_arrays.c
        src/rest/rest_json_member_key.c
        src/rest/rest_lifecycle.c
        src/rest/rest_guild_member_controls.c
        src/rest/rest_guild_member_builders.c
        src/rest/rest_guild_member_roles.c
        src/rest/rest_guild_members_lifecycle.c
        src/rest/rest_guild_members_query.c
        src/rest/rest_guild_bans_fetch.c
        src/rest/rest_guild_bans_create_params.c
        src/rest/rest_guild_bans_modify.c
        src/rest/rest_guild_body.c
        src/rest/rest_guild_body_tail.c
        src/rest/rest_guild_body_validate.c
        src/rest/rest_guild_prune_body.c
        src/rest/rest_guild_stickers.c
        src/rest/rest_guild_stickers_params.c
        src/rest/rest_guilds_audit_log.c
        src/rest/rest_guilds_core.c
        src/rest/rest_guilds_current_member.c
        src/rest/rest_guilds_current_member_body.c
        src/rest/rest_guilds_welcome_body.c
        src/rest/rest_guilds_integrations.c
        src/rest/rest_guilds_prune.c
        src/rest/rest_guilds_widget.c
        src/rest/rest_guilds_welcome.c
        src/rest/rest_message_builders.c
        src/rest/rest_message_actions.c
        src/rest/rest_message_create.c
        src/rest/rest_message_edit.c
        src/rest/rest_message_edit_builder.c
        src/rest/rest_message_edit_multipart.c
        src/rest/rest_message_flags.c
        src/rest/rest_message_pins.c
        src/rest/rest_message_polls.c
        src/rest/rest_message_reaction_paths.c
        src/rest/rest_message_reactions.c
        src/rest/rest_message_reactions_delete_all.c
        src/rest/rest_message_reactions_fetch.c
        src/rest/rest_messages.c
        src/rest/rest_message_search.c
        src/rest/rest_message_search_query.c
        src/rest/rest_message_search_query_filters.c
        src/rest/rest_message_search_query_options.c
        src/rest/rest_multipart.c
        src/rest/rest_multipart_build.c
        src/rest/rest_multipart_parts.c
        src/rest/rest_multipart_validate.c
        src/rest/rest_onboarding.c
        src/rest/rest_onboarding_builders.c
        src/rest/rest_onboarding_option_builders.c
        src/rest/rest_onboarding_prompt_builders.c
        src/rest/rest_methods.c
        src/rest/rest_path_escape.c
        src/rest/rest_paths.c
        src/rest/rest_platform.c
        src/rest/rest_platform_wait.c
        src/rest/rest_query_append.c
        src/rest/rest_query_collections.c
        src/rest/rest_query_collections_csv.c
        src/rest/rest_query_collections_search_has.c
        src/rest/rest_rate_limit.c
        src/rest/rest_rate_limit_buckets.c
        src/rest/rest_rate_limit_update.c
        src/rest/rest_request.c
        src/rest/rest_request_headers.c
        src/rest/rest_request_prepare.c
        src/rest/rest_request_rate_limit_response.c
        src/rest/rest_request_url.c
        src/rest/rest_role_connections.c
        src/rest/rest_role_connections_key.c
        src/rest/rest_role_connections_metadata_builder.c
        src/rest/rest_role_connections_user.c
        src/rest/rest_role_connections_user_builder.c
        src/rest/rest_role_body.c
        src/rest/rest_roles_create.c
        src/rest/rest_roles_delete.c
        src/rest/rest_roles_fetch.c
        src/rest/rest_roles_modify.c
        src/rest/rest_roles_positions.c
        src/rest/rest_routes.c
        src/rest/rest_routes_segments.c
        src/rest/rest_scheduled_event_body.c
        src/rest/rest_scheduled_event_builders.c
        src/rest/rest_scheduled_event_users.c
        src/rest/rest_scheduled_event_validate.c
        src/rest/rest_scheduled_events.c
        src/rest/rest_scheduled_events_params.c
        src/rest/rest_skus.c
        src/rest/rest_stickers.c
        src/rest/rest_stage_instances.c
        src/rest/rest_stage_instances_params.c
        src/rest/rest_template_body.c
        src/rest/rest_template_paths.c
        src/rest/rest_templates_create.c
        src/rest/rest_templates_fetch.c
        src/rest/rest_templates_modify.c
        src/rest/rest_templates_sync_delete.c
        src/rest/rest_thread_archives.c
        src/rest/rest_thread_archives_pages.c
        src/rest/rest_thread_body.c
        src/rest/rest_thread_body_create_members.c
        src/rest/rest_thread_body_members.c
        src/rest/rest_thread_body_modify_members.c
        src/rest/rest_thread_body_validate.c
        src/rest/rest_thread_create.c
        src/rest/rest_thread_members.c
        src/rest/rest_threads.c
        src/rest/rest_users_current.c
        src/rest/rest_users_dms.c
        src/rest/rest_users.c
        src/rest/rest_voice_regions.c
        src/rest/rest_voice_state_body.c
        src/rest/rest_voice_states_current_user.c
        src/rest/rest_voice_states_user.c
        src/rest/rest_webhook_body.c
        src/rest/rest_webhook_execute.c
        src/rest/rest_webhook_execute_multipart.c
        src/rest/rest_webhook_helpers.c
        src/rest/rest_webhook_messages_delete.c
        src/rest/rest_webhook_messages_edit.c
        src/rest/rest_webhook_messages_edit_builder.c
        src/rest/rest_webhook_messages_edit_multipart.c
        src/rest/rest_webhook_messages_fetch.c
        src/rest/rest_webhooks_create.c
        src/rest/rest_webhooks_delete.c
        src/rest/rest_webhooks_fetch.c
        src/rest/rest_webhooks_modify.c
        src/rest/rest.c
        src/rest/rest_request_multipart.c
        src/rest/rest_request_path.c
        src/runtime/task_group.c
        src/runtime/task_group_cancel.c
        src/runtime/task_group_entries.c
        src/runtime/task_group_options.c
        src/runtime/task_group_runtime_helpers.c
        src/runtime/task_group_spawn.c
        src/runtime/task_group_state.c
        src/runtime/task_group_status.c
        src/runtime/task_group_wait.c
        src/runtime/task_group_wait_any.c
        src/runtime/task_group_wait_cancel.c
        src/runtime/task_group_wait_result.c
        src/runtime/runtime.c
        src/runtime/runtime_control.c
        src/runtime/runtime_spawn.c
        src/voice/voice_crypto.c
        src/voice/voice_dave.c
        src/voice/voice_dave_client.c
        src/voice/voice_dave_client_binary.c
        src/voice/voice_dave_client_builders.c
        src/voice/voice_dave_gateway_build.c
        src/voice/voice_dave_gateway_parse.c
        src/voice/voice_dave_state.c
        src/voice/voice_connect_wait.c
        src/voice/voice_connect_wait_descriptor.c
        src/voice/voice_connect_wait_options.c
        src/voice/voice_connect_wait_result.c
        src/voice/voice_gateway.c
        src/voice/voice_gateway_binary.c
        src/voice/voice_gateway_client_events.c
        src/voice/voice_gateway_dave_events.c
        src/voice/voice_gateway_helpers.c
        src/voice/voice_gateway_ready.c
        src/voice/voice_hchacha20.c
        src/voice/voice_health_json.c
        src/voice/voice_health_snapshot.c
        src/voice/voice_health_state.c
        src/voice/voice_health_strings.c
        src/voice/voice_ip_discovery.c
        src/voice/voice_ip_discovery_packet.c
        src/voice/voice_json.c
        src/voice/voice_json_finish.c
        src/voice/voice_json_string.c
        src/voice/voice_notify.c
        src/voice/voice_notify_client.c
        src/voice/voice_notify_media.c
        src/voice/voice_opus.c
        src/voice/voice_opus_codec.c
        src/voice/voice_opus_decode.c
        src/voice/voice_opus_encode.c
        src/voice/voice_opus_lifecycle.c
        src/voice/voice_opus_options.c
        src/voice/voice_payloads_control.c
        src/voice/voice_payloads_sequence.c
        src/voice/voice_payloads.c
        src/voice/voice_receive.c
        src/voice/voice_receive_decode.c
        src/voice/voice_receive_mix.c
        src/voice/voice_receive_mix_flush.c
        src/voice/voice_receive_reorder.c
        src/voice/voice_receive_reorder_drain.c
        src/voice/voice_receive_reorder_slots.c
        src/voice/voice_receive_reorder_utils.c
        src/voice/voice_receive_rtp.c
        src/voice/voice_receive_rtp_payload.c
        src/voice/voice_receive_stats.c
        src/voice/voice_receive_timer.c
        src/voice/voice_receive_timer_task.c
        src/voice/voice_registry.c
        src/voice/voice_registry_gateway_events.c
        src/voice/voice_registry_lock.c
        src/voice/voice_rtp_state.c
        src/voice/voice_send_queue_core.c
        src/voice/voice_send_queue_markers.c
        src/voice/voice_send_queue_media.c
        src/voice/voice_reconnect.c
        src/voice/voice_reconnect_policy.c
        src/voice/voice_session.c
        src/voice/voice_session_events.c
        src/voice/voice_session_stats.c
        src/voice/voice_session_state.c
        src/voice/voice_session_transport.c
        src/voice/voice_ssrc.c
        src/voice/voice_ssrc_lookup.c
        src/voice/voice_ssrc_unmap.c
        src/voice/voice_transport.c
        src/voice/voice_transport_packet.c
        src/voice/voice_udp.c
        src/voice/voice_url.c
        src/voice/voice_utils.c
        src/voice/voice_wait.c
        src/voice/voice_ws_handshake.c
        src/voice/voice_ws_heartbeat.c
        src/voice/voice_ws_initial_payload.c
        src/voice/voice_ws_loop.c
        src/voice/voice_ws_loop_task.c
        src/voice/voice_ws_receive_loop.c
        src/voice/voice_ws_reconnect.c
        src/voice/voice_ws_send_current.c
        src/voice/voice_ws_state.c
        src/voice/voice_ws_task.c
        src/voice/voice_ws_timing.c
        src/voice/voice_ws.c
        src/voice/voice.c
        src/ws/dcc_ws.c
        src/ws/dcc_ws_connect.c
        src/ws/dcc_ws_handshake.c
        src/ws/dcc_ws_handshake_buffer.c
        src/ws/dcc_ws_handshake_crypto.c
        src/ws/dcc_ws_handshake_headers.c
        src/ws/dcc_ws_io.c
        src/ws/dcc_ws_recv.c
        src/ws/dcc_ws_send.c
        src/ws/dcc_ws_url.c
)

target_include_directories(dcc
    PUBLIC
        $<BUILD_INTERFACE:${DCC_PUBLIC_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${DCC_PRIVATE_INCLUDE_DIR}
)

target_link_libraries(dcc
    PUBLIC
        $<BUILD_INTERFACE:llam::runtime>
        $<INSTALL_INTERFACE:llam::runtime>
        Threads::Threads
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        ${CMAKE_DL_LIBS}
)
if(TARGET dcc_opus)
    target_link_libraries(dcc PRIVATE dcc_opus)
    target_compile_definitions(dcc PRIVATE DCC_HAVE_OPUS=1)
endif()
set(DCC_INTERFACE_LINK_LIBRARIES
    "$<BUILD_INTERFACE:llam::runtime>"
    "$<INSTALL_INTERFACE:llam::runtime>"
    "Threads::Threads"
    "$<LINK_ONLY:OpenSSL::SSL>"
    "$<LINK_ONLY:OpenSSL::Crypto>"
)
if(TARGET dcc_opus)
    list(APPEND DCC_INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:dcc_opus>")
endif()
if(CMAKE_DL_LIBS)
    list(APPEND DCC_INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:${CMAKE_DL_LIBS}>")
endif()
set_property(TARGET dcc PROPERTY INTERFACE_LINK_LIBRARIES "${DCC_INTERFACE_LINK_LIBRARIES}")
target_compile_definitions(dcc
    PUBLIC
        $<$<BOOL:${DCC_BUILD_SHARED}>:DCC_SHARED>
    PRIVATE
        DCC_BUILD
)

if(MSVC)
    target_compile_options(dcc PRIVATE /W4)
    target_compile_definitions(dcc PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(dcc PRIVATE -Wall -Wextra -Wpedantic)
endif()
dcc_target_enable_sanitizers(dcc)

if(DCC_BUILD_TOOLS OR DCC_BUILD_EXAMPLES OR DCC_BUILD_TESTS)
    add_executable(dcc_hot_reload_worker tools/dcc_hot_reload_worker.c)
    dcc_target_use_private_headers(dcc_hot_reload_worker)
    target_link_libraries(dcc_hot_reload_worker PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_worker)
    if(DCC_BUILD_TOOLS)
        install(
            TARGETS dcc_hot_reload_worker
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endif()

if(DCC_BUILD_TOOLS)
    add_executable(dcc_command_sync
        tools/dcc_command_sync.c
        tools/dcc_command_sync_file.c
        tools/dcc_command_sync_local.c
        tools/dcc_command_sync_options.c
        tools/dcc_command_sync_remote.c
        tools/dcc_command_sync_usage.c
    )
    dcc_target_use_private_headers(dcc_command_sync)
    target_link_libraries(dcc_command_sync PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_command_sync)
    install(
        TARGETS dcc_command_sync
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    add_executable(dcc_replay
        tools/dcc_replay.c
        tools/dcc_replay_frame.c
        tools/dcc_replay_options.c
        tools/dcc_replay_run.c
        tools/dcc_replay_runtime.c
        tools/dcc_replay_stats.c
        tools/dcc_replay_usage.c
    )
    dcc_target_use_private_headers(dcc_replay)
    target_link_libraries(dcc_replay PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_replay)
    install(
        TARGETS dcc_replay
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(DCC_BUILD_TOOLS OR DCC_BUILD_EXAMPLES)
    add_executable(dcc_hot_reload_host
        tools/dcc_hot_reload_host.c
        tools/dcc_hot_reload_host_check.c
        tools/dcc_hot_reload_host_check_paths.c
        tools/dcc_hot_reload_host_env.c
        tools/dcc_hot_reload_host_option_values.c
        tools/dcc_hot_reload_host_options.c
        tools/dcc_hot_reload_host_options_args.c
        tools/dcc_hot_reload_host_options_env.c
        tools/dcc_hot_reload_host_options_scope.c
        tools/dcc_hot_reload_host_options_value_args.c
        tools/dcc_hot_reload_host_options_value_parse.c
        tools/dcc_hot_reload_host_intents.c
        tools/dcc_hot_reload_host_parse.c
        tools/dcc_hot_reload_host_parse_intents.c
        tools/dcc_hot_reload_host_runtime.c
        tools/dcc_hot_reload_host_sidecar.c
        tools/dcc_hot_reload_host_usage.c
        tools/dcc_hot_reload_host_worker_candidate_paths.c
        tools/dcc_hot_reload_host_worker_candidates.c
        tools/dcc_hot_reload_host_worker_path.c
    )
    target_link_libraries(dcc_hot_reload_host PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_host)
    if(DCC_BUILD_TOOLS)
        install(
            TARGETS dcc_hot_reload_host
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()

    add_executable(dcc_interaction_webhook examples/interaction_webhook.c)
    target_link_libraries(dcc_interaction_webhook PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_interaction_webhook)
    if(DCC_BUILD_TOOLS)
        install(
            TARGETS dcc_interaction_webhook
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endif()

if(DCC_BUILD_EXAMPLES)
    add_executable(dcc_basic examples/basic.c)
    target_link_libraries(dcc_basic PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_basic)

    add_executable(dcc_pingpong examples/pingpong.c)
    target_link_libraries(dcc_pingpong PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_pingpong)

    add_executable(dcc_discord_game
        examples/discord_game.c
        examples/discord_game_ai.c
        examples/discord_game_command.c
        examples/discord_game_env.c
        examples/discord_game_gateway.c
        examples/discord_game_interactions.c
    )
    target_link_libraries(dcc_discord_game PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_discord_game)

    add_executable(dcc_cluster_rollout examples/cluster_rollout.c)
    target_link_libraries(dcc_cluster_rollout PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_cluster_rollout)

    add_executable(dcc_live_gateway_soak examples/live_gateway_soak.c)
    target_link_libraries(dcc_live_gateway_soak PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_live_gateway_soak)

    add_executable(dcc_live_voice_probe examples/live_voice_probe.c)
    target_link_libraries(dcc_live_voice_probe PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_live_voice_probe)

    add_library(dcc_hot_reload_bot MODULE examples/hot_reload_bot.c)
    target_include_directories(dcc_hot_reload_bot PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    target_link_libraries(dcc_hot_reload_bot PRIVATE dcc::dcc)
    set_target_properties(dcc_hot_reload_bot PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_bot)

    add_executable(dcc_hot_reload_test_host
        examples/hot_reload/host.c
    )
    target_link_libraries(dcc_hot_reload_test_host PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_test_host)

    set(DCC_HOT_RELOAD_TEST_LOCAL_BUILD_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/hot_reload/build")
    set(DCC_HOT_RELOAD_TEST_STAMP_HEADER
        "${DCC_HOT_RELOAD_TEST_LOCAL_BUILD_DIR}/hot_reload_stamp.h")
    file(MAKE_DIRECTORY "${DCC_HOT_RELOAD_TEST_LOCAL_BUILD_DIR}")
    if(NOT EXISTS "${DCC_HOT_RELOAD_TEST_STAMP_HEADER}")
        file(WRITE "${DCC_HOT_RELOAD_TEST_STAMP_HEADER}"
            "#ifndef DCC_HOT_RELOAD_STAMP_H\n"
            "#define DCC_HOT_RELOAD_STAMP_H\n"
            "#define DCC_HOT_RELOAD_TEST_BUILD_STAMP \"manual\"\n"
            "#endif\n"
        )
    endif()

    add_library(dcc_hot_reload_test_bot MODULE examples/hot_reload/bot.c)
    set_source_files_properties(examples/hot_reload/bot.c
        PROPERTIES
            OBJECT_DEPENDS "${DCC_HOT_RELOAD_TEST_STAMP_HEADER}"
    )
    target_include_directories(dcc_hot_reload_test_bot
        PRIVATE
            ${DCC_PUBLIC_INCLUDE_DIR}
            ${DCC_HOT_RELOAD_TEST_LOCAL_BUILD_DIR}
    )
    target_link_libraries(dcc_hot_reload_test_bot PRIVATE dcc::dcc)
    set_target_properties(dcc_hot_reload_test_bot PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_test_bot)
endif()

if(DCC_BUILD_TESTS)
    enable_testing()
    find_package(Python3 COMPONENTS Interpreter)

    add_executable(dcc_smoke tests/smoke.c)
    target_link_libraries(dcc_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_smoke)
    add_test(NAME dcc_smoke COMMAND dcc_smoke)

    add_executable(dcc_pkg_config_consumer_smoke tests/pkg_config_consumer/main.c)
    target_link_libraries(dcc_pkg_config_consumer_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_pkg_config_consumer_smoke)
    add_test(NAME dcc_pkg_config_consumer_smoke COMMAND dcc_pkg_config_consumer_smoke)

    add_executable(dcc_event_surface_smoke tests/event_surface_smoke.c)
    target_link_libraries(dcc_event_surface_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_event_surface_smoke)
    add_test(NAME dcc_event_surface_smoke COMMAND dcc_event_surface_smoke)

    add_executable(dcc_json_smoke
        tests/json_smoke.c
        tests/support/json_smoke_dom.c
        tests/support/json_smoke_gateway_extended.c
        tests/support/json_smoke_gateway_extended_presence.c
        tests/support/json_smoke_gateway_foundation.c
        tests/support/json_smoke_gateway_foundation_automation.c
        tests/support/json_smoke_gateway_foundation_guild.c
        tests/support/json_smoke_gateway_foundation_members.c
        tests/support/json_smoke_helpers.c
        tests/support/json_smoke_interactions.c
        tests/support/json_smoke_invalid.c
    )
    dcc_target_use_private_headers(dcc_json_smoke)
    target_link_libraries(dcc_json_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_json_smoke)
    add_test(NAME dcc_json_smoke COMMAND dcc_json_smoke)

    add_executable(dcc_http_smoke
        tests/http_smoke.c
        tests/support/http_smoke_async_basic.c
        tests/support/http_smoke_async_cancel_retry.c
        tests/support/http_smoke_async_misc.c
        tests/support/http_smoke_async_routes.c
        tests/support/http_smoke_bucket.c
        tests/support/http_smoke_builders.c
        tests/support/http_smoke_builders_commands.c
        tests/support/http_smoke_command_registry.c
        tests/support/http_smoke_common.c
        tests/support/http_smoke_gap.c
        tests/support/http_smoke_public.c
        tests/support/http_smoke_public_multipart.c
        tests/support/http_smoke_rest_calls_channels.c
        tests/support/http_smoke_rest_calls_guilds.c
        tests/support/http_smoke_rest_calls_users_threads.c
        tests/support/http_smoke_rest_calls_webhooks_apps.c
        tests/support/http_smoke_rest_wrapper_interaction_convenience.c
        tests/support/http_smoke_rest_wrapper.c
        tests/support/http_smoke_rest_wrapper_channels.c
        tests/support/http_smoke_rest_wrapper_guilds.c
        tests/support/http_smoke_rest_wrapper_interactions.c
        tests/support/http_smoke_rest_wrapper_users_threads.c
        tests/support/http_smoke_rest_wrapper_webhooks_apps.c
        tests/support/http_smoke_server.c
    )
    dcc_target_use_private_headers(dcc_http_smoke)
    target_link_libraries(dcc_http_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_http_smoke)
    add_test(NAME dcc_http_smoke COMMAND dcc_http_smoke)

    add_executable(dcc_component_session_smoke tests/component_session_smoke.c)
    target_link_libraries(dcc_component_session_smoke PRIVATE dcc::dcc)
    dcc_target_use_private_headers(dcc_component_session_smoke)
    dcc_target_enable_sanitizers(dcc_component_session_smoke)
    add_test(NAME dcc_component_session_smoke COMMAND dcc_component_session_smoke)

    add_executable(dcc_interaction_flow_smoke
        tests/interaction_flow_smoke.c
        tests/support/http_smoke_server.c
    )
    target_link_libraries(dcc_interaction_flow_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_interaction_flow_smoke)
    add_test(NAME dcc_interaction_flow_smoke COMMAND dcc_interaction_flow_smoke)

    add_executable(dcc_replay_smoke tests/replay_smoke.c)
    target_link_libraries(dcc_replay_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_replay_smoke)
    add_test(NAME dcc_replay_smoke COMMAND dcc_replay_smoke)

    add_executable(dcc_hot_reload_canary_smoke tests/hot_reload_canary_smoke.c)
    target_link_libraries(dcc_hot_reload_canary_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_canary_smoke)
    add_test(NAME dcc_hot_reload_canary_smoke COMMAND dcc_hot_reload_canary_smoke)

    add_executable(dcc_rest_firewall_smoke
        tests/rest_firewall_smoke.c
        tests/support/http_smoke_server.c
    )
    dcc_target_use_test_headers(dcc_rest_firewall_smoke)
    target_link_libraries(dcc_rest_firewall_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_rest_firewall_smoke)
    add_test(NAME dcc_rest_firewall_smoke COMMAND dcc_rest_firewall_smoke)

    add_executable(dcc_interaction_server_smoke
        tests/interaction_server_smoke.c
        tests/support/interaction_server_smoke_handlers.c
        tests/support/interaction_server_smoke_health.c
        tests/support/interaction_server_smoke_http.c
        tests/support/interaction_server_smoke_scenarios.c
    )
    target_link_libraries(dcc_interaction_server_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_interaction_server_smoke)
    add_test(NAME dcc_interaction_server_smoke COMMAND dcc_interaction_server_smoke)

    add_executable(dcc_ws_smoke tests/ws_smoke.c)
    dcc_target_use_private_headers(dcc_ws_smoke)
    target_link_libraries(dcc_ws_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_ws_smoke)
    add_test(NAME dcc_ws_smoke COMMAND dcc_ws_smoke)

    add_executable(dcc_gateway_smoke
        tests/gateway_smoke.c
        tests/support/gateway_smoke_api_server.c
        tests/support/gateway_smoke_cache.c
        tests/support/gateway_smoke_handlers_core.c
        tests/support/gateway_smoke_handlers_channels.c
        tests/support/gateway_smoke_handlers_guild_create.c
        tests/support/gateway_smoke_handlers_guilds.c
        tests/support/gateway_smoke_handlers_interactions.c
        tests/support/gateway_smoke_handlers_list_updates.c
        tests/support/gateway_smoke_handlers_members.c
        tests/support/gateway_smoke_handlers_messages.c
        tests/support/gateway_smoke_handlers_moderation.c
        tests/support/gateway_smoke_handlers_misc.c
        tests/support/gateway_smoke_handlers_presence.c
        tests/support/gateway_smoke_handlers_reactions.c
        tests/support/gateway_smoke_handlers_resources.c
        tests/support/gateway_smoke_handlers_roles.c
        tests/support/gateway_smoke_handlers_stage_soundboard.c
        tests/support/gateway_smoke_handlers_voice.c
        tests/support/gateway_smoke_server.c
        tests/support/gateway_smoke_server_core_dispatch.c
        tests/support/gateway_smoke_server_dispatch.c
        tests/support/gateway_smoke_state.c
        tests/support/gateway_smoke_ws.c
    )
    target_link_libraries(dcc_gateway_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_gateway_smoke)
    add_test(NAME dcc_gateway_smoke COMMAND dcc_gateway_smoke)

    add_executable(dcc_gateway_control_smoke tests/gateway_control_smoke.c)
    target_link_libraries(dcc_gateway_control_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_gateway_control_smoke)
    add_test(NAME dcc_gateway_control_smoke COMMAND dcc_gateway_control_smoke)

    add_executable(dcc_gateway_chaos_smoke tests/gateway_chaos_smoke.c)
    target_link_libraries(dcc_gateway_chaos_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_gateway_chaos_smoke)
    add_test(NAME dcc_gateway_chaos_smoke COMMAND dcc_gateway_chaos_smoke)

    add_executable(dcc_event_object_wait_smoke
        tests/event_object_wait_smoke.c
        tests/support/event_object_wait_smoke_invalid.c
        tests/support/event_object_wait_smoke_runtime.c
        tests/support/event_object_wait_smoke_server.c
    )
    target_link_libraries(dcc_event_object_wait_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_event_object_wait_smoke)
    add_test(NAME dcc_event_object_wait_smoke COMMAND dcc_event_object_wait_smoke)

    add_executable(dcc_event_wait_convenience_smoke
        tests/event_wait_convenience_smoke.c
        tests/support/event_wait_convenience_smoke_support.c
    )
    dcc_target_use_private_headers(dcc_event_wait_convenience_smoke)
    target_link_libraries(dcc_event_wait_convenience_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_event_wait_convenience_smoke)
    add_test(NAME dcc_event_wait_convenience_smoke COMMAND dcc_event_wait_convenience_smoke)

    add_executable(dcc_task_group_smoke tests/task_group_smoke.c)
    target_link_libraries(dcc_task_group_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_task_group_smoke)
    add_test(NAME dcc_task_group_smoke COMMAND dcc_task_group_smoke)

    add_executable(dcc_cluster_smoke
        tests/cluster_smoke.c
        tests/support/cluster_smoke_callbacks.c
        tests/support/cluster_smoke_contracts.c
        tests/support/cluster_smoke_json.c
        tests/support/cluster_smoke_operations.c
        tests/support/cluster_smoke_runtime.c
    )
    target_link_libraries(dcc_cluster_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_cluster_smoke)
    add_test(NAME dcc_cluster_smoke COMMAND dcc_cluster_smoke)

    add_executable(dcc_cluster_status_route_smoke tests/cluster_status_route_smoke.c)
    target_link_libraries(dcc_cluster_status_route_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_cluster_status_route_smoke)
    add_test(NAME dcc_cluster_status_route_smoke COMMAND dcc_cluster_status_route_smoke)

    add_executable(dcc_cluster_gateway_smoke
        tests/cluster_gateway_smoke.c
        tests/support/cluster_gateway_smoke_server.c
    )
    target_link_libraries(dcc_cluster_gateway_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_cluster_gateway_smoke)
    add_test(NAME dcc_cluster_gateway_smoke COMMAND dcc_cluster_gateway_smoke)

    add_executable(dcc_cluster_chaos_smoke
        tests/cluster_chaos_smoke.c
        tests/support/cluster_chaos_smoke_server.c
    )
    target_link_libraries(dcc_cluster_chaos_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_cluster_chaos_smoke)
    add_test(NAME dcc_cluster_chaos_smoke COMMAND dcc_cluster_chaos_smoke)

    add_executable(dcc_cluster_identify_budget_smoke tests/cluster_identify_budget_smoke.c)
    target_link_libraries(dcc_cluster_identify_budget_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_cluster_identify_budget_smoke)
    add_test(NAME dcc_cluster_identify_budget_smoke COMMAND dcc_cluster_identify_budget_smoke)

    add_executable(dcc_cluster_restart_smoke tests/cluster_restart_smoke.c)
    target_link_libraries(dcc_cluster_restart_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_cluster_restart_smoke)
    add_test(NAME dcc_cluster_restart_smoke COMMAND dcc_cluster_restart_smoke)

    add_executable(dcc_cluster_restart_suppress_smoke
        tests/cluster_restart_suppress_smoke.c
        tests/support/cluster_restart_suppress_smoke_monitor.c
        tests/support/cluster_restart_suppress_smoke_server.c
    )
    target_link_libraries(dcc_cluster_restart_suppress_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_cluster_restart_suppress_smoke)
    add_test(NAME dcc_cluster_restart_suppress_smoke COMMAND dcc_cluster_restart_suppress_smoke)

    add_executable(dcc_cluster_supervisor_policy_smoke tests/cluster_supervisor_policy_smoke.c)
    target_link_libraries(dcc_cluster_supervisor_policy_smoke PRIVATE dcc::dcc)
    dcc_target_use_private_headers(dcc_cluster_supervisor_policy_smoke)
    dcc_target_enable_sanitizers(dcc_cluster_supervisor_policy_smoke)
    add_test(NAME dcc_cluster_supervisor_policy_smoke COMMAND dcc_cluster_supervisor_policy_smoke)

    add_executable(dcc_cluster_supervisor_policy_restart_smoke tests/cluster_supervisor_policy_restart_smoke.c)
    target_link_libraries(dcc_cluster_supervisor_policy_restart_smoke PRIVATE dcc::dcc)
    dcc_target_use_private_headers(dcc_cluster_supervisor_policy_restart_smoke)
    dcc_target_enable_sanitizers(dcc_cluster_supervisor_policy_restart_smoke)
    add_test(NAME dcc_cluster_supervisor_policy_restart_smoke COMMAND dcc_cluster_supervisor_policy_restart_smoke)

    add_executable(dcc_voice_smoke
        tests/voice_smoke.c
        tests/support/voice_smoke_common.c
        tests/support/voice_smoke_dave.c
        tests/support/voice_smoke_invalid_args.c
        tests/support/voice_smoke_rtp.c
        tests/support/voice_smoke_session.c
        tests/support/voice_smoke_udp.c
        tests/support/voice_smoke_websocket_full_reconnect.c
        tests/support/voice_smoke_websocket.c
        tests/support/voice_smoke_websocket_loop.c
        tests/support/voice_smoke_websocket_managed.c
        tests/support/voice_smoke_ws_frame.c
        tests/support/voice_smoke_ws_full_reconnect.c
        tests/support/voice_smoke_ws_loop.c
        tests/support/voice_smoke_ws_support.c
    )
    target_link_libraries(dcc_voice_smoke PRIVATE dcc::dcc OpenSSL::Crypto)
    dcc_target_enable_sanitizers(dcc_voice_smoke)
    add_test(NAME dcc_voice_smoke COMMAND dcc_voice_smoke)

    add_executable(dcc_voice_wait_smoke tests/voice_wait_smoke.c)
    target_link_libraries(dcc_voice_wait_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_voice_wait_smoke)
    add_test(NAME dcc_voice_wait_smoke COMMAND dcc_voice_wait_smoke)

    add_library(dcc_hot_reload_module_v1 MODULE tests/hot_reload_module_v1.c)
    target_include_directories(dcc_hot_reload_module_v1 PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    dcc_target_use_test_headers(dcc_hot_reload_module_v1)
    set_target_properties(dcc_hot_reload_module_v1 PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_module_v1)

    add_library(dcc_hot_reload_module_v2 MODULE tests/hot_reload_module_v2.c)
    target_include_directories(dcc_hot_reload_module_v2 PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    dcc_target_use_test_headers(dcc_hot_reload_module_v2)
    set_target_properties(dcc_hot_reload_module_v2 PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_module_v2)

    add_library(dcc_hot_reload_module_bad MODULE tests/hot_reload_module_bad.c)
    target_include_directories(dcc_hot_reload_module_bad PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    dcc_target_use_test_headers(dcc_hot_reload_module_bad)
    set_target_properties(dcc_hot_reload_module_bad PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_module_bad)

    add_library(dcc_hot_reload_module_exit MODULE tests/hot_reload_module_exit.c)
    target_include_directories(dcc_hot_reload_module_exit PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    set_target_properties(dcc_hot_reload_module_exit PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_module_exit)

    add_library(dcc_hot_reload_module_hang MODULE tests/hot_reload_module_hang.c)
    target_include_directories(dcc_hot_reload_module_hang PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    set_target_properties(dcc_hot_reload_module_hang PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_module_hang)

    add_library(dcc_hot_reload_isolated_module MODULE tests/hot_reload_isolated_module.c)
    target_include_directories(dcc_hot_reload_isolated_module PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    target_link_libraries(dcc_hot_reload_isolated_module PRIVATE dcc::dcc)
    set_target_properties(dcc_hot_reload_isolated_module PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_module)

    add_library(dcc_hot_reload_isolated_crash_module MODULE tests/hot_reload_isolated_crash_module.c)
    target_include_directories(dcc_hot_reload_isolated_crash_module PRIVATE ${DCC_PUBLIC_INCLUDE_DIR})
    target_link_libraries(dcc_hot_reload_isolated_crash_module PRIVATE dcc::dcc)
    set_target_properties(dcc_hot_reload_isolated_crash_module PROPERTIES PREFIX "")
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_crash_module)

    add_executable(dcc_hot_reload_smoke
        tests/hot_reload_smoke.c
        tests/support/hot_reload_isolated_support.c
        tests/support/hot_reload_smoke_support.c
    )
    dcc_target_use_private_headers(dcc_hot_reload_smoke)
    dcc_target_use_test_headers(dcc_hot_reload_smoke)
    target_compile_definitions(dcc_hot_reload_smoke PRIVATE
        "DCC_HOT_RELOAD_MODULE_V1=\"$<TARGET_FILE:dcc_hot_reload_module_v1>\""
        "DCC_HOT_RELOAD_MODULE_V2=\"$<TARGET_FILE:dcc_hot_reload_module_v2>\""
        "DCC_HOT_RELOAD_MODULE_BAD=\"$<TARGET_FILE:dcc_hot_reload_module_bad>\""
    )
    target_link_libraries(dcc_hot_reload_smoke PRIVATE dcc::dcc)
    add_dependencies(dcc_hot_reload_smoke dcc_hot_reload_module_v1 dcc_hot_reload_module_v2 dcc_hot_reload_module_bad)
    dcc_target_enable_sanitizers(dcc_hot_reload_smoke)
    add_test(NAME dcc_hot_reload_smoke COMMAND dcc_hot_reload_smoke)

    add_executable(dcc_hot_reload_health_route_smoke tests/hot_reload_health_route_smoke.c)
    target_link_libraries(dcc_hot_reload_health_route_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_health_route_smoke)
    add_test(NAME dcc_hot_reload_health_route_smoke COMMAND dcc_hot_reload_health_route_smoke)

    add_executable(dcc_hot_reload_isolated_smoke tests/hot_reload_isolated_smoke.c)
    dcc_target_use_private_headers(dcc_hot_reload_isolated_smoke)
    target_compile_definitions(dcc_hot_reload_isolated_smoke PRIVATE
        "DCC_HOT_RELOAD_ISOLATED_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_module>\""
        "DCC_HOT_RELOAD_WORKER=\"$<TARGET_FILE:dcc_hot_reload_worker>\""
    )
    target_link_libraries(dcc_hot_reload_isolated_smoke PRIVATE dcc::dcc)
    add_dependencies(dcc_hot_reload_isolated_smoke dcc_hot_reload_isolated_module dcc_hot_reload_worker)
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_smoke)
    add_test(NAME dcc_hot_reload_isolated_smoke COMMAND dcc_hot_reload_isolated_smoke)

    add_executable(dcc_hot_reload_canary_integration_smoke
        tests/hot_reload_canary_integration_smoke.c
        tests/support/hot_reload_isolated_support.c
    )
    dcc_target_use_private_headers(dcc_hot_reload_canary_integration_smoke)
    target_compile_definitions(dcc_hot_reload_canary_integration_smoke PRIVATE
        "DCC_HOT_RELOAD_ISOLATED_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_module>\""
        "DCC_HOT_RELOAD_ISOLATED_CRASH_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_crash_module>\""
        "DCC_HOT_RELOAD_WORKER=\"$<TARGET_FILE:dcc_hot_reload_worker>\""
    )
    target_link_libraries(dcc_hot_reload_canary_integration_smoke PRIVATE dcc::dcc)
    add_dependencies(
        dcc_hot_reload_canary_integration_smoke
        dcc_hot_reload_isolated_module
        dcc_hot_reload_isolated_crash_module
        dcc_hot_reload_worker
    )
    dcc_target_enable_sanitizers(dcc_hot_reload_canary_integration_smoke)
    add_test(NAME dcc_hot_reload_canary_integration_smoke COMMAND dcc_hot_reload_canary_integration_smoke)

    add_executable(dcc_hot_reload_isolated_runner_smoke tests/hot_reload_isolated_runner_smoke.c)
    target_link_libraries(dcc_hot_reload_isolated_runner_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_runner_smoke)
    add_test(NAME dcc_hot_reload_isolated_runner_smoke COMMAND dcc_hot_reload_isolated_runner_smoke)

    add_executable(dcc_hot_reload_worker_protocol_smoke tests/hot_reload_worker_protocol_smoke.c)
    dcc_target_use_private_headers(dcc_hot_reload_worker_protocol_smoke)
    target_link_libraries(dcc_hot_reload_worker_protocol_smoke PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_worker_protocol_smoke)
    add_test(NAME dcc_hot_reload_worker_protocol_smoke COMMAND dcc_hot_reload_worker_protocol_smoke)

    add_executable(dcc_hot_reload_invalid_ready_worker tests/hot_reload_invalid_ready_worker.c)
    dcc_target_use_private_headers(dcc_hot_reload_invalid_ready_worker)
    target_link_libraries(dcc_hot_reload_invalid_ready_worker PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_hot_reload_invalid_ready_worker)

    add_executable(dcc_hot_reload_isolated_drain_smoke tests/hot_reload_isolated_drain_smoke.c)
    dcc_target_use_private_headers(dcc_hot_reload_isolated_drain_smoke)
    target_compile_definitions(dcc_hot_reload_isolated_drain_smoke PRIVATE
        "DCC_HOT_RELOAD_ISOLATED_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_module>\""
        "DCC_HOT_RELOAD_WORKER=\"$<TARGET_FILE:dcc_hot_reload_worker>\""
    )
    target_link_libraries(dcc_hot_reload_isolated_drain_smoke PRIVATE dcc::dcc)
    add_dependencies(dcc_hot_reload_isolated_drain_smoke dcc_hot_reload_isolated_module dcc_hot_reload_worker)
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_drain_smoke)
    add_test(NAME dcc_hot_reload_isolated_drain_smoke COMMAND dcc_hot_reload_isolated_drain_smoke)

    if(TARGET dcc_hot_reload_host)
        set(DCC_HOT_RELOAD_HOST_ENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/hot_reload_host_env")
        file(MAKE_DIRECTORY "${DCC_HOT_RELOAD_HOST_ENV_DIR}")
        file(GENERATE
            OUTPUT "${DCC_HOT_RELOAD_HOST_ENV_DIR}/.env"
            CONTENT "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>\nDCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>\nDISCORD_TOKEN=fixture-token\nDCC_HOT_RELOAD_INTENTS=guilds,guild_messages\n"
        )

        add_test(NAME dcc_hot_reload_host_help COMMAND dcc_hot_reload_host --help)
        set_tests_properties(dcc_hot_reload_host_help PROPERTIES
            PASS_REGULAR_EXPRESSION "DCC_HOT_RELOAD_INTENTS"
        )

        add_test(NAME dcc_hot_reload_host_invalid_intents
            COMMAND ${CMAKE_COMMAND} -E env
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --intents definitely_not_an_intent
        )
        set_tests_properties(dcc_hot_reload_host_invalid_intents PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_invalid_guild
            COMMAND ${CMAKE_COMMAND} -E env
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --guild definitely_not_a_snowflake
        )
        set_tests_properties(dcc_hot_reload_host_invalid_guild PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_invalid_health_port
            COMMAND ${CMAKE_COMMAND} -E env
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --health-port definitely_not_a_port
        )
        set_tests_properties(dcc_hot_reload_host_invalid_health_port PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_invalid_worker_health
            COMMAND ${CMAKE_COMMAND} -E env
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --worker-health-ms 0
        )
        set_tests_properties(dcc_hot_reload_host_invalid_worker_health PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_check_missing
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                --unset=DISCORD_TOKEN
                --unset=DCC_BOT_MODULE
                --unset=DCC_HOT_RELOAD_WORKER
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_missing PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_check_ok
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_INTENTS=guilds,guild_messages"
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_ok PROPERTIES
            PASS_REGULAR_EXPRESSION "token=set"
        )

        add_test(NAME dcc_hot_reload_host_check_health_sidecar
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "$<TARGET_FILE:dcc_hot_reload_host>" --health-address 127.0.0.1 --health-port 18080 --check
        )
        set_tests_properties(dcc_hot_reload_host_check_health_sidecar PROPERTIES
            PASS_REGULAR_EXPRESSION "health_port=18080"
        )

        add_test(NAME dcc_hot_reload_host_check_tuning
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_INTENTS=guilds"
                "DCC_HOT_RELOAD_POLL_MS=125"
                "DCC_HOT_RELOAD_WORKER_HEALTH_MS=2400"
                "$<TARGET_FILE:dcc_hot_reload_host>" --settle-ms none --worker-drain-ms 25 --check
        )
        set_tests_properties(dcc_hot_reload_host_check_tuning PROPERTIES
            PASS_REGULAR_EXPRESSION "poll_interval_ms=125.*settle_interval_ms=4294967295.*worker_health_timeout_ms=2400.*worker_drain_timeout_ms=25"
        )

        add_test(NAME dcc_hot_reload_host_check_guild_scope
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_GUILD_ID=invalid-env-value"
                "$<TARGET_FILE:dcc_hot_reload_host>" --guild 123456789012345678 --check
        )
        set_tests_properties(dcc_hot_reload_host_check_guild_scope PROPERTIES
            PASS_REGULAR_EXPRESSION "command_scope=guild"
        )

        add_test(NAME dcc_hot_reload_host_check_global_scope
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_GUILD_ID=123456789012345678"
                "$<TARGET_FILE:dcc_hot_reload_host>" --global --check
        )
        set_tests_properties(dcc_hot_reload_host_check_global_scope PROPERTIES
            PASS_REGULAR_EXPRESSION "command_scope=global"
        )

        add_test(NAME dcc_hot_reload_host_check_invalid_guild_env
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "DCC_HOT_RELOAD_GUILD_ID=invalid-env-value"
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_invalid_guild_env PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_check_missing_module_file
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=${CMAKE_CURRENT_BINARY_DIR}/missing-hot-reload-module.so"
                "DCC_HOT_RELOAD_WORKER=$<TARGET_FILE:dcc_hot_reload_worker>"
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_missing_module_file PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_check_missing_worker_file
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                "DISCORD_TOKEN=test-token"
                "DCC_BOT_MODULE=$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                "DCC_HOT_RELOAD_WORKER=dcc_hot_reload_worker_missing_for_ctest_4e8d7a0d"
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_missing_worker_file PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_hot_reload_host_check_dotenv
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=BOT_TOKEN
                --unset=DISCORD_TOKEN
                --unset=DCC_BOT_MODULE
                --unset=DCC_HOT_RELOAD_WORKER
                --unset=DCC_HOT_RELOAD_INTENTS
                "$<TARGET_FILE:dcc_hot_reload_host>" --check
        )
        set_tests_properties(dcc_hot_reload_host_check_dotenv PROPERTIES
            WORKING_DIRECTORY "${DCC_HOT_RELOAD_HOST_ENV_DIR}"
            PASS_REGULAR_EXPRESSION "worker_found=yes"
        )
    endif()

    if(TARGET dcc_command_sync)
        add_test(NAME dcc_command_sync_help COMMAND dcc_command_sync --help)
        set_tests_properties(dcc_command_sync_help PROPERTIES
            PASS_REGULAR_EXPRESSION "delete-stale"
        )

        add_test(NAME dcc_command_sync_plan_fixture
            COMMAND "$<TARGET_FILE:dcc_command_sync>"
                --commands "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/command_registry_local.json"
                --remote "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/command_registry_remote.json"
                --delete-stale
                --plan
        )
        set_tests_properties(dcc_command_sync_plan_fixture PROPERTIES
            PASS_REGULAR_EXPRESSION "summary create=1 update=1 delete_stale=1 noop=1"
        )

        add_test(NAME dcc_command_sync_check_fixture
            COMMAND "$<TARGET_FILE:dcc_command_sync>"
                --commands "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/command_registry_local.json"
                --remote "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/command_registry_remote.json"
                --guild 123456789012345678
                --check
        )
        set_tests_properties(dcc_command_sync_check_fixture PROPERTIES
            PASS_REGULAR_EXPRESSION "check=ok local_commands=3 remote_commands=3 scope=guild"
        )

        add_test(NAME dcc_command_sync_missing_commands
            COMMAND "$<TARGET_FILE:dcc_command_sync>" --remote "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/command_registry_remote.json"
        )
        set_tests_properties(dcc_command_sync_missing_commands PROPERTIES
            WILL_FAIL TRUE
        )
    endif()

    if(TARGET dcc_replay)
        add_test(NAME dcc_replay_tool_help COMMAND dcc_replay --help)
        set_tests_properties(dcc_replay_tool_help PROPERTIES
            PASS_REGULAR_EXPRESSION "events.jsonl"
        )

        add_test(NAME dcc_replay_tool_summary
            COMMAND "$<TARGET_FILE:dcc_replay>"
                --file "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/replay_events.jsonl"
                --summary
                --validate
        )
        set_tests_properties(dcc_replay_tool_summary PROPERTIES
            PASS_REGULAR_EXPRESSION "records=2 gateway=1 interactions=1"
        )

        if(TARGET dcc_hot_reload_isolated_module AND TARGET dcc_hot_reload_worker)
            add_test(NAME dcc_replay_tool_module_ready
                COMMAND "$<TARGET_FILE:dcc_replay>"
                    --file "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/replay_ready.jsonl"
                    --module "$<TARGET_FILE:dcc_hot_reload_isolated_module>"
                    --worker "$<TARGET_FILE:dcc_hot_reload_worker>"
                    --summary
            )
            set_tests_properties(dcc_replay_tool_module_ready PROPERTIES
                PASS_REGULAR_EXPRESSION "records=1 gateway=1 interactions=0"
            )
        endif()
    endif()

    if(TARGET dcc_interaction_webhook)
        add_test(NAME dcc_interaction_webhook_help COMMAND dcc_interaction_webhook --help)
        set_tests_properties(dcc_interaction_webhook_help PROPERTIES
            PASS_REGULAR_EXPRESSION "DISCORD_PUBLIC_KEY"
        )

        add_test(NAME dcc_interaction_webhook_check_missing_key
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=DISCORD_PUBLIC_KEY
                "$<TARGET_FILE:dcc_interaction_webhook>" --check
        )
        set_tests_properties(dcc_interaction_webhook_check_missing_key PROPERTIES
            WILL_FAIL TRUE
        )

        add_test(NAME dcc_interaction_webhook_check_ok
            COMMAND ${CMAKE_COMMAND} -E env
                "DISCORD_PUBLIC_KEY=0000000000000000000000000000000000000000000000000000000000000000"
                "DCC_INTERACTION_ADDRESS=127.0.0.1"
                "DCC_INTERACTION_PORT=8080"
                "DCC_INTERACTION_PATH=/interactions"
                "$<TARGET_FILE:dcc_interaction_webhook>" --check
        )
        set_tests_properties(dcc_interaction_webhook_check_ok PROPERTIES
            PASS_REGULAR_EXPRESSION "server_create=yes.*routes=ready"
        )
    endif()

    add_executable(
        dcc_hot_reload_isolated_last_good_smoke
        tests/hot_reload_isolated_last_good_smoke.c
        tests/support/hot_reload_isolated_support.c
        tests/support/hot_reload_isolated_candidate_failures.c
        tests/support/hot_reload_isolated_last_good_candidates.c
        tests/support/hot_reload_isolated_last_good_dispatch.c
        tests/support/hot_reload_isolated_last_good_retry.c
    )
    dcc_target_use_private_headers(dcc_hot_reload_isolated_last_good_smoke)
    target_compile_definitions(dcc_hot_reload_isolated_last_good_smoke PRIVATE
        "DCC_HOT_RELOAD_ISOLATED_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_module>\""
        "DCC_HOT_RELOAD_ISOLATED_CRASH_MODULE=\"$<TARGET_FILE:dcc_hot_reload_isolated_crash_module>\""
        "DCC_HOT_RELOAD_ISOLATED_BAD_MODULE=\"$<TARGET_FILE:dcc_hot_reload_module_bad>\""
        "DCC_HOT_RELOAD_ISOLATED_EXIT_MODULE=\"$<TARGET_FILE:dcc_hot_reload_module_exit>\""
        "DCC_HOT_RELOAD_ISOLATED_HANG_MODULE=\"$<TARGET_FILE:dcc_hot_reload_module_hang>\""
        "DCC_HOT_RELOAD_INVALID_READY_WORKER=\"$<TARGET_FILE:dcc_hot_reload_invalid_ready_worker>\""
        "DCC_HOT_RELOAD_WORKER=\"$<TARGET_FILE:dcc_hot_reload_worker>\""
    )
    target_link_libraries(dcc_hot_reload_isolated_last_good_smoke PRIVATE dcc::dcc)
    add_dependencies(
        dcc_hot_reload_isolated_last_good_smoke
        dcc_hot_reload_isolated_module
        dcc_hot_reload_isolated_crash_module
        dcc_hot_reload_module_bad
        dcc_hot_reload_module_exit
        dcc_hot_reload_module_hang
        dcc_hot_reload_invalid_ready_worker
        dcc_hot_reload_worker
    )
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_last_good_smoke)
    add_test(NAME dcc_hot_reload_isolated_last_good_smoke COMMAND dcc_hot_reload_isolated_last_good_smoke)

    add_executable(
        dcc_hot_reload_isolated_health_smoke
        tests/hot_reload_isolated_health_smoke.c
        tests/support/hot_reload_isolated_support.c
    )
    dcc_target_use_private_headers(dcc_hot_reload_isolated_health_smoke)
    target_compile_definitions(dcc_hot_reload_isolated_health_smoke PRIVATE
        "DCC_HOT_RELOAD_INVALID_READY_WORKER=\"$<TARGET_FILE:dcc_hot_reload_invalid_ready_worker>\""
    )
    target_link_libraries(dcc_hot_reload_isolated_health_smoke PRIVATE dcc::dcc)
    add_dependencies(
        dcc_hot_reload_isolated_health_smoke
        dcc_hot_reload_invalid_ready_worker
    )
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_health_smoke)
    add_test(NAME dcc_hot_reload_isolated_health_smoke COMMAND dcc_hot_reload_isolated_health_smoke)

    add_executable(
        dcc_hot_reload_isolated_dispatch_failure_smoke
        tests/hot_reload_isolated_dispatch_failure_smoke.c
        tests/support/hot_reload_isolated_support.c
    )
    dcc_target_use_private_headers(dcc_hot_reload_isolated_dispatch_failure_smoke)
    target_compile_definitions(dcc_hot_reload_isolated_dispatch_failure_smoke PRIVATE
        "DCC_HOT_RELOAD_INVALID_READY_WORKER=\"$<TARGET_FILE:dcc_hot_reload_invalid_ready_worker>\""
    )
    target_link_libraries(dcc_hot_reload_isolated_dispatch_failure_smoke PRIVATE dcc::dcc)
    add_dependencies(
        dcc_hot_reload_isolated_dispatch_failure_smoke
        dcc_hot_reload_invalid_ready_worker
    )
    dcc_target_enable_sanitizers(dcc_hot_reload_isolated_dispatch_failure_smoke)
    add_test(
        NAME dcc_hot_reload_isolated_dispatch_failure_smoke
        COMMAND dcc_hot_reload_isolated_dispatch_failure_smoke
    )

    if(Python3_Interpreter_FOUND)
        add_test(
            NAME dcc_gateway_fast_path_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_gateway_fast_path.py
        )
        set_tests_properties(dcc_gateway_fast_path_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_gateway_payload_field_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_gateway_payload_fields.py
        )
        set_tests_properties(dcc_gateway_payload_field_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_event_surface_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_event_surface.py
        )
        set_tests_properties(dcc_event_surface_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_rest_surface_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_rest_surface.py
        )
        set_tests_properties(dcc_rest_surface_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_rest_request_assertion_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_rest_request_assertions.py
        )
        set_tests_properties(dcc_rest_request_assertion_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_rest_field_smoke_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_rest_field_smoke.py
        )
        set_tests_properties(dcc_rest_field_smoke_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_capacity_growth_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_capacity_growth.py
        )
        set_tests_properties(dcc_capacity_growth_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_project_layout_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_project_layout.py
        )
        set_tests_properties(dcc_project_layout_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_source_package_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_source_package.py
        )
        set_tests_properties(dcc_source_package_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_test(
            NAME dcc_deploy_template_audit
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_deploy_templates.py
        )
        set_tests_properties(dcc_deploy_template_audit PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        message(WARNING "Python3 interpreter not found; source audit tests are disabled")
    endif()
endif()

if(DCC_BUILD_BENCHMARKS)
    add_executable(dcc_json_gateway_bench tests/json_gateway_bench.c)
    dcc_target_use_private_headers(dcc_json_gateway_bench)
    target_link_libraries(dcc_json_gateway_bench PRIVATE dcc::dcc)
    dcc_target_enable_sanitizers(dcc_json_gateway_bench)
endif()

if(DCC_BUILD_FUZZERS)
    set(DCC_SAVED_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=fuzzer,address,undefined")
    check_c_source_compiles("
        #include <stddef.h>
        #include <stdint.h>
        int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
            (void)data;
            (void)size;
            return 0;
        }
    " DCC_HAS_LIBFUZZER_RUNTIME)
    set(CMAKE_REQUIRED_FLAGS "${DCC_SAVED_REQUIRED_FLAGS}")

    add_executable(dcc_json_fuzzer fuzz/json_fuzzer.c)
    dcc_target_use_private_headers(dcc_json_fuzzer)
    target_link_libraries(dcc_json_fuzzer PRIVATE dcc::dcc)
    if(DCC_HAS_LIBFUZZER_RUNTIME)
        target_compile_options(dcc_json_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(dcc_json_fuzzer PRIVATE -fsanitize=fuzzer,address,undefined)
    else()
        message(WARNING "libFuzzer runtime is not linkable with this compiler; building dcc_json_fuzzer as an ASan/UBSan corpus runner")
        target_compile_definitions(dcc_json_fuzzer PRIVATE DCC_FUZZ_STANDALONE=1)
        dcc_target_enable_sanitizers(dcc_json_fuzzer)
    endif()
endif()

set(DCC_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/dcc")
set(DCC_CONFIG_HAVE_OPUS 0)
set(DCC_PC_REQUIRES_PRIVATE "")
set(DCC_PC_LIBS_PRIVATE "")
foreach(DCC_OPENSSL_LIBRARY IN ITEMS "${OPENSSL_SSL_LIBRARY}" "${OPENSSL_CRYPTO_LIBRARY}")
    if(DCC_OPENSSL_LIBRARY)
        string(APPEND DCC_PC_LIBS_PRIVATE " ${DCC_OPENSSL_LIBRARY}")
    endif()
endforeach()
if(UNIX AND NOT APPLE)
    string(APPEND DCC_PC_LIBS_PRIVATE " -pthread")
endif()
if(CMAKE_DL_LIBS)
    string(APPEND DCC_PC_LIBS_PRIVATE " -l${CMAKE_DL_LIBS}")
endif()
if(TARGET dcc_opus)
    set(DCC_CONFIG_HAVE_OPUS 1)
    string(APPEND DCC_PC_LIBS_PRIVATE " ${DCC_OPUS_LIBRARY}")
endif()
if(DCC_LLAM_USE_SUBDIRECTORY AND DCC_BUNDLE_LLAM)
    set(DCC_PC_REQUIRES_PRIVATE "llam")
elseif(NOT DCC_LLAM_USE_SUBDIRECTORY AND DCC_LLAM_LIBRARY)
    string(APPEND DCC_PC_LIBS_PRIVATE " ${DCC_LLAM_LIBRARY}")
endif()

configure_package_config_file(
    cmake/dcc-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/dcc-config.cmake
    INSTALL_DESTINATION ${DCC_CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/dcc-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

if(DCC_LLAM_USE_SUBDIRECTORY AND DCC_BUNDLE_LLAM)
    set(DCC_BUNDLED_LLAM_VERSION "0")
    if(EXISTS "${DCC_LLAM_ROOT}/CMakeLists.txt")
        file(
            STRINGS "${DCC_LLAM_ROOT}/CMakeLists.txt"
            DCC_BUNDLED_LLAM_MAJOR_LINE
            REGEX "^set\\(LLAM_ABI_VERSION_MAJOR [0-9]+\\)"
        )
        file(
            STRINGS "${DCC_LLAM_ROOT}/CMakeLists.txt"
            DCC_BUNDLED_LLAM_MINOR_LINE
            REGEX "^set\\(LLAM_ABI_VERSION_MINOR [0-9]+\\)"
        )
        if(DCC_BUNDLED_LLAM_MAJOR_LINE AND DCC_BUNDLED_LLAM_MINOR_LINE)
            string(
                REGEX REPLACE "^set\\(LLAM_ABI_VERSION_MAJOR ([0-9]+)\\)$" "\\1"
                DCC_BUNDLED_LLAM_VERSION_MAJOR
                "${DCC_BUNDLED_LLAM_MAJOR_LINE}"
            )
            string(
                REGEX REPLACE "^set\\(LLAM_ABI_VERSION_MINOR ([0-9]+)\\)$" "\\1"
                DCC_BUNDLED_LLAM_VERSION_MINOR
                "${DCC_BUNDLED_LLAM_MINOR_LINE}"
            )
            set(DCC_BUNDLED_LLAM_VERSION "${DCC_BUNDLED_LLAM_VERSION_MAJOR}.${DCC_BUNDLED_LLAM_VERSION_MINOR}.0")
        endif()
    endif()
    set(DCC_BUNDLED_LLAM_PC_PRIVATE_LIBS "-pthread")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(DCC_BUNDLED_LLAM_PC_PRIVATE_LIBS "-pthread -luring -lm")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
        set(DCC_BUNDLED_LLAM_PC_PRIVATE_LIBS "-pthread -lrt")
    endif()

    configure_package_config_file(
        cmake/llam-bundled-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/dcc-bundled-llam-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/llam
    )
    configure_file(
        cmake/llam-bundled.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/dcc-bundled-llam.pc
        @ONLY
    )
endif()

configure_file(
    cmake/dcc.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/dcc.pc
    @ONLY
)

install(
    TARGETS dcc
    EXPORT dcc-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(DCC_LLAM_USE_SUBDIRECTORY AND DCC_BUNDLE_LLAM)
    install(
        FILES "$<TARGET_FILE:llam_runtime>"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(
        DIRECTORY "${DCC_LLAM_ROOT}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    if(EXISTS "${DCC_LLAM_ROOT}/README.md")
        install(
            FILES "${DCC_LLAM_ROOT}/README.md"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/llam
            RENAME README.md
        )
    endif()
    if(EXISTS "${DCC_LLAM_ROOT}/LICENSE")
        install(
            FILES "${DCC_LLAM_ROOT}/LICENSE"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/llam
            RENAME LICENSE
        )
    endif()
    if(EXISTS "${DCC_LLAM_ROOT}/CHANGELOG.md")
        install(
            FILES "${DCC_LLAM_ROOT}/CHANGELOG.md"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/llam
            RENAME CHANGELOG.md
        )
    endif()
    if(EXISTS "${DCC_LLAM_ROOT}/docs")
        install(
            DIRECTORY "${DCC_LLAM_ROOT}/docs/"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/llam/docs
            FILES_MATCHING
                PATTERN "*.md"
        )
    endif()

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/_deps/llam/llam-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/llam
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/dcc-bundled-llam.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        RENAME llam.pc
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/dcc-bundled-llam-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/llam
        RENAME llam-config.cmake
    )
endif()

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES
        LICENSE
        README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dcc
)

install(
    DIRECTORY examples/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dcc/examples
    FILES_MATCHING
        PATTERN "build" EXCLUDE
        PATTERN "*.c"
        PATTERN "*.h"
        PATTERN "*.sh"
        PATTERN "Makefile"
        PATTERN "README.md"
)

install(
    DIRECTORY docs/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dcc/docs
    FILES_MATCHING
        PATTERN "*.md"
        PATTERN "*.png"
        PATTERN "*.css"
)

install(
    DIRECTORY deploy/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dcc/deploy
    FILES_MATCHING
        PATTERN "*.md"
        PATTERN "*.env"
        PATTERN "*.service"
        PATTERN "*.conf"
        PATTERN "*.yaml"
        PATTERN "Caddyfile"
        PATTERN "Containerfile"
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/dcc.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/dcc-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/dcc-config-version.cmake
    DESTINATION ${DCC_CONFIG_INSTALL_DIR}
)

install(
    EXPORT dcc-targets
    NAMESPACE dcc::
    DESTINATION ${DCC_CONFIG_INSTALL_DIR}
)

set(CPACK_PACKAGE_NAME dcc)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "DCC")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_IGNORE_FILES
    "/build[^/]*/"
    "/site/"
    "/target/"
    "/dist/"
    "/\\\\.venv/"
    "/compile_commands\\\\.json$"
    "/__pycache__/"
    "\\\\.pyc$"
    "/\\\\.DS_Store$"
    "/\\\\.env($|\\\\.)"
    "/\\\\.git/"
)
include(CPack)
