/**
 * @file include/dcc/client.h
 * @brief Public DCC client declarations.
 */

#ifndef DCC_CLIENT_H
#define DCC_CLIENT_H

#include <dcc/error.h>
#include <dcc/events.h>
#include <dcc/export.h>
#include <dcc/intents.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#define DCC_GATEWAY_INFO_HAS_SESSION_STATE 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Receives log messages emitted by a client.
 *
 * @param level Severity of the message.
 * @param message Null-terminated message text owned by DCC for the duration of the call.
 * @param user_data Opaque pointer from dcc_client_options_t::log_user_data.
 */
typedef void (*dcc_log_fn)(dcc_log_level_t level, const char *message, void *user_data);

/**
 * @brief Creation options for a DCC client.
 *
 * Set size to `sizeof(dcc_client_options_t)`. DCC uses the field to keep the
 * structure ABI-extensible; fields beyond size are treated as omitted.
 */
typedef struct dcc_client_options {
    size_t size;                        /**< Size of this structure in bytes. */
    const char *token;                  /**< Discord bot token; copied during creation. */
    dcc_intents_t intents;              /**< Gateway intents used when identifying. */
    uint32_t shard_id;                  /**< Shard ID for single-client gateway use. */
    uint32_t shard_count;               /**< Total shard count; zero defaults to one. */
    uint32_t rest_concurrency;          /**< Maximum concurrent REST requests; zero uses the default. */
    uint32_t enable_cache;              /**< Non-zero enables the gateway object cache. */
    dcc_log_fn log_fn;                  /**< Optional log callback. */
    void *log_user_data;                /**< User data passed to log_fn. */
    const char *gateway_url;            /**< Optional gateway URL override; copied during creation. */
    size_t cache_max_guilds;            /**< Maximum cached guild count, or zero for no explicit cap. */
    size_t cache_max_channels;          /**< Maximum cached channel count, or zero for no explicit cap. */
    size_t cache_max_roles;             /**< Maximum cached role count, or zero for no explicit cap. */
    size_t cache_max_members;           /**< Maximum cached member count, or zero for no explicit cap. */
    size_t cache_max_users;             /**< Maximum cached user count, or zero for no explicit cap. */
    size_t cache_max_messages;          /**< Maximum cached message count, or zero for no explicit cap. */
    size_t cache_max_voice_states;      /**< Maximum cached voice-state count, or zero for no explicit cap. */
    uint32_t gateway_max_concurrency;   /**< Gateway identify concurrency; zero uses the default. */
    uint32_t infer_guild_id_from_channel; /**< Non-zero allows channel cache lookups to infer guild IDs. */
} dcc_client_options_t;

/**
 * @brief Runtime counters collected by a client.
 *
 * Callers must set size to `sizeof(dcc_runtime_stats_t)` before passing the
 * structure to dcc_client_runtime_stats().
 */
typedef struct dcc_runtime_stats {
    size_t size;                 /**< Size of this structure in bytes. */
    uint64_t tasks_spawned;      /**< Number of runtime tasks spawned by the client. */
    uint64_t events_dispatched;  /**< Number of events dispatched through the event bus. */
} dcc_runtime_stats_t;

/**
 * @brief Snapshot of gateway connection and session state.
 *
 * Callers must set size to `sizeof(dcc_gateway_info_t)` before passing the
 * structure to dcc_client_gateway_info(). String pointers are owned by the
 * client and remain valid only while the client is alive and unchanged.
 */
typedef struct dcc_gateway_info {
    size_t size;                    /**< Size of this structure in bytes. */
    const char *url;                /**< Gateway URL currently used by the client. */
    const char *resume_url;         /**< Resume URL supplied by Discord, when available. */
    uint32_t recommended_shards;    /**< Shard count recommended by Discord. */
    uint32_t max_concurrency;       /**< Gateway identify concurrency currently known to the client. */
    uint32_t remaining_identifies;  /**< Remaining identifies in the current session-start window. */
    uint32_t total_identifies;      /**< Total identifies allowed in the current session-start window. */
    uint64_t reset_after_ms;        /**< Milliseconds until the session-start window resets. */
    uint64_t reset_at_ms;           /**< Runtime timestamp for the next session-start reset. */
    const char *session_id;         /**< Current gateway session ID, when known. */
    uint64_t seq;                   /**< Last gateway sequence number. */
    uint32_t identify_delay_ms;     /**< Delay currently applied before identifying. */
    dcc_status_t last_status;       /**< Last status reported by the gateway task. */
    uint8_t has_seq;                /**< Non-zero when seq contains a valid gateway sequence. */
    uint8_t can_resume;             /**< Non-zero when the client has enough state to resume. */
    uint8_t task_running;           /**< Non-zero while the gateway runtime task is running. */
    uint8_t reconnect_requested;    /**< Non-zero when a reconnect has been requested. */
    uint8_t reconnect_resume;       /**< Non-zero when the pending reconnect should attempt resume. */
} dcc_gateway_info_t;

/**
 * @brief Options for Gateway Request Guild Members.
 *
 * Set size to `sizeof(dcc_gateway_request_guild_members_options_t)`. When
 * user_ids is empty, DCC sends query and limit. When user_ids is non-empty,
 * DCC sends the user ID array form accepted by Discord.
 */
