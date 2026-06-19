#ifndef DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TIME_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TIME_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TIME_FIELDS \
    char gateway_timestamp_text[64]; \
    char gateway_join_timestamp[64]; \
    char gateway_created_at[64]; \
    char gateway_expires_at[64]; \
    char gateway_start_time[64]; \
    char gateway_end_time[64]; \
    char gateway_current_period_start[64]; \
    char gateway_current_period_end[64]; \
    char gateway_canceled_at[64]; \
    char gateway_joined_at[64]; \
    char gateway_premium_since[64]; \
    char gateway_communication_disabled_until[64]

#endif
