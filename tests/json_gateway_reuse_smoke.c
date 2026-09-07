#include "internal/json/dcc_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "reuse: %s line %d\n", #x, __LINE__); return 1; } } while (0)

/* Catch omitted full resets, stale string tails, stale optional fields and
 * stale truncation markers when one payload storage serves unrelated events. */
int main(void) {
    dcc_json_gateway_payload_t *p = malloc(sizeof(*p));
    CHECK(p != NULL);
    const char *large = "{\"op\":0,\"s\":7,\"t\":\"MESSAGE_CREATE\",\"d\":{\"id\":\"11\",\"channel_id\":\"22\",\"content\":\"a much longer previous message\",\"author\":{\"id\":\"33\",\"username\":\"old-user\"}}}";
    const char *small = "{\"op\":0,\"t\":\"MESSAGE_CREATE\",\"d\":{\"id\":\"44\",\"content\":\"x\"}}";
    CHECK(dcc_json_parse_gateway_payload(large, strlen(large), p) == DCC_OK);
    CHECK(p->has_message && p->message.id == 11U);
    CHECK(dcc_json_parse_gateway_payload(small, strlen(small), p) == DCC_OK);
    CHECK(p->has_message && p->message.id == 44U && p->message.channel_id == 0U);
    CHECK(strcmp(p->message.content, "x") == 0 && p->message_content[2] == '\0');
    CHECK(!p->has_seq);
    const char *bad = "{\"op\":0,\"t\":\"READY\",\"d\":{\"session_id\":\"stale\",";
    CHECK(dcc_json_parse_gateway_payload(bad, strlen(bad), p) == DCC_ERR_JSON);
    const char *hello = "{\"op\":10,\"d\":{\"heartbeat_interval\":45000}}";
    CHECK(dcc_json_parse_gateway_payload(hello, strlen(hello), p) == DCC_OK);
    CHECK(p->has_heartbeat_interval && p->heartbeat_interval == 45000);
    CHECK(!p->has_message && !p->has_session_id && !p->has_event_name);
    CHECK(p->message.content == NULL && p->session_id[0] == '\0');
    size_t capacity = sizeof(p->ready_guild_id_storage) / sizeof(p->ready_guild_id_storage[0]);
    char *json = malloc(capacity * 24U + 128U);
    CHECK(json != NULL);
    size_t used = (size_t)sprintf(json, "{\"op\":0,\"t\":\"READY\",\"d\":{\"guilds\":[");
    for (size_t i = 0U; i <= capacity; ++i)
        used += (size_t)sprintf(json + used, "%s{\"id\":\"1\"}", i ? "," : "");
    used += (size_t)sprintf(json + used, "]}}");
    CHECK(dcc_json_parse_gateway_payload(json, used, p) == DCC_OK);
    CHECK(p->ready_guild_ids_truncated && p->ready_guild_count == capacity + 1U);
    CHECK(dcc_json_parse_gateway_payload(hello, strlen(hello), p) == DCC_OK);
    CHECK(!p->ready_guild_ids_truncated && p->ready_guild_ids_count == 0U);
    CHECK(p->ready_guild_ids == NULL && p->ready_guild_count == 0U);
    free(json);
    free(p);
    return 0;
}