typedef struct dcc_gateway_request_guild_members_options {
    size_t size;                        /**< Size of this structure in bytes. */
    dcc_snowflake_t guild_id;           /**< Guild to request members from. */
    const char *query;                  /**< Optional member search query. NULL defaults to an empty query. */
    uint32_t limit;                     /**< Maximum matching members for query requests. */
    uint8_t presences;                  /**< Non-zero requests presence data when permitted by intents. */
    const dcc_snowflake_t *user_ids;    /**< Optional user IDs to request directly. */
    size_t user_id_count;               /**< Number of entries in user_ids. */
    const char *nonce;                  /**< Optional nonce, up to 32 bytes per Discord's Gateway contract. */
} dcc_gateway_request_guild_members_options_t;

/**
 * @brief Options for Gateway Update Presence.
 *
 * activities_json is a raw JSON array. Passing NULL sends an empty array.
 */
typedef struct dcc_gateway_presence_update {
    size_t size;              /**< Size of this structure in bytes. */
    const char *status;       /**< Presence status: online, idle, dnd, invisible, or offline. */
    int64_t since_ms;         /**< Unix time in milliseconds for idle state. */
    uint8_t has_since;        /**< Non-zero includes since_ms; zero sends JSON null. */
    uint8_t afk;              /**< Whether the bot is AFK. */
    const char *activities_json; /**< Raw JSON activity array, or NULL for []. */
} dcc_gateway_presence_update_t;

/**
 * @brief Creates a client from the supplied options.
 *
 * @param options Client options. size must cover at least the token field.
 * @param out Receives the created client on success.
 * @return DCC_OK on success, otherwise a status code describing the failure.
 */
DCC_API dcc_status_t dcc_client_create(const dcc_client_options_t *options, dcc_client_t **out);

/**
 * @brief Stops and releases a client.
 *
 * Passing `NULL` is allowed and has no effect.
 */
DCC_API void dcc_client_destroy(dcc_client_t *client);

/**
 * @brief Starts the client's runtime services.
 */
DCC_API dcc_status_t dcc_client_start(dcc_client_t *client);

/**
 * @brief Requests client shutdown.
 */
DCC_API dcc_status_t dcc_client_stop(dcc_client_t *client);

/**
 * @brief Blocks until the client runtime finishes.
 */
DCC_API dcc_status_t dcc_client_wait(dcc_client_t *client);

/**
 * @brief Starts the gateway loop for an already-created client.
 */
DCC_API dcc_status_t dcc_client_gateway_start(dcc_client_t *client);

/**
 * @brief Stops the gateway loop without destroying the client.
 */
DCC_API dcc_status_t dcc_client_gateway_stop(dcc_client_t *client);

/**
 * @brief Requests a gateway reconnect.
 *
 * @param resume Non-zero to attempt gateway session resume when possible.
 */
DCC_API dcc_status_t dcc_client_gateway_reconnect(dcc_client_t *client, uint8_t resume);

/**
 * @brief Sends a gateway voice-state update for the current user.
 */
DCC_API dcc_status_t dcc_client_update_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf
);

/**
 * @brief Sends Gateway Request Guild Members (opcode 8).
 */
DCC_API dcc_status_t dcc_client_request_guild_members(
    dcc_client_t *client,
    const dcc_gateway_request_guild_members_options_t *options
);

/**
 * @brief Sends Gateway Request Soundboard Sounds (opcode 31).
 */
DCC_API dcc_status_t dcc_client_request_soundboard_sounds(
    dcc_client_t *client,
    const dcc_snowflake_t *guild_ids,
    size_t guild_id_count
);

/**
 * @brief Sends Gateway Request Channel Info (opcode 43).
 */
DCC_API dcc_status_t dcc_client_request_channel_info(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char * const *fields,
    size_t field_count
);

/**
 * @brief Sends Gateway Update Presence (opcode 3).
 */
DCC_API dcc_status_t dcc_client_update_presence(
    dcc_client_t *client,
    const dcc_gateway_presence_update_t *presence
);

/**
 * @brief Sends Gateway Update Presence (opcode 3) with a raw presence object.
 *
 * presence_json must be the inner `d` object, not the full Gateway payload.
 */
DCC_API dcc_status_t dcc_client_update_presence_raw(
    dcc_client_t *client,
    const char *presence_json
);

/**
 * @brief Looks up the guild that owns a cached channel.
 */
DCC_API dcc_status_t dcc_client_infer_guild_id_from_channel(
    const dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t *out_guild_id
);

/**
 * @brief Copies runtime counters into an output snapshot.
 *
 * The caller must initialize out->size before calling.
 */
DCC_API dcc_status_t dcc_client_runtime_stats(dcc_client_t *client, dcc_runtime_stats_t *out);

/**
 * @brief Copies gateway state into an output snapshot.
 *
 * The caller must initialize out->size before calling.
 */
DCC_API dcc_status_t dcc_client_gateway_info(const dcc_client_t *client, dcc_gateway_info_t *out);

/**
 * @brief Returns the cached DM channel ID for a user, or zero when unknown.
 */
DCC_API dcc_snowflake_t dcc_client_get_dm_channel(const dcc_client_t *client, dcc_snowflake_t user_id);

/**
 * @brief Stores or updates a user-to-DM-channel mapping in the client cache.
 */
DCC_API dcc_status_t dcc_client_set_dm_channel(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id
);

/**
 * @brief Returns the client's last diagnostic error message.
 *
 * The returned pointer is owned by the client. A null client returns an empty
 * string.
 */
DCC_API const char *dcc_client_last_error(const dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
