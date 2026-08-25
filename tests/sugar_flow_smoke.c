#include <dcc/sugar/flow.h>
#include <dcc/client.h>

#include <stdio.h>
#include <string.h>

static int expect(const char *name, int ok) {
  if (!ok) {
    fprintf(stderr, "flow sugar check failed: %s\n", name);
    return 0;
  }
  return 1;
}

int main(void) {
  dcc_client_options_t client_options = {
      .size = sizeof(client_options), .token = "", .intents = 0U};
  dcc_client_t *client = NULL;
  if (dcc_client_create(&client_options, &client) != DCC_OK)
    return 1;
  dcc_interaction_t interaction_value = {
      .id = 1U, .application_id = 2U, .token = "sugar-flow"};
  const dcc_interaction_t *interaction = &interaction_value;
  dcc_interaction_flow_t *flow = DCC_FLOW_STARTED(client, interaction, 1000U);
  interaction_value.id = 2U;
  interaction_value.token = "sugar-auto";
  dcc_interaction_flow_t *auto_flow =
      DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED(client, interaction, 2000U, 1500U);
  interaction_value.id = 3U;
  interaction_value.token = "sugar-private";
  dcc_interaction_flow_t *private_auto_flow =
      DCC_FLOW_AUTO_DEFER_PRIVATE_STARTED(client, interaction, 3000U, 2000U);
  dcc_modal_builder_t modal = {.size = sizeof(dcc_modal_builder_t),
                               .version = DCC_MODAL_BUILDER_VERSION,
                               .present = DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID |
                                          DCC_MODAL_BUILDER_PRESENT_TITLE,
                               .custom_id = "flow.modal",
                               .title = "Flow modal"};

  if (!expect("constructor", flow != NULL) ||
      !expect("constructor state",
              DCC_FLOW_STATE(flow) == DCC_INTERACTION_FLOW_READY) ||
      !expect("auto defer", auto_flow != NULL) ||
      !expect("private auto defer", private_auto_flow != NULL) ||
      !expect("state null",
              DCC_FLOW_STATE(NULL) == DCC_INTERACTION_FLOW_FAILED) ||
      !expect("state name null",
              strcmp(DCC_FLOW_STATE_NAME(NULL), "failed") == 0) ||
      !expect("message text",
              strcmp(DCC_FLOW_MESSAGE_TEXT("pong").content, "pong") == 0) ||
      !expect("message text flag",
              (DCC_FLOW_MESSAGE_TEXT("pong").present &
               DCC_MESSAGE_BUILDER_PRESENT_CONTENT) != 0U) ||
      !expect("defer invalid", DCC_FLOW_DEFER(NULL) == DCC_ERR_INVALID_ARG) ||
      !expect("defer cb invalid",
              DCC_FLOW_DEFER_CB(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG) ||
      !expect("defer ephemeral invalid",
              DCC_FLOW_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG) ||
      !expect("defer private invalid",
              DCC_FLOW_DEFER_PRIVATE(NULL) == DCC_ERR_INVALID_ARG) ||
      !expect("defer update invalid",
              DCC_FLOW_DEFER_UPDATE(NULL) == DCC_ERR_INVALID_ARG) ||
      !expect("maybe auto defer invalid",
              DCC_FLOW_MAYBE_AUTO_DEFER(NULL, 3000U) == DCC_ERR_INVALID_ARG) ||
      !expect("reply invalid",
              DCC_FLOW_REPLY(NULL, DCC_FLOW_MESSAGE_TEXT("pong")) ==
                  DCC_ERR_INVALID_ARG) ||
      !expect("reply text invalid",
              DCC_FLOW_REPLY_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
      !expect("reply private invalid",
              DCC_FLOW_REPLY_PRIVATE(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
      !expect("reply silent invalid",
              DCC_FLOW_REPLY_SILENT(NULL, "pong") == DCC_ERR_INVALID_ARG) ||
      !expect("edit invalid",
              DCC_FLOW_EDIT_TEXT(NULL, "edited") == DCC_ERR_INVALID_ARG) ||
      !expect("followup invalid",
              DCC_FLOW_FOLLOWUP_TEXT(NULL, "next") == DCC_ERR_INVALID_ARG) ||
      !expect("followup private invalid",
              DCC_FLOW_FOLLOWUP_PRIVATE(NULL, "next") == DCC_ERR_INVALID_ARG) ||
      !expect("followup silent invalid",
              DCC_FLOW_FOLLOWUP_SILENT(NULL, "next") == DCC_ERR_INVALID_ARG) ||
      !expect("modal invalid",
              DCC_FLOW_SHOW_MODAL(NULL, modal) == DCC_ERR_INVALID_ARG)) {
    return 1;
  }

  DCC_FLOW_DESTROY(private_auto_flow);
  DCC_FLOW_DESTROY(auto_flow);
  DCC_FLOW_DESTROY(flow);
  dcc_client_destroy(client);

  return 0;
}
