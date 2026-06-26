/**
 * @file include/dcc/webhook_events.h
 * @brief Public Discord outgoing Webhook Events declarations.
 */

#ifndef DCC_WEBHOOK_EVENTS_H
#define DCC_WEBHOOK_EVENTS_H

#include <dcc/interaction_server.h>
#include <dcc/rest/base.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Discord outgoing Webhook Event type.
 */
typedef enum dcc_webhook_event_type {
    DCC_WEBHOOK_EVENT_UNKNOWN = 0,              /**< Event type was not recognized. */
    DCC_WEBHOOK_EVENT_PING,                     /**< Webhook endpoint verification ping. */
    DCC_WEBHOOK_EVENT_APPLICATION_AUTHORIZED,   /**< APPLICATION_AUTHORIZED. */
    DCC_WEBHOOK_EVENT_APPLICATION_DEAUTHORIZED, /**< APPLICATION_DEAUTHORIZED. */
    DCC_WEBHOOK_EVENT_ENTITLEMENT_CREATE,       /**< ENTITLEMENT_CREATE. */
    DCC_WEBHOOK_EVENT_ENTITLEMENT_UPDATE,       /**< ENTITLEMENT_UPDATE. */
    DCC_WEBHOOK_EVENT_ENTITLEMENT_DELETE,       /**< ENTITLEMENT_DELETE. */
    DCC_WEBHOOK_EVENT_QUEST_USER_ENROLLMENT,    /**< QUEST_USER_ENROLLMENT. */
    DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_CREATE,     /**< LOBBY_MESSAGE_CREATE. */
    DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_UPDATE,     /**< LOBBY_MESSAGE_UPDATE. */
    DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_DELETE,     /**< LOBBY_MESSAGE_DELETE. */
    DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_CREATE, /**< GAME_DIRECT_MESSAGE_CREATE. */
    DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_UPDATE, /**< GAME_DIRECT_MESSAGE_UPDATE. */
    DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_DELETE  /**< GAME_DIRECT_MESSAGE_DELETE. */
} dcc_webhook_event_type_t;

/**
 * @brief Parsed Webhook Event envelope.
 */
typedef struct dcc_webhook_event {
    size_t size;                         /**< Size of this structure in bytes. */
    dcc_webhook_event_type_t type;       /**< Parsed event type. */
    const char *type_name;               /**< Event type string, or NULL for PING/unknown. */
    const char *timestamp;               /**< Borrowed ISO8601 event timestamp span, when present. */
    size_t timestamp_len;                /**< Number of bytes in timestamp. */
    dcc_snowflake_t application_id;      /**< Application ID, when present. */
    const char *event_json;              /**< Borrowed inner `event` object JSON, when present. */
    size_t event_json_len;               /**< Number of bytes in event_json. */
    const char *data_json;               /**< Borrowed inner event `data` object JSON, when present. */
    size_t data_json_len;                /**< Number of bytes in data_json. */
    const char *raw_body;                /**< Raw request body. */
    size_t raw_body_len;                 /**< Number of bytes in raw_body. */
} dcc_webhook_event_t;

/**
 * @brief Callback invoked by a Webhook Events interaction-server route.
 */
typedef dcc_status_t (*dcc_webhook_event_cb)(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    const dcc_webhook_event_t *event,
    void *user_data
);

/**
 * @brief Caller-owned route state for a Webhook Events route.
 *
 * The structure must stay alive until the route is cleared or the interaction
 * server is destroyed.
 */
typedef struct dcc_webhook_event_route {
    size_t size;                   /**< Size of this structure in bytes. */
    uint32_t flags;                /**< Additional dcc_interaction_route_flag_t values. */
    dcc_webhook_event_cb callback; /**< Event callback. */
    void *user_data;               /**< Opaque callback data. */
} dcc_webhook_event_route_t;

/**
 * @brief Returns the Discord string name for a Webhook Event type.
 */
DCC_API const char *dcc_webhook_event_type_name(dcc_webhook_event_type_t type);

/**
 * @brief Parses a Discord Webhook Event type string.
 */
DCC_API dcc_webhook_event_type_t dcc_webhook_event_type_from_string(const char *name);

/**
 * @brief Parses the raw outer Webhook Event payload envelope.
 */
DCC_API dcc_status_t dcc_webhook_event_parse(
    const char *body,
    size_t body_len,
    dcc_webhook_event_t *out
);

/**
 * @brief Adds a signed Webhook Events route to an interaction server.
 *
 * PING payloads are acknowledged automatically with HTTP 204. Event payloads
 * are parsed and routed to route->callback, then acknowledged with HTTP 204.
 */
DCC_API dcc_status_t dcc_webhook_events_add_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_webhook_event_route_t *route
);

#ifdef __cplusplus
}
#endif

#endif
