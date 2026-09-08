# Choose an API Layer

| Goal | Header | Use |
| --- | --- | --- |
| Normal bot | `<dcc/bot.h>` | handlers, listeners, replies, UI, runner |
| Advanced App lifecycle | focused `<dcc/app/*.h>` | explicit create/register/run/destroy |
| Raw client/Gateway | `<dcc/client.h>`, `<dcc/events.h>` | custom runtime integration |
| Discord REST endpoint | focused `<dcc/rest/*.h>` | owned request handles and results |
| Full framework aggregate | `<dcc/dcc.h>` | all canonical subsystems |

Builder construction can be allocation-free. Reply helpers such as
`DCC_CTX_REPLY_TEXT` submit operations that allocate queue/request storage;
the Bot API as a whole is not allocation-free. All layers share the canonical
App, ordered interaction queue, component validator, and request/result REST
engine. Moving down a layer does not create a second execution model.

REST admission returns an optional owned `dcc_rest_request_t`. Use
`dcc_rest_request_cancel`, `dcc_rest_request_wait`, and
`dcc_rest_request_destroy`; retain a result with `dcc_rest_result_clone` only
when it must outlive its callback or request handle.

## Advanced operation control (2.1)

The Context and Flow `_ex` variants of reply, defer, edit-original, and followup
accept `dcc_rest_call_options_t` and an optional request-handle output. They
preserve ordered interaction admission while exposing cancellation, waiting,
and completion. A successful return means admission, not HTTP success. These
operations support default authentication and no audit reason; unsupported
options fail validation. A null output selects automatic handle release.

See the [ownership and async contract](ownership-and-async.md),
[Context declarations](../reference/api/app/context.md), and
[Flow declarations](../reference/api/interaction_flow.md).
