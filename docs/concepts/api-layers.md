# Choose an API Layer

| Goal | Header | Use |
| --- | --- | --- |
| Normal bot | `<dcc/bot.h>` | handlers, listeners, replies, UI, runner |
| Advanced App lifecycle | focused `<dcc/app/*.h>` | explicit create/register/run/destroy |
| Raw client/Gateway | `<dcc/client.h>`, `<dcc/events.h>` | custom runtime integration |
| Discord REST endpoint | focused `<dcc/rest/*.h>` | owned request handles and results |
| Full framework aggregate | `<dcc/dcc.h>` | all canonical subsystems |

Bot helpers are allocation-free construction functions over the canonical App,
ordered interaction queue, tagged component validator, and request/result REST
engine. Moving down a layer does not create a second execution model.

REST admission returns an optional owned `dcc_rest_request_t`. Use
`dcc_rest_request_cancel`, `dcc_rest_request_wait`, and
`dcc_rest_request_destroy`; retain a result with `dcc_rest_result_clone` only
when it must outlive its callback or request handle.
