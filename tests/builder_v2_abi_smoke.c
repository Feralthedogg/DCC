#include <dcc/app/context.h>
#include <dcc/application_command.h>
#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/command_registry.h>
#include <dcc/embed.h>
#include <dcc/interaction_flow.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/rest.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

#define CHECK(condition, message)                                              \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, "builder v2 ABI smoke: %s\n", (message));                \
      return 1;                                                                \
    }                                                                          \
  } while (0)

typedef struct builder_v2_prefix {
  size_t size;
  uint32_t version;
  uint64_t present;
} builder_v2_prefix_t;

typedef struct builder_v2_runner {
  dcc_client_t *client;
  dcc_status_t status;
#if defined(_WIN32)
  HANDLE thread;
#else
  pthread_t thread;
#endif
} builder_v2_runner_t;

#if defined(_WIN32)
static DWORD WINAPI builder_v2_runner_main(LPVOID user_data) {
#else
static void *builder_v2_runner_main(void *user_data) {
#endif
  builder_v2_runner_t *runner = (builder_v2_runner_t *)user_data;
  runner->status = dcc_client_wait(runner->client);
#if defined(_WIN32)
  return 0U;
#else
  return NULL;
#endif
}

static int builder_v2_runner_start(dcc_client_t *client,
                                   builder_v2_runner_t *runner) {
  memset(runner, 0, sizeof(*runner));
  runner->client = client;
  runner->status = DCC_ERR_STATE;
  if (dcc_client_start(client) != DCC_OK) {
    return 0;
  }
#if defined(_WIN32)
  runner->thread =
      CreateThread(NULL, 0U, builder_v2_runner_main, runner, 0U, NULL);
  return runner->thread != NULL;
#else
  return pthread_create(&runner->thread, NULL, builder_v2_runner_main,
                        runner) == 0;
#endif
}

static void builder_v2_runner_stop(builder_v2_runner_t *runner) {
  (void)dcc_client_stop(runner->client);
#if defined(_WIN32)
  (void)WaitForSingleObject(runner->thread, INFINITE);
  (void)CloseHandle(runner->thread);
#else
  (void)pthread_join(runner->thread, NULL);
#endif
}

#define CHECK_PREFIX(type_, first_field_)                                      \
  _Static_assert(offsetof(type_, size) == offsetof(builder_v2_prefix_t, size), \
                 #type_ " size offset");                                       \
  _Static_assert(offsetof(type_, version) ==                                   \
                     offsetof(builder_v2_prefix_t, version),                   \
                 #type_ " version offset");                                    \
  _Static_assert(offsetof(type_, present) ==                                   \
                     offsetof(builder_v2_prefix_t, present),                   \
                 #type_ " present offset");                                    \
  _Static_assert(offsetof(type_, first_field_) == sizeof(builder_v2_prefix_t), \
                 #type_ " exact common prefix")

CHECK_PREFIX(dcc_message_builder_t, content);
CHECK_PREFIX(dcc_poll_builder_t, question);
CHECK_PREFIX(dcc_allowed_mentions_builder_t, users);
CHECK_PREFIX(dcc_embed_builder_t, title);
CHECK_PREFIX(dcc_modal_builder_t, custom_id);
CHECK_PREFIX(dcc_autocomplete_choice_t, name);
CHECK_PREFIX(dcc_autocomplete_builder_t, choices);
CHECK_PREFIX(dcc_application_command_option_builder_t, name);
CHECK_PREFIX(dcc_application_command_builder_t, name);

#define ALIGN_UP_TO(value_, alignment_)                                        \
  ((((value_) + (alignment_) - 1U) / (alignment_)) * (alignment_))
#define FIELD_END(type_, field_)                                               \
  (offsetof(type_, field_) + sizeof(((type_ *)0)->field_))

#define CHOICE_HISTORICAL_STRIDE                                               \
  ALIGN_UP_TO(offsetof(dcc_autocomplete_choice_t, name_localizations_json),    \
              _Alignof(dcc_autocomplete_choice_t))
#define OPTION_HISTORICAL_STRIDE                                               \
  ALIGN_UP_TO(offsetof(dcc_application_command_option_builder_t,               \
                       name_localizations_json),                               \
              _Alignof(dcc_application_command_option_builder_t))
#define COMMAND_HISTORICAL_STRIDE                                              \
  offsetof(dcc_application_command_builder_t, default_member_permissions)
#define EMBED_HISTORICAL_STRIDE                                                \
  ALIGN_UP_TO(offsetof(dcc_embed_builder_t, url), _Alignof(dcc_embed_builder_t))

_Static_assert(CHOICE_HISTORICAL_STRIDE % _Alignof(dcc_autocomplete_choice_t) ==
                   0U,
               "choice historical stride alignment");
_Static_assert(CHOICE_HISTORICAL_STRIDE < sizeof(dcc_autocomplete_choice_t),
               "choice historical stride must be smaller");
_Static_assert(CHOICE_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_autocomplete_choice_t, name) &&
                   CHOICE_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_autocomplete_choice_t, value_string) &&
                   CHOICE_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_autocomplete_choice_t, value_integer) &&
                   CHOICE_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_autocomplete_choice_t, value_number) &&
                   CHOICE_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_autocomplete_choice_t, value_type),
               "choice historical stride must cover value group");
_Static_assert(OPTION_HISTORICAL_STRIDE %
                       _Alignof(dcc_application_command_option_builder_t) ==
                   0U,
               "option historical stride alignment");
_Static_assert(OPTION_HISTORICAL_STRIDE <
                   sizeof(dcc_application_command_option_builder_t),
               "option historical stride must be smaller");
_Static_assert(
    OPTION_HISTORICAL_STRIDE >=
            FIELD_END(dcc_application_command_option_builder_t, name) &&
        OPTION_HISTORICAL_STRIDE >=
            FIELD_END(dcc_application_command_option_builder_t, description) &&
        OPTION_HISTORICAL_STRIDE >=
            FIELD_END(dcc_application_command_option_builder_t, type),
    "option historical stride must cover required type");
_Static_assert(COMMAND_HISTORICAL_STRIDE %
                       _Alignof(dcc_application_command_builder_t) ==
                   0U,
               "command historical stride alignment");
_Static_assert(COMMAND_HISTORICAL_STRIDE <
                   sizeof(dcc_application_command_builder_t),
               "command historical stride must be smaller");
_Static_assert(COMMAND_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_application_command_builder_t, name) &&
                   COMMAND_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_application_command_builder_t,
                                 description) &&
                   COMMAND_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_application_command_builder_t, type),
               "command historical stride must cover required fields");
_Static_assert(EMBED_HISTORICAL_STRIDE % _Alignof(dcc_embed_builder_t) == 0U,
               "embed historical stride alignment");
_Static_assert(EMBED_HISTORICAL_STRIDE < sizeof(dcc_embed_builder_t),
               "embed historical stride must be smaller");
_Static_assert(EMBED_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_embed_builder_t, title) &&
                   EMBED_HISTORICAL_STRIDE >=
                       FIELD_END(dcc_embed_builder_t, description),
               "embed historical stride must cover serialized fields");

static int json_has(const char *json, const char *fragment) {
  return json != NULL && fragment != NULL && strstr(json, fragment) != NULL;
}

static int bytes_are(const unsigned char *bytes, size_t count,
                     unsigned char value) {
  for (size_t i = 0U; i < count; ++i) {
    if (bytes[i] != value) {
      return 0;
    }
  }
  return 1;
}

static int check_initializer_contracts(void) {
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  dcc_poll_builder_t poll = DCC_POLL_BUILDER_INIT;
  dcc_allowed_mentions_builder_t mentions = DCC_ALLOWED_MENTIONS_BUILDER_INIT;
  dcc_embed_builder_t embed = DCC_EMBED_BUILDER_INIT;
  dcc_modal_builder_t modal = DCC_MODAL_BUILDER_INIT;
  dcc_autocomplete_choice_t choice = DCC_AUTOCOMPLETE_CHOICE_INIT;
  dcc_autocomplete_choice_t named_choice =
      DCC_AUTOCOMPLETE_CHOICE_NAMED_INIT("name");
  dcc_autocomplete_builder_t autocomplete = DCC_AUTOCOMPLETE_BUILDER_INIT;
  dcc_application_command_option_builder_t option =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_INIT;
  dcc_application_command_builder_t command =
      DCC_APPLICATION_COMMAND_BUILDER_INIT;

#define CHECK_INITIALIZED(value_, version_, label_)                            \
  CHECK((value_).size == sizeof(value_) && (value_).version == (version_) &&   \
            (value_).present == 0U,                                            \
        (label_))
  CHECK_INITIALIZED(message, DCC_MESSAGE_BUILDER_VERSION,
                    "message constant initializer");
  CHECK_INITIALIZED(poll, DCC_POLL_BUILDER_VERSION,
                    "poll constant initializer");
  CHECK_INITIALIZED(mentions, DCC_ALLOWED_MENTIONS_BUILDER_VERSION,
                    "allowed mentions constant initializer");
  CHECK_INITIALIZED(embed, DCC_EMBED_BUILDER_VERSION,
                    "embed constant initializer");
  CHECK_INITIALIZED(modal, DCC_MODAL_BUILDER_VERSION,
                    "modal constant initializer");
  CHECK_INITIALIZED(choice, DCC_AUTOCOMPLETE_CHOICE_VERSION,
                    "autocomplete choice constant initializer");
  CHECK(named_choice.size == sizeof(named_choice) &&
            named_choice.version == DCC_AUTOCOMPLETE_CHOICE_VERSION &&
            named_choice.present == DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME &&
            strcmp(named_choice.name, "name") == 0,
        "autocomplete named choice initializer");
  CHECK_INITIALIZED(autocomplete, DCC_AUTOCOMPLETE_BUILDER_VERSION,
                    "autocomplete builder constant initializer");
  CHECK_INITIALIZED(option, DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION,
                    "command option constant initializer");
  CHECK_INITIALIZED(command, DCC_APPLICATION_COMMAND_BUILDER_VERSION,
                    "command constant initializer");
#undef CHECK_INITIALIZED

  CHECK(message.content == NULL && message.nonce == NULL &&
            message.message_reference_json == NULL &&
            message.allowed_mentions_json == NULL &&
            message.embeds_json == NULL && message.components_json == NULL &&
            message.attachments_json == NULL && message.poll_json == NULL &&
            message.embeds == NULL && message.embeds_count == 0U &&
            message.components == NULL && message.components_count == 0U &&
            message.components_v2 == NULL &&
            message.components_v2_count == 0U &&
            message.components_v2_json == NULL && message.poll == NULL &&
            message.allowed_mentions == NULL && message.sticker_ids == NULL &&
            message.sticker_ids_count == 0U && message.flags == 0U &&
            message.tts == 0U,
        "message initializer clears every public field");
  CHECK(poll.question.text == NULL && poll.question.emoji.name == NULL &&
            poll.question.emoji.id == 0U &&
            poll.question.emoji.animated == 0U &&
            poll.question.emoji.has_id == 0U &&
            poll.question.emoji.has_animated == 0U &&
            poll.question.has_emoji == 0U && poll.answers == NULL &&
            poll.answer_count == 0U && poll.duration_hours == 0U &&
            poll.layout_type == 0 && poll.allow_multiselect == 0U,
        "poll initializer clears every public field");
  CHECK(mentions.users == NULL && mentions.user_count == 0U &&
            mentions.roles == NULL && mentions.role_count == 0U &&
            mentions.parse_users == 0U && mentions.parse_roles == 0U &&
            mentions.parse_everyone == 0U && mentions.replied_user == 0U,
        "mentions initializer clears every public field");
  CHECK(embed.title == NULL && embed.type == NULL &&
            embed.description == NULL && embed.url == NULL &&
            embed.timestamp == NULL && embed.footer.text == NULL &&
            embed.footer.icon_url == NULL && embed.image.url == NULL &&
            embed.thumbnail.url == NULL && embed.author.name == NULL &&
            embed.author.url == NULL && embed.author.icon_url == NULL &&
            embed.fields == NULL && embed.field_count == 0U &&
            embed.color == 0U,
        "embed initializer clears every public field");
  CHECK(modal.custom_id == NULL && modal.title == NULL &&
            modal.components_json == NULL && modal.components == NULL &&
            modal.components_count == 0U && modal.components_v2 == NULL &&
            modal.components_v2_count == 0U && modal.components_v2_json == NULL,
        "modal initializer clears every public field");
  CHECK(choice.name == NULL && choice.value_string == NULL &&
            choice.value_integer == 0 && choice.value_number == 0.0 &&
            choice.value_type == 0 && choice.abi_padding == 0U &&
            choice.name_localizations_json == NULL,
        "choice initializer clears every public field");
  CHECK(named_choice.value_string == NULL && named_choice.value_integer == 0 &&
            named_choice.value_number == 0.0 && named_choice.value_type == 0 &&
            named_choice.abi_padding == 0U &&
            named_choice.name_localizations_json == NULL,
        "named choice initializer clears every optional public field");
  CHECK(autocomplete.choices == NULL && autocomplete.choices_count == 0U,
        "autocomplete initializer clears every public field");
  CHECK(option.name == NULL && option.description == NULL &&
            option.type == 0U && option.name_localizations_json == NULL &&
            option.description_localizations_json == NULL &&
            option.choices_json == NULL && option.choices == NULL &&
            option.choices_count == 0U && option.options_json == NULL &&
            option.options == NULL && option.options_count == 0U &&
            option.channel_types == NULL && option.channel_types_count == 0U &&
            option.min_integer_value == 0 && option.max_integer_value == 0 &&
            option.min_number_value == 0.0 && option.max_number_value == 0.0 &&
            option.required == 0U && option.autocomplete == 0U,
        "command option initializer clears every public field");
  CHECK(command.name == NULL && command.name_localizations_json == NULL &&
            command.description == NULL &&
            command.description_localizations_json == NULL &&
            command.options_json == NULL && command.options == NULL &&
            command.options_count == 0U &&
            command.integration_types_json == NULL &&
            command.contexts_json == NULL && command.type == 0U &&
            command.default_member_permissions == 0U &&
            command.dm_permission == 0U && command.nsfw == 0U &&
            command.default_member_permissions_null == 0U,
        "command initializer clears every public field");

  memset(&message, 0xA5, sizeof(message));
  memset(&poll, 0xA5, sizeof(poll));
  memset(&mentions, 0xA5, sizeof(mentions));
  memset(&embed, 0xA5, sizeof(embed));
  memset(&modal, 0xA5, sizeof(modal));
  memset(&choice, 0xA5, sizeof(choice));
  memset(&autocomplete, 0xA5, sizeof(autocomplete));
  memset(&option, 0xA5, sizeof(option));
  memset(&command, 0xA5, sizeof(command));
  dcc_message_builder_init(&message);
  dcc_poll_builder_init(&poll);
  dcc_allowed_mentions_builder_init(&mentions);
  dcc_embed_builder_init(&embed);
  dcc_modal_builder_init(&modal);
  dcc_autocomplete_choice_init(&choice, "name");
  dcc_autocomplete_builder_init(&autocomplete);
  dcc_application_command_option_builder_init(&option);
  dcc_application_command_builder_init(&command);
  CHECK(message.size == sizeof(message) && message.present == 0U,
        "message init function");
  CHECK(poll.size == sizeof(poll) && poll.present == 0U, "poll init function");
  CHECK(mentions.size == sizeof(mentions) && mentions.present == 0U,
        "mentions init function");
  CHECK(embed.size == sizeof(embed) && embed.present == 0U,
        "embed init function");
  CHECK(modal.size == sizeof(modal) && modal.present == 0U,
        "modal init function");
  CHECK(choice.size == sizeof(choice) &&
            choice.present == DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME,
        "choice init function");
  CHECK(autocomplete.size == sizeof(autocomplete) && autocomplete.present == 0U,
        "autocomplete init function");
  CHECK(option.size == sizeof(option) && option.present == 0U,
        "option init function");
  CHECK(command.size == sizeof(command) && command.present == 0U,
        "command init function");

  dcc_message_builder_init(NULL);
  dcc_poll_builder_init(NULL);
  dcc_allowed_mentions_builder_init(NULL);
  dcc_embed_builder_init(NULL);
  dcc_modal_builder_init(NULL);
  dcc_autocomplete_choice_init(NULL, "ignored");
  dcc_autocomplete_builder_init(NULL);
  dcc_application_command_option_builder_init(NULL);
  dcc_application_command_builder_init(NULL);
  return 0;
}

#define EXERCISE_ABI_MATRIX(type_, value_, validate_, version_, first_bit_,    \
                            label_)                                            \
  do {                                                                         \
    dcc_builder_abi_view_t view_;                                              \
    type_ clean_ = (value_);                                                   \
    CHECK((validate_)(&clean_, &view_) == DCC_OK &&                            \
              view_.size == sizeof(type_) && view_.version == (version_) &&    \
              view_.present == 0U,                                             \
          label_ " exact current record");                                     \
    (value_) = clean_;                                                         \
    (value_).size = 0U;                                                        \
    CHECK((validate_)(&(value_), &view_) == DCC_ERR_INVALID_ARG,               \
          label_ " zero size");                                                \
    (value_) = clean_;                                                         \
    (value_).size = DCC_BUILDER_ABI_MINIMUM(type_) - 1U;                       \
    CHECK((validate_)(&(value_), &view_) == DCC_ERR_INVALID_ARG,               \
          label_ " truncated prefix");                                         \
    (value_) = clean_;                                                         \
    (value_).version = (version_) + 1U;                                        \
    CHECK((validate_)(&(value_), &view_) == DCC_ERR_INVALID_ARG,               \
          label_ " unsupported version");                                      \
    (value_) = clean_;                                                         \
    (value_).present = UINT64_C(1) << 63U;                                     \
    CHECK((validate_)(&(value_), &view_) == DCC_ERR_INVALID_ARG,               \
          label_ " unknown bit");                                              \
    memset(&(value_), 0xA5, sizeof(value_));                                   \
    (value_).size = DCC_BUILDER_ABI_MINIMUM(type_);                            \
    (value_).version = (version_);                                             \
    (value_).present = 0U;                                                     \
    type_ history_snapshot_ = (value_);                                        \
    CHECK((validate_)(&(value_), &view_) == DCC_OK &&                          \
              memcmp(&(value_), &history_snapshot_, sizeof(value_)) == 0,      \
          label_ " historical poisoned prefix");                               \
    (value_).present = (first_bit_);                                           \
    CHECK((validate_)(&(value_), &view_) == DCC_ERR_INVALID_ARG,               \
          label_ " uncovered present bit");                                    \
    struct {                                                                   \
      type_ value;                                                             \
      unsigned char tail[32];                                                  \
    } larger_;                                                                 \
    memset(&larger_, 0x5A, sizeof(larger_));                                   \
    larger_.value = clean_;                                                    \
    larger_.value.size = sizeof(larger_);                                      \
    larger_.value.present = 0U;                                                \
    CHECK((validate_)(&larger_.value, &view_) == DCC_OK &&                     \
              bytes_are(larger_.tail, sizeof(larger_.tail), 0x5A),             \
          label_ " larger record");                                            \
    (value_) = clean_;                                                         \
  } while (0)

static int check_all_type_abi_matrix(void) {
  dcc_message_builder_t message;
  dcc_poll_builder_t poll;
  dcc_allowed_mentions_builder_t mentions;
  dcc_embed_builder_t embed;
  dcc_modal_builder_t modal;
  dcc_autocomplete_choice_t choice;
  dcc_autocomplete_builder_t autocomplete;
  dcc_application_command_option_builder_t option;
  dcc_application_command_builder_t command;
  dcc_message_builder_init(&message);
  dcc_poll_builder_init(&poll);
  dcc_allowed_mentions_builder_init(&mentions);
  dcc_embed_builder_init(&embed);
  dcc_modal_builder_init(&modal);
  dcc_autocomplete_choice_init(&choice, NULL);
  dcc_autocomplete_builder_init(&autocomplete);
  dcc_application_command_option_builder_init(&option);
  dcc_application_command_builder_init(&command);

  EXERCISE_ABI_MATRIX(dcc_message_builder_t, message,
                      dcc_message_builder_abi_validate,
                      DCC_MESSAGE_BUILDER_VERSION,
                      DCC_MESSAGE_BUILDER_PRESENT_CONTENT, "message");
  EXERCISE_ABI_MATRIX(dcc_poll_builder_t, poll, dcc_poll_builder_abi_validate,
                      DCC_POLL_BUILDER_VERSION,
                      DCC_POLL_BUILDER_PRESENT_QUESTION, "poll");
  EXERCISE_ABI_MATRIX(dcc_allowed_mentions_builder_t, mentions,
                      dcc_allowed_mentions_builder_abi_validate,
                      DCC_ALLOWED_MENTIONS_BUILDER_VERSION,
                      DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS,
                      "allowed mentions");
  EXERCISE_ABI_MATRIX(dcc_embed_builder_t, embed,
                      dcc_embed_builder_abi_validate, DCC_EMBED_BUILDER_VERSION,
                      DCC_EMBED_BUILDER_PRESENT_TITLE, "embed");
  EXERCISE_ABI_MATRIX(dcc_modal_builder_t, modal,
                      dcc_modal_builder_abi_validate, DCC_MODAL_BUILDER_VERSION,
                      DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID, "modal");
  EXERCISE_ABI_MATRIX(
      dcc_autocomplete_choice_t, choice, dcc_autocomplete_choice_abi_validate,
      DCC_AUTOCOMPLETE_CHOICE_VERSION, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME,
      "autocomplete choice");
  EXERCISE_ABI_MATRIX(
      dcc_autocomplete_builder_t, autocomplete,
      dcc_autocomplete_builder_abi_validate, DCC_AUTOCOMPLETE_BUILDER_VERSION,
      DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES, "autocomplete builder");
  EXERCISE_ABI_MATRIX(dcc_application_command_option_builder_t, option,
                      dcc_application_command_option_builder_abi_validate,
                      DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION,
                      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME,
                      "command option");
  EXERCISE_ABI_MATRIX(dcc_application_command_builder_t, command,
                      dcc_application_command_builder_abi_validate,
                      DCC_APPLICATION_COMMAND_BUILDER_VERSION,
                      DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME, "command");
  return 0;
}

#undef EXERCISE_ABI_MATRIX

static int check_historical_poison_serialization(void) {
  char *json = NULL;

  dcc_message_builder_t message;
  memset(&message, 0xA5, sizeof(message));
  message.size = offsetof(dcc_message_builder_t, nonce);
  message.version = DCC_MESSAGE_BUILDER_VERSION;
  message.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
  message.content = "historical-message";
  CHECK(dcc_message_builder_build_json(&message, &json) == DCC_OK &&
            json_has(json, "\"content\":\"historical-message\"") &&
            !json_has(json, "nonce"),
        "historical message ignores poisoned suffix");
  dcc_message_builder_json_free(json);

  dcc_poll_answer_t answer = {.media = {.text = "yes"}};
  dcc_poll_builder_t poll;
  memset(&poll, 0xA5, sizeof(poll));
  poll.size = offsetof(dcc_poll_builder_t, duration_hours);
  poll.version = DCC_POLL_BUILDER_VERSION;
  poll.present =
      DCC_POLL_BUILDER_PRESENT_QUESTION | DCC_POLL_BUILDER_PRESENT_ANSWERS;
  poll.question = (dcc_poll_media_t){.text = "ready?"};
  poll.answers = &answer;
  poll.answer_count = 1U;
  json = NULL;
  CHECK(dcc_message_poll_builder_build_json(&poll, &json) == DCC_OK &&
            json_has(json, "\"text\":\"ready?\"") &&
            json_has(json, "\"duration\":24"),
        "historical poll defaults poisoned suffix");
  dcc_message_poll_builder_json_free(json);

  dcc_snowflake_t user = 42U;
  dcc_allowed_mentions_builder_t mentions;
  memset(&mentions, 0xA5, sizeof(mentions));
  mentions.size = offsetof(dcc_allowed_mentions_builder_t, roles);
  mentions.version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION;
  mentions.present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS;
  mentions.users = &user;
  mentions.user_count = 1U;
  dcc_message_builder_init(&message);
  CHECK(dcc_message_builder_set_allowed_mentions(&message, &mentions) == DCC_OK,
        "set historical mentions");
  json = NULL;
  CHECK(dcc_message_builder_build_json(&message, &json) == DCC_OK &&
            json_has(json, "\"users\":[\"42\"]") &&
            !json_has(json, "replied_user"),
        "historical mentions ignore poisoned suffix");
  dcc_message_builder_json_free(json);

  dcc_embed_builder_t embed;
  memset(&embed, 0xA5, sizeof(embed));
  embed.size = offsetof(dcc_embed_builder_t, type);
  embed.version = DCC_EMBED_BUILDER_VERSION;
  embed.present = DCC_EMBED_BUILDER_PRESENT_TITLE;
  embed.title = "historical-embed";
  json = NULL;
  CHECK(dcc_embed_builder_build_json(&embed, &json) == DCC_OK &&
            json_has(json, "\"title\":\"historical-embed\"") &&
            !json_has(json, "description"),
        "historical embed ignores poisoned suffix");
  dcc_embed_builder_json_free(json);

  dcc_modal_builder_t modal;
  memset(&modal, 0xA5, sizeof(modal));
  modal.size = offsetof(dcc_modal_builder_t, components);
  modal.version = DCC_MODAL_BUILDER_VERSION;
  modal.present = DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID |
                  DCC_MODAL_BUILDER_PRESENT_TITLE |
                  DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON;
  modal.custom_id = "historical.modal";
  modal.title = "Historical";
  modal.components_json = "[]";
  json = NULL;
  CHECK(dcc_modal_builder_build_json(&modal, &json) == DCC_OK &&
            json_has(json, "\"custom_id\":\"historical.modal\"") &&
            json_has(json, "\"components\":[]"),
        "historical modal ignores poisoned suffix");
  dcc_modal_builder_json_free(json);

  dcc_autocomplete_choice_t choice;
  memset(&choice, 0xA5, sizeof(choice));
  choice.size = CHOICE_HISTORICAL_STRIDE;
  choice.version = DCC_AUTOCOMPLETE_CHOICE_VERSION;
  choice.present = DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME |
                   DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  choice.name = "historical-choice";
  choice.value_string = "history";
  choice.value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  CHECK(dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U) ==
            DCC_OK,
        "set historical choice");
  json = NULL;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) == DCC_OK &&
            json_has(json, "\"name\":\"historical-choice\"") &&
            !json_has(json, "name_localizations"),
        "historical choice ignores poisoned suffix");
  dcc_autocomplete_builder_json_free(json);

  memset(&autocomplete, 0xA5, sizeof(autocomplete));
  autocomplete.size = DCC_BUILDER_ABI_MINIMUM(dcc_autocomplete_builder_t);
  autocomplete.version = DCC_AUTOCOMPLETE_BUILDER_VERSION;
  autocomplete.present = 0U;
  json = NULL;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) == DCC_OK &&
            json_has(json, "\"choices\":[]"),
        "historical autocomplete builder ignores poisoned suffix");
  dcc_autocomplete_builder_json_free(json);

  dcc_application_command_option_builder_t option;
  memset(&option, 0xA5, sizeof(option));
  option.size = OPTION_HISTORICAL_STRIDE;
  option.version = DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION;
  option.present = DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
                   DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
                   DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE;
  option.name = "historical-option";
  option.description = "Historical option";
  option.type = DCC_APPLICATION_COMMAND_OPTION_STRING;
  json = NULL;
  CHECK(dcc_application_command_option_builder_build_array_json(
            &option, 1U, &json) == DCC_OK &&
            json_has(json, "\"name\":\"historical-option\"") &&
            !json_has(json, "required"),
        "historical option ignores poisoned suffix");
  dcc_application_command_option_builder_json_free(json);

  dcc_application_command_builder_t command;
  memset(&command, 0xA5, sizeof(command));
  command.size = COMMAND_HISTORICAL_STRIDE;
  command.version = DCC_APPLICATION_COMMAND_BUILDER_VERSION;
  command.present = DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME |
                    DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION |
                    DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
  command.name = "historical-command";
  command.description = "Historical command";
  command.type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
  json = NULL;
  CHECK(dcc_application_command_builder_build_json(&command, &json) == DCC_OK &&
            json_has(json, "\"name\":\"historical-command\"") &&
            !json_has(json, "dm_permission"),
        "historical command ignores poisoned suffix");
  dcc_application_command_builder_json_free(json);
  return 0;
}

static int check_presence_and_zero_semantics(void) {
  char *json = NULL;
  dcc_message_builder_t message;
  dcc_message_builder_init(&message);
  message.nonce = "poison-without-presence";
  CHECK(dcc_message_builder_set_flags(&message, 0U) == DCC_OK &&
            dcc_message_builder_set_tts(&message, 0U) == DCC_OK &&
            dcc_message_builder_build_json(&message, &json) == DCC_OK &&
            json_has(json, "\"flags\":0") && json_has(json, "\"tts\":false") &&
            !json_has(json, "nonce"),
        "message zero values and authoritative presence");
  dcc_message_builder_json_free(json);
  CHECK(dcc_message_builder_set_content(&message, "clear-me") == DCC_OK &&
            dcc_message_builder_set_content(&message, NULL) == DCC_OK &&
            (message.present & DCC_MESSAGE_BUILDER_PRESENT_CONTENT) == 0U,
        "message null clears presence");

  dcc_poll_answer_t answer = {.media = {.text = "no"}};
  dcc_poll_builder_t poll;
  dcc_poll_builder_init(&poll);
  poll.present = DCC_POLL_BUILDER_PRESENT_QUESTION |
                 DCC_POLL_BUILDER_PRESENT_ANSWERS |
                 DCC_POLL_BUILDER_PRESENT_ALLOW_MULTISELECT;
  poll.question.text = "continue?";
  poll.answers = &answer;
  poll.answer_count = 1U;
  poll.allow_multiselect = 0U;
  json = NULL;
  CHECK(dcc_message_poll_builder_build_json(&poll, &json) == DCC_OK &&
            json_has(json, "\"allow_multiselect\":false"),
        "poll present false value");
  dcc_message_poll_builder_json_free(json);

  dcc_allowed_mentions_builder_t mentions;
  dcc_allowed_mentions_builder_init(&mentions);
  mentions.present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER;
  mentions.replied_user = 0U;
  dcc_message_builder_init(&message);
  CHECK(dcc_message_builder_set_allowed_mentions(&message, &mentions) == DCC_OK,
        "set zero mentions");
  json = NULL;
  CHECK(dcc_message_builder_build_json(&message, &json) == DCC_OK &&
            json_has(json, "\"replied_user\":false"),
        "mentions present false value");
  dcc_message_builder_json_free(json);

  dcc_embed_builder_t embed;
  dcc_embed_builder_init(&embed);
  CHECK(dcc_embed_builder_set_color(&embed, 0U) == DCC_OK &&
            dcc_embed_builder_build_json(&embed, &json) == DCC_OK &&
            json_has(json, "\"color\":0"),
        "embed present zero color");
  dcc_embed_builder_json_free(json);

  dcc_autocomplete_choice_t choice;
  dcc_autocomplete_choice_init(&choice, "zero");
  CHECK(dcc_autocomplete_choice_set_integer_value(&choice, 0) == DCC_OK,
        "choice zero setter");
  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  CHECK(dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U) ==
                DCC_OK &&
            dcc_autocomplete_builder_build_json(&autocomplete, &json) ==
                DCC_OK &&
            json_has(json, "\"value\":0"),
        "choice present zero value");
  dcc_autocomplete_builder_json_free(json);
  CHECK(dcc_autocomplete_builder_set_choices(&autocomplete, NULL, 0U) ==
                DCC_OK &&
            (autocomplete.present & DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES) ==
                0U,
        "autocomplete empty clears presence");

  dcc_application_command_option_builder_t option;
  dcc_application_command_option_builder_init(&option);
  option.present =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE;
  option.name = "count";
  option.description = "Count";
  option.type = DCC_APPLICATION_COMMAND_OPTION_INTEGER;
  option.required = 0U;
  option.min_integer_value = 0;
  json = NULL;
  CHECK(dcc_application_command_option_builder_build_array_json(
            &option, 1U, &json) == DCC_OK &&
            json_has(json, "\"required\":false") &&
            json_has(json, "\"min_value\":0"),
        "option present zero values");
  dcc_application_command_option_builder_json_free(json);

  dcc_application_command_builder_t command;
  dcc_application_command_builder_init(&command);
  CHECK(dcc_application_command_builder_set_default_member_permissions(
            &command, 0U) == DCC_OK &&
            dcc_application_command_builder_set_dm_permission(&command, 0U) ==
                DCC_OK &&
            dcc_application_command_builder_set_nsfw(&command, 0U) == DCC_OK &&
            dcc_application_command_builder_build_json(&command, &json) ==
                DCC_OK &&
            json_has(json, "\"default_member_permissions\":\"0\"") &&
            json_has(json, "\"dm_permission\":false") &&
            json_has(json, "\"nsfw\":false"),
        "command present zero values");
  dcc_application_command_builder_json_free(json);
  return 0;
}

static int check_all_current_setters(void) {
  dcc_snowflake_t sticker = 7U;
  dcc_poll_answer_t answer = {.media = {.text = "yes"}};
  dcc_poll_builder_t poll;
  dcc_poll_builder_init(&poll);
  poll.present =
      DCC_POLL_BUILDER_PRESENT_QUESTION | DCC_POLL_BUILDER_PRESENT_ANSWERS;
  poll.question.text = "question";
  poll.answers = &answer;
  poll.answer_count = 1U;
  dcc_allowed_mentions_builder_t mentions;
  dcc_allowed_mentions_builder_init(&mentions);
  dcc_embed_builder_t message_embed;
  dcc_embed_builder_init(&message_embed);
  dcc_component_builder_t component;
  dcc_component_builder_init(&component, DCC_COMPONENT_BUTTON);
  dcc_component_v2_builder_t component_v2;
  dcc_component_v2_builder_init(&component_v2, DCC_COMPONENT_V2_TEXT_DISPLAY);

  dcc_message_builder_t message;
  dcc_message_builder_init(&message);
  CHECK(
      dcc_message_builder_set_content(&message, "content") == DCC_OK &&
          dcc_message_builder_set_nonce(&message, "nonce") == DCC_OK &&
          dcc_message_builder_set_tts(&message, 0U) == DCC_OK &&
          dcc_message_builder_set_flags(&message, 0U) == DCC_OK &&
          dcc_message_builder_set_sticker_ids(&message, &sticker, 1U) ==
              DCC_OK &&
          dcc_message_builder_set_message_reference_json(
              &message, "{\"message_id\":\"1\"}") == DCC_OK &&
          dcc_message_builder_set_allowed_mentions_json(&message, "{}") ==
              DCC_OK &&
          dcc_message_builder_set_allowed_mentions(&message, &mentions) ==
              DCC_OK &&
          dcc_message_builder_set_embeds(&message, &message_embed, 1U) ==
              DCC_OK &&
          dcc_message_builder_set_components(&message, &component, 1U) ==
              DCC_OK &&
          dcc_message_builder_set_components_v2(&message, &component_v2, 1U) ==
              DCC_OK &&
          dcc_message_builder_set_embeds_json(&message, "[]") == DCC_OK &&
          dcc_message_builder_set_components_json(&message, "[]") == DCC_OK &&
          dcc_message_builder_set_components_v2_json(&message, "[]") ==
              DCC_OK &&
          dcc_message_builder_set_attachments_json(&message, "[]") == DCC_OK &&
          dcc_message_builder_set_poll(&message, &poll) == DCC_OK &&
          dcc_message_builder_set_poll_json(&message, "{}") == DCC_OK,
      "every current message setter accepts a valid builder");
  const uint64_t message_pointer_bits =
      DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_NONCE |
      DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS |
      DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS |
      DCC_MESSAGE_BUILDER_PRESENT_EMBEDS |
      DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS |
      DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2 |
      DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON |
      DCC_MESSAGE_BUILDER_PRESENT_POLL | DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON;
  CHECK((message.present & message_pointer_bits) == message_pointer_bits &&
            (message.present & DCC_MESSAGE_BUILDER_PRESENT_TTS) != 0U &&
            (message.present & DCC_MESSAGE_BUILDER_PRESENT_FLAGS) != 0U,
        "message setters mark all presence bits");
  CHECK(
      dcc_message_builder_set_content(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_nonce(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_sticker_ids(&message, NULL, 0U) == DCC_OK &&
          dcc_message_builder_set_message_reference_json(&message, NULL) ==
              DCC_OK &&
          dcc_message_builder_set_allowed_mentions_json(&message, NULL) ==
              DCC_OK &&
          dcc_message_builder_set_allowed_mentions(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_embeds(&message, NULL, 0U) == DCC_OK &&
          dcc_message_builder_set_components(&message, NULL, 0U) == DCC_OK &&
          dcc_message_builder_set_components_v2(&message, NULL, 0U) == DCC_OK &&
          dcc_message_builder_set_embeds_json(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_components_json(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_components_v2_json(&message, NULL) ==
              DCC_OK &&
          dcc_message_builder_set_attachments_json(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_poll(&message, NULL) == DCC_OK &&
          dcc_message_builder_set_poll_json(&message, NULL) == DCC_OK &&
          (message.present & message_pointer_bits) == 0U,
      "message null and empty setters clear presence bits");

  dcc_embed_field_t field = {.name = "name", .value = "value"};
  dcc_embed_builder_t embed;
  dcc_embed_builder_init(&embed);
  CHECK(
      dcc_embed_builder_set_title(&embed, "title") == DCC_OK &&
          dcc_embed_builder_set_type(&embed, "rich") == DCC_OK &&
          dcc_embed_builder_set_description(&embed, "description") == DCC_OK &&
          dcc_embed_builder_set_url(&embed, "https://example.test") == DCC_OK &&
          dcc_embed_builder_set_timestamp(&embed, "2026-08-11T00:00:00Z") ==
              DCC_OK &&
          dcc_embed_builder_set_color(&embed, 0U) == DCC_OK &&
          dcc_embed_builder_set_footer(&embed, "footer", "icon") == DCC_OK &&
          dcc_embed_builder_set_image(&embed, "image") == DCC_OK &&
          dcc_embed_builder_set_thumbnail(&embed, "thumbnail") == DCC_OK &&
          dcc_embed_builder_set_author(&embed, "author", "url", "icon") ==
              DCC_OK &&
          dcc_embed_builder_set_fields(&embed, &field, 1U) == DCC_OK,
      "every current embed setter accepts a valid builder");
  const uint64_t embed_bits =
      DCC_EMBED_BUILDER_PRESENT_TITLE | DCC_EMBED_BUILDER_PRESENT_TYPE |
      DCC_EMBED_BUILDER_PRESENT_DESCRIPTION | DCC_EMBED_BUILDER_PRESENT_URL |
      DCC_EMBED_BUILDER_PRESENT_TIMESTAMP | DCC_EMBED_BUILDER_PRESENT_COLOR |
      DCC_EMBED_BUILDER_PRESENT_FOOTER | DCC_EMBED_BUILDER_PRESENT_IMAGE |
      DCC_EMBED_BUILDER_PRESENT_THUMBNAIL | DCC_EMBED_BUILDER_PRESENT_AUTHOR |
      DCC_EMBED_BUILDER_PRESENT_FIELDS;
  CHECK((embed.present & embed_bits) == embed_bits,
        "embed setters mark all presence bits");
  CHECK(dcc_embed_builder_set_title(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_type(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_description(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_url(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_timestamp(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_footer(&embed, NULL, NULL) == DCC_OK &&
            dcc_embed_builder_set_image(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_thumbnail(&embed, NULL) == DCC_OK &&
            dcc_embed_builder_set_author(&embed, NULL, NULL, NULL) == DCC_OK &&
            dcc_embed_builder_set_fields(&embed, NULL, 0U) == DCC_OK &&
            (embed.present & (embed_bits & ~DCC_EMBED_BUILDER_PRESENT_COLOR)) ==
                0U,
        "embed null and empty setters clear presence bits");

  dcc_modal_builder_t modal;
  dcc_modal_builder_init(&modal);
  CHECK(dcc_modal_builder_set_custom_id(&modal, "modal") == DCC_OK &&
            dcc_modal_builder_set_title(&modal, "Modal") == DCC_OK &&
            dcc_modal_builder_set_components(&modal, &component, 1U) ==
                DCC_OK &&
            dcc_modal_builder_set_components_v2(&modal, &component_v2, 1U) ==
                DCC_OK &&
            dcc_modal_builder_set_components_json(&modal, "[]") == DCC_OK &&
            dcc_modal_builder_set_components_v2_json(&modal, "[]") == DCC_OK,
        "every current modal setter accepts a valid builder");
  const uint64_t modal_bits = DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID |
                              DCC_MODAL_BUILDER_PRESENT_TITLE |
                              DCC_MODAL_BUILDER_PRESENT_COMPONENTS |
                              DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2 |
                              DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON |
                              DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON;
  CHECK((modal.present & modal_bits) == modal_bits &&
            dcc_modal_builder_set_custom_id(&modal, NULL) == DCC_OK &&
            dcc_modal_builder_set_title(&modal, NULL) == DCC_OK &&
            dcc_modal_builder_set_components(&modal, NULL, 0U) == DCC_OK &&
            dcc_modal_builder_set_components_v2(&modal, NULL, 0U) == DCC_OK &&
            dcc_modal_builder_set_components_json(&modal, NULL) == DCC_OK &&
            dcc_modal_builder_set_components_v2_json(&modal, NULL) == DCC_OK &&
            (modal.present & modal_bits) == 0U,
        "modal setters mark and clear presence bits");

  dcc_autocomplete_choice_t choice;
  dcc_autocomplete_choice_init(&choice, NULL);
  CHECK(dcc_autocomplete_choice_set_name(&choice, "choice") == DCC_OK &&
            dcc_autocomplete_choice_set_name_localizations_json(
                &choice, "{\"ko\":\"choice\"}") == DCC_OK &&
            dcc_autocomplete_choice_set_string_value(&choice, "value") ==
                DCC_OK &&
            dcc_autocomplete_choice_set_integer_value(&choice, 0) == DCC_OK &&
            dcc_autocomplete_choice_set_number_value(&choice, 0.0) == DCC_OK,
        "every current choice setter accepts a valid builder");
  CHECK((choice.present & DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME) != 0U &&
            (choice.present &
             DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON) != 0U &&
            (choice.present & DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE) != 0U &&
            dcc_autocomplete_choice_set_name(&choice, NULL) == DCC_OK &&
            dcc_autocomplete_choice_set_name_localizations_json(
                &choice, NULL) == DCC_OK &&
            dcc_autocomplete_choice_set_string_value(&choice, NULL) == DCC_OK &&
            choice.present == 0U,
        "choice setters mark and clear presence bits");
  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  dcc_autocomplete_choice_init(&choice, "choice");
  CHECK(dcc_autocomplete_choice_set_string_value(&choice, "value") == DCC_OK &&
            dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U) ==
                DCC_OK &&
            (autocomplete.present & DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES) !=
                0U &&
            dcc_autocomplete_builder_set_choices(&autocomplete, NULL, 0U) ==
                DCC_OK &&
            autocomplete.present == 0U,
        "autocomplete builder setter marks and clears presence");

  dcc_application_command_option_builder_t command_option;
  dcc_application_command_option_builder_init(&command_option);
  dcc_application_command_builder_t command;
  dcc_application_command_builder_init(&command);
  CHECK(
      dcc_application_command_builder_set_name(&command, "command") == DCC_OK &&
          dcc_application_command_builder_set_name_localizations_json(
              &command, "{\"ko\":\"command\"}") == DCC_OK &&
          dcc_application_command_builder_set_description(
              &command, "Description") == DCC_OK &&
          dcc_application_command_builder_set_description_localizations_json(
              &command, "{\"ko\":\"description\"}") == DCC_OK &&
          dcc_application_command_builder_set_type(
              &command, DCC_APPLICATION_COMMAND_CHAT_INPUT) == DCC_OK &&
          dcc_application_command_builder_set_options_json(&command, "[]") ==
              DCC_OK &&
          (command.present &
           DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON) != 0U &&
          dcc_application_command_builder_set_options(&command, &command_option,
                                                      1U) == DCC_OK &&
          (command.present & DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS) !=
              0U &&
          (command.present &
           DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON) == 0U &&
          dcc_application_command_builder_set_default_member_permissions(
              &command, 0U) == DCC_OK &&
          dcc_application_command_builder_set_default_member_permissions_null(
              &command) == DCC_OK &&
          command.default_member_permissions_null == 1U &&
          dcc_application_command_builder_set_dm_permission(&command, 0U) ==
              DCC_OK &&
          dcc_application_command_builder_set_nsfw(&command, 0U) == DCC_OK &&
          dcc_application_command_builder_set_integration_types_json(
              &command, "[0]") == DCC_OK &&
          dcc_application_command_builder_set_contexts_json(&command, "[0]") ==
              DCC_OK,
      "every current command setter accepts a valid builder");
  CHECK(
      dcc_application_command_builder_set_name(&command, NULL) == DCC_OK &&
          dcc_application_command_builder_set_name_localizations_json(
              &command, NULL) == DCC_OK &&
          dcc_application_command_builder_set_description(&command, NULL) ==
              DCC_OK &&
          dcc_application_command_builder_set_description_localizations_json(
              &command, NULL) == DCC_OK &&
          dcc_application_command_builder_set_options(&command, NULL, 0U) ==
              DCC_OK &&
          dcc_application_command_builder_set_options_json(&command, NULL) ==
              DCC_OK &&
          dcc_application_command_builder_set_integration_types_json(
              &command, NULL) == DCC_OK &&
          dcc_application_command_builder_set_contexts_json(&command, NULL) ==
              DCC_OK &&
          (command.present &
           (DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON |
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON)) == 0U,
      "command nullable setters clear presence bits");
  return 0;
}

static int check_setter_failure_no_mutation(void) {
#define SNAPSHOT_AND_REJECT(value_, expression_, label_)                       \
  do {                                                                         \
    unsigned char before_[sizeof(value_)];                                     \
    memcpy(before_, &(value_), sizeof(value_));                                \
    CHECK((expression_) == DCC_ERR_INVALID_ARG &&                              \
              memcmp(before_, &(value_), sizeof(value_)) == 0,                 \
          (label_));                                                           \
  } while (0)

  dcc_message_builder_t message;
  dcc_message_builder_init(&message);
  message.size = DCC_BUILDER_ABI_MINIMUM(dcc_message_builder_t);
  SNAPSHOT_AND_REJECT(message,
                      dcc_message_builder_set_content(&message, "no-write"),
                      "message truncated setter no mutation");
  struct {
    unsigned char before[16];
    dcc_message_builder_t value;
    unsigned char after[16];
  } guarded;
  memset(&guarded, 0x6D, sizeof(guarded));
  dcc_message_builder_init(&guarded.value);
  guarded.value.size = DCC_BUILDER_ABI_MINIMUM(dcc_message_builder_t);
  unsigned char guarded_value_before[sizeof(guarded.value)];
  memcpy(guarded_value_before, &guarded.value, sizeof(guarded.value));
  CHECK(dcc_message_builder_set_content(&guarded.value, "no-write") ==
                DCC_ERR_INVALID_ARG &&
            memcmp(guarded_value_before, &guarded.value,
                   sizeof(guarded.value)) == 0 &&
            bytes_are(guarded.before, sizeof(guarded.before), 0x6D) &&
            bytes_are(guarded.after, sizeof(guarded.after), 0x6D),
        "rejected historical setter preserves external canaries");
  dcc_message_builder_init(&message);
  message.size = offsetof(dcc_message_builder_t, flags);
  SNAPSHOT_AND_REJECT(
      message,
      dcc_message_builder_set_components_v2(
          &message, (const dcc_component_v2_builder_t *)(uintptr_t)1U, 1U),
      "message multi-field setter prevalidates complete write set");

  dcc_embed_builder_t embed;
  dcc_embed_builder_init(&embed);
  embed.size = DCC_BUILDER_ABI_MINIMUM(dcc_embed_builder_t);
  SNAPSHOT_AND_REJECT(embed, dcc_embed_builder_set_title(&embed, "no-write"),
                      "embed truncated setter no mutation");

  dcc_modal_builder_t modal;
  dcc_modal_builder_init(&modal);
  modal.version++;
  SNAPSHOT_AND_REJECT(modal, dcc_modal_builder_set_title(&modal, "no-write"),
                      "modal invalid version setter no mutation");

  dcc_autocomplete_choice_t choice;
  dcc_autocomplete_choice_init(&choice, NULL);
  choice.size = DCC_BUILDER_ABI_MINIMUM(dcc_autocomplete_choice_t);
  SNAPSHOT_AND_REJECT(choice,
                      dcc_autocomplete_choice_set_integer_value(&choice, 7),
                      "choice truncated value setter no mutation");

  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  autocomplete.present = UINT64_C(1) << 63U;
  SNAPSHOT_AND_REJECT(
      autocomplete,
      dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U),
      "autocomplete unknown-bit setter no mutation");

  dcc_application_command_builder_t command;
  dcc_application_command_builder_init(&command);
  command.size = DCC_BUILDER_ABI_MINIMUM(dcc_application_command_builder_t);
  SNAPSHOT_AND_REJECT(
      command, dcc_application_command_builder_set_name(&command, "no-write"),
      "command truncated setter no mutation");
#undef SNAPSHOT_AND_REJECT
  return 0;
}

typedef union aligned_builder_bytes {
  max_align_t alignment;
  unsigned char bytes[1024];
} aligned_builder_bytes_t;

static dcc_autocomplete_choice_t *
choice_record(aligned_builder_bytes_t *storage, size_t stride, size_t index) {
  return (dcc_autocomplete_choice_t *)(storage->bytes + stride * index);
}

static dcc_application_command_option_builder_t *
option_record(aligned_builder_bytes_t *storage, size_t stride, size_t index) {
  return (dcc_application_command_option_builder_t *)(storage->bytes +
                                                      stride * index);
}

static dcc_application_command_builder_t *
command_record(aligned_builder_bytes_t *storage, size_t stride, size_t index) {
  return (dcc_application_command_builder_t *)(storage->bytes + stride * index);
}

static dcc_embed_builder_t *embed_record(aligned_builder_bytes_t *storage,
                                         size_t stride, size_t index) {
  return (dcc_embed_builder_t *)(storage->bytes + stride * index);
}

static void init_historical_choice(dcc_autocomplete_choice_t *choice,
                                   size_t stride, const char *name,
                                   const char *value) {
  memset(choice, 0, stride);
  choice->size = stride;
  choice->version = DCC_AUTOCOMPLETE_CHOICE_VERSION;
  choice->present = DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME |
                    DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  choice->name = name;
  choice->value_string = value;
  choice->value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  choice->abi_padding = UINT32_C(0xA5A5A5A5);
}

static void
init_historical_option(dcc_application_command_option_builder_t *option,
                       size_t stride, const char *name, const char *description,
                       uint32_t type) {
  memset(option, 0, stride);
  option->size = stride;
  option->version = DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION;
  option->present = DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
                    DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
                    DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE;
  option->name = name;
  option->description = description;
  option->type = type;
}

static int check_versioned_stride_arrays(void) {
  aligned_builder_bytes_t choices_storage;
  memset(&choices_storage, 0xCC, sizeof(choices_storage));
  dcc_autocomplete_choice_t *choice0 =
      choice_record(&choices_storage, CHOICE_HISTORICAL_STRIDE, 0U);
  dcc_autocomplete_choice_t *choice1 =
      choice_record(&choices_storage, CHOICE_HISTORICAL_STRIDE, 1U);
  init_historical_choice(choice0, CHOICE_HISTORICAL_STRIDE, "Alpha", "a");
  init_historical_choice(choice1, CHOICE_HISTORICAL_STRIDE, "Beta", "b");
  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  CHECK(dcc_autocomplete_builder_set_choices(&autocomplete, choice0, 2U) ==
            DCC_OK,
        "set two historical choice records");
  char *json = NULL;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) == DCC_OK &&
            json_has(json, "\"name\":\"Alpha\"") &&
            json_has(json, "\"name\":\"Beta\""),
        "two historical choice records use byte stride");
  dcc_autocomplete_builder_json_free(json);

  choice1->size =
      CHOICE_HISTORICAL_STRIDE + _Alignof(dcc_autocomplete_choice_t);
  json = NULL;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) ==
                DCC_ERR_INVALID_ARG &&
            json == NULL,
        "choice second-record stride mismatch");
  choice1->size = CHOICE_HISTORICAL_STRIDE;
  choice1->version++;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) ==
            DCC_ERR_INVALID_ARG,
        "choice second-record version mismatch");
  choice1->version = DCC_AUTOCOMPLETE_CHOICE_VERSION;
  choice1->present |= UINT64_C(1) << 63U;
  CHECK(dcc_autocomplete_builder_build_json(&autocomplete, &json) ==
            DCC_ERR_INVALID_ARG,
        "choice second-record unknown bit");
  choice1->present &= ~(UINT64_C(1) << 63U);

  aligned_builder_bytes_t options_storage;
  memset(&options_storage, 0xCC, sizeof(options_storage));
  dcc_application_command_option_builder_t *option0 =
      option_record(&options_storage, OPTION_HISTORICAL_STRIDE, 0U);
  dcc_application_command_option_builder_t *option1 =
      option_record(&options_storage, OPTION_HISTORICAL_STRIDE, 1U);
  init_historical_option(option0, OPTION_HISTORICAL_STRIDE, "first",
                         "First option", DCC_APPLICATION_COMMAND_OPTION_STRING);
  init_historical_option(option1, OPTION_HISTORICAL_STRIDE, "second",
                         "Second option",
                         DCC_APPLICATION_COMMAND_OPTION_INTEGER);
  json = NULL;
  CHECK(dcc_application_command_option_builder_build_array_json(
            option0, 2U, &json) == DCC_OK &&
            json_has(json, "\"name\":\"first\"") &&
            json_has(json, "\"name\":\"second\""),
        "two historical option records use byte stride");
  dcc_application_command_option_builder_json_free(json);

  dcc_application_command_builder_t nested_command;
  dcc_application_command_builder_init(&nested_command);
  CHECK(dcc_application_command_builder_set_name(&nested_command, "nested") ==
                DCC_OK &&
            dcc_application_command_builder_set_description(
                &nested_command, "Nested stride") == DCC_OK &&
            dcc_application_command_builder_set_type(
                &nested_command, DCC_APPLICATION_COMMAND_CHAT_INPUT) ==
                DCC_OK &&
            dcc_application_command_builder_set_options(&nested_command,
                                                        option0, 2U) == DCC_OK,
        "build command with historical recursive options");
  json = NULL;
  CHECK(dcc_application_command_builder_build_json(&nested_command, &json) ==
                DCC_OK &&
            json_has(json, "\"name\":\"first\"") &&
            json_has(json, "\"name\":\"second\""),
        "recursive option array uses declared byte stride");
  dcc_application_command_builder_json_free(json);
  option1->version++;
  CHECK(dcc_application_command_builder_build_json(&nested_command, &json) ==
            DCC_ERR_INVALID_ARG,
        "recursive option second-record version mismatch");
  option1->version = DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION;

  aligned_builder_bytes_t commands_storage;
  memset(&commands_storage, 0xCC, sizeof(commands_storage));
  dcc_application_command_builder_t *command0 =
      command_record(&commands_storage, COMMAND_HISTORICAL_STRIDE, 0U);
  dcc_application_command_builder_t *command1 =
      command_record(&commands_storage, COMMAND_HISTORICAL_STRIDE, 1U);
  for (size_t i = 0U; i < 2U; ++i) {
    dcc_application_command_builder_t *command = i == 0U ? command0 : command1;
    memset(command, 0, COMMAND_HISTORICAL_STRIDE);
    command->size = COMMAND_HISTORICAL_STRIDE;
    command->version = DCC_APPLICATION_COMMAND_BUILDER_VERSION;
    command->present = DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME |
                       DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION |
                       DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    command->name = i == 0U ? "one" : "two";
    command->description = i == 0U ? "One" : "Two";
    command->type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
  }
  json = NULL;
  CHECK(dcc_application_command_builder_build_array_json(command0, 2U, &json) ==
                DCC_OK &&
            json_has(json, "\"name\":\"one\"") &&
            json_has(json, "\"name\":\"two\""),
        "two historical command records use byte stride");
  dcc_application_command_builder_json_free(json);
  command1->size = sizeof(dcc_application_command_builder_t);
  CHECK(dcc_application_command_builder_build_array_json(command0, 2U, &json) ==
            DCC_ERR_INVALID_ARG,
        "command second-record stride mismatch");

  aligned_builder_bytes_t embeds_storage;
  memset(&embeds_storage, 0xCC, sizeof(embeds_storage));
  dcc_embed_builder_t *embed0 =
      embed_record(&embeds_storage, EMBED_HISTORICAL_STRIDE, 0U);
  dcc_embed_builder_t *embed1 =
      embed_record(&embeds_storage, EMBED_HISTORICAL_STRIDE, 1U);
  for (size_t i = 0U; i < 2U; ++i) {
    dcc_embed_builder_t *embed = i == 0U ? embed0 : embed1;
    memset(embed, 0, EMBED_HISTORICAL_STRIDE);
    embed->size = EMBED_HISTORICAL_STRIDE;
    embed->version = DCC_EMBED_BUILDER_VERSION;
    embed->present =
        DCC_EMBED_BUILDER_PRESENT_TITLE | DCC_EMBED_BUILDER_PRESENT_DESCRIPTION;
    embed->title = i == 0U ? "left" : "right";
    embed->description = i == 0U ? "Left" : "Right";
  }
  json = NULL;
  CHECK(dcc_embed_builder_build_array_json(embed0, 2U, &json) == DCC_OK &&
            json_has(json, "\"title\":\"left\"") &&
            json_has(json, "\"title\":\"right\""),
        "two historical embed records use byte stride");
  dcc_embed_builder_json_free(json);

  dcc_application_command_builder_t overflow_command;
  dcc_application_command_builder_init(&overflow_command);
  size_t stride = overflow_command.size;
  CHECK(dcc_application_command_builder_build_array_json(
            &overflow_command, SIZE_MAX / stride + 1U, &json) ==
            DCC_ERR_INVALID_ARG,
        "array count multiplication overflow");
  CHECK(dcc_application_command_builder_build_array_json(
            &overflow_command, SIZE_MAX / stride, &json) == DCC_ERR_INVALID_ARG,
        "array address span overflow");
  _Alignas(dcc_application_command_builder_t) unsigned char
      misaligned_storage[sizeof(dcc_application_command_builder_t) +
                         _Alignof(dcc_application_command_builder_t)];
  memset(misaligned_storage, 0, sizeof(misaligned_storage));
  dcc_application_command_builder_t aligned_command;
  dcc_application_command_builder_init(&aligned_command);
  memcpy(misaligned_storage + 1U, &aligned_command, sizeof(aligned_command));
  CHECK(
      dcc_application_command_builder_build_array_json(
          (const dcc_application_command_builder_t *)(misaligned_storage + 1U),
          1U, &json) == DCC_ERR_INVALID_ARG,
      "misaligned versioned array base");
  return 0;
}

static int check_required_field_rejection(void) {
  char *json = NULL;
  dcc_modal_builder_t modal;
  dcc_modal_builder_init(&modal);
  CHECK(dcc_modal_builder_build_json(&modal, &json) == DCC_ERR_INVALID_ARG &&
            json == NULL,
        "modal serializer rejects missing required fields");

  dcc_autocomplete_choice_t choice;
  dcc_autocomplete_choice_init(&choice, "missing-value");
  dcc_autocomplete_builder_t autocomplete;
  dcc_autocomplete_builder_init(&autocomplete);
  CHECK(dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U) ==
                DCC_OK &&
            dcc_autocomplete_builder_build_json(&autocomplete, &json) ==
                DCC_ERR_INVALID_ARG &&
            json == NULL,
        "choice serializer rejects missing required value");

  dcc_application_command_option_builder_t option;
  dcc_application_command_option_builder_init(&option);
  CHECK(dcc_application_command_option_builder_build_array_json(
            &option, 1U, &json) == DCC_ERR_INVALID_ARG &&
            json == NULL,
        "option serializer rejects missing required fields");
  return 0;
}

static void init_filter_output(aligned_builder_bytes_t *storage, size_t stride,
                               size_t count, unsigned char canary) {
  memset(storage->bytes, canary, sizeof(storage->bytes));
  for (size_t i = 0U; i < count; ++i) {
    dcc_autocomplete_choice_t *choice = choice_record(storage, stride, i);
    memset(choice, 0, stride);
    choice->size = stride;
    choice->version = DCC_AUTOCOMPLETE_CHOICE_VERSION;
  }
}

typedef struct matching_intercept_state {
  size_t calls;
  char body[2048];
} matching_intercept_state_t;

static dcc_status_t
matching_intercept(dcc_client_t *client, const char *method, const char *path,
                   const void *body, size_t body_len, const char *content_type,
                   dcc_rest_cb cb, void *user_data, void *intercept_user_data) {
  (void)method;
  (void)path;
  (void)content_type;
  matching_intercept_state_t *state =
      (matching_intercept_state_t *)intercept_user_data;
  if (state != NULL) {
    state->calls++;
    size_t copy_len = body_len < sizeof(state->body) - 1U
                          ? body_len
                          : sizeof(state->body) - 1U;
    if (body != NULL && copy_len != 0U) {
      memcpy(state->body, body, copy_len);
    }
    state->body[copy_len] = '\0';
  }
  if (cb != NULL) {
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 204U,
        .error = DCC_OK,
    };
    cb(client, &response, user_data);
  }
  return DCC_OK;
}

static int check_autocomplete_filter_stride_and_atomicity(void) {
  aligned_builder_bytes_t inputs;
  memset(&inputs, 0xCC, sizeof(inputs));
  dcc_autocomplete_choice_t *input0 =
      choice_record(&inputs, CHOICE_HISTORICAL_STRIDE, 0U);
  dcc_autocomplete_choice_t *input1 =
      choice_record(&inputs, CHOICE_HISTORICAL_STRIDE, 1U);
  init_historical_choice(input0, CHOICE_HISTORICAL_STRIDE, "Alpha", "alpha");
  init_historical_choice(input1, CHOICE_HISTORICAL_STRIDE, "Alpine", "alpine");

  dcc_interaction_option_t focused;
  memset(&focused, 0, sizeof(focused));
  focused.value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
  focused.string_value = "al";
  focused.focused = 1U;
  dcc_interaction_t interaction;
  memset(&interaction, 0, sizeof(interaction));
  interaction.focused_option = &focused;
  dcc_ctx_t ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.interaction = &interaction;

  aligned_builder_bytes_t outputs;
  unsigned char output_snapshot[sizeof(outputs.bytes)];
  size_t out_count = 99U;
  CHECK(dcc_ctx_autocomplete_filter_choices(&ctx, NULL, 0U, NULL, 0U,
                                            &out_count) == DCC_OK &&
            out_count == 0U,
        "valid empty autocomplete input accepts zero output capacity");

#define CHECK_FILTER_REJECTS_UNCHANGED(source_, source_count_, capacity_,      \
                                       label_)                                 \
  do {                                                                         \
    init_filter_output(&outputs, sizeof(dcc_autocomplete_choice_t), 2U, 0xD7); \
    memcpy(output_snapshot, outputs.bytes, sizeof(output_snapshot));           \
    out_count = 99U;                                                           \
    CHECK(dcc_ctx_autocomplete_filter_choices(                                 \
              &ctx, (source_), (source_count_),                                \
              choice_record(&outputs, sizeof(dcc_autocomplete_choice_t), 0U),  \
              (capacity_), &out_count) == DCC_ERR_INVALID_ARG &&               \
              out_count == 0U &&                                               \
              memcmp(output_snapshot, outputs.bytes,                           \
                     sizeof(output_snapshot)) == 0,                            \
          (label_));                                                           \
  } while (0)

  dcc_autocomplete_choice_t invalid_choice =
      DCC_AUTOCOMPLETE_CHOICE_NAMED_INIT("Future");
  invalid_choice.present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  invalid_choice.value_string = "future";
  invalid_choice.value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  invalid_choice.version++;
  CHECK_FILTER_REJECTS_UNCHANGED(
      &invalid_choice, 1U, 0U,
      "zero-capacity filter validates future-version source without mutation");

  dcc_autocomplete_choice_t malformed_later[2] = {
      DCC_AUTOCOMPLETE_CHOICE_NAMED_INIT("First"),
      DCC_AUTOCOMPLETE_CHOICE_NAMED_INIT("Later"),
  };
  malformed_later[0].present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  malformed_later[0].value_string = "first";
  malformed_later[0].value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  malformed_later[1].present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  malformed_later[1].value_string = "later";
  malformed_later[1].value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  malformed_later[1].version++;
  CHECK_FILTER_REJECTS_UNCHANGED(
      malformed_later, 2U, 0U,
      "zero-capacity filter validates malformed later source without mutation");

  dcc_autocomplete_choice_init(&invalid_choice, "Number");
  invalid_choice.present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
  invalid_choice.value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER;
  invalid_choice.value_number = NAN;
  CHECK_FILTER_REJECTS_UNCHANGED(&invalid_choice, 1U, 1U,
                                 "filter rejects NaN without output mutation");
  invalid_choice.value_number = INFINITY;
  CHECK_FILTER_REJECTS_UNCHANGED(
      &invalid_choice, 1U, 1U,
      "filter rejects positive infinity without output mutation");
  invalid_choice.value_number = -INFINITY;
  CHECK_FILTER_REJECTS_UNCHANGED(
      &invalid_choice, 1U, 1U,
      "filter rejects negative infinity without output mutation");

  dcc_autocomplete_choice_init(&invalid_choice, "Localized");
  invalid_choice.present |=
      DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE |
      DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON;
  invalid_choice.value_string = "localized";
  invalid_choice.value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
  invalid_choice.name_localizations_json = NULL;
  CHECK_FILTER_REJECTS_UNCHANGED(
      &invalid_choice, 1U, 1U,
      "filter rejects present null localizations without output mutation");
#undef CHECK_FILTER_REJECTS_UNCHANGED

  init_filter_output(&outputs, CHOICE_HISTORICAL_STRIDE, 2U, 0xD7);
  unsigned char *terminal_canary =
      outputs.bytes + 2U * CHOICE_HISTORICAL_STRIDE;
  out_count = 99U;
  CHECK(dcc_ctx_autocomplete_filter_choices(
            &ctx, input0, 2U,
            choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 0U), 2U,
            &out_count) == DCC_OK &&
            out_count == 2U &&
            strcmp(choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 0U)->name,
                   "Alpha") == 0 &&
            strcmp(choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 1U)->name,
                   "Alpine") == 0 &&
            bytes_are(terminal_canary, 32U, 0xD7),
        "autocomplete historical input/output stride and canary");

  init_filter_output(&outputs, CHOICE_HISTORICAL_STRIDE, 2U, 0xD7);
  memcpy(output_snapshot, outputs.bytes, sizeof(output_snapshot));
  input1->version++;
  out_count = 99U;
  CHECK(dcc_ctx_autocomplete_filter_choices(
            &ctx, input0, 2U,
            choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 0U), 2U,
            &out_count) == DCC_ERR_INVALID_ARG &&
            out_count == 0U &&
            memcmp(output_snapshot, outputs.bytes, sizeof(output_snapshot)) ==
                0,
        "invalid second input causes no partial output mutation");
  input1->version = DCC_AUTOCOMPLETE_CHOICE_VERSION;

  init_filter_output(&outputs, CHOICE_HISTORICAL_STRIDE, 2U, 0xD7);
  dcc_autocomplete_choice_t *output1 =
      choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 1U);
  output1->size = sizeof(dcc_autocomplete_choice_t);
  memcpy(output_snapshot, outputs.bytes, sizeof(output_snapshot));
  out_count = 99U;
  CHECK(dcc_ctx_autocomplete_filter_choices(
            &ctx, input0, 2U,
            choice_record(&outputs, CHOICE_HISTORICAL_STRIDE, 0U), 2U,
            &out_count) == DCC_ERR_INVALID_ARG &&
            out_count == 0U &&
            memcmp(output_snapshot, outputs.bytes, sizeof(output_snapshot)) ==
                0,
        "mismatched output stride causes no mutation");

  dcc_client_options_t options = {
      .size = sizeof(options),
      .token = "",
      .intents = 0U,
  };
  dcc_client_t *client = NULL;
  CHECK(dcc_client_create(&options, &client) == DCC_OK,
        "create client for autocomplete matching regression");
  builder_v2_runner_t runner;
  CHECK(builder_v2_runner_start(client, &runner),
        "start client for autocomplete matching regression");
  matching_intercept_state_t intercepted;
  memset(&intercepted, 0, sizeof(intercepted));
  dcc_rest_set_interceptor(client, matching_intercept, &intercepted);
  dcc_interaction_t response_interaction;
  memset(&response_interaction, 0, sizeof(response_interaction));
  response_interaction.id = 1U;
  response_interaction.application_id = 2U;
  response_interaction.token = "token";
  response_interaction.focused_option = &focused;
  dcc_ctx_t response_ctx;
  memset(&response_ctx, 0, sizeof(response_ctx));
  response_ctx.client = client;
  response_ctx.interaction = &response_interaction;
  if (dcc_flow_create(client, &response_interaction, &response_ctx.flow) !=
      DCC_OK)
    return 1;
  dcc_status_t reply_status = dcc_ctx_reply_autocomplete_matching(
      &response_ctx, input0, 2U, DCC_AUTOCOMPLETE_MAX_CHOICES, NULL, NULL);
  dcc_status_t drain_status = reply_status == DCC_OK
                                  ? dcc_rest_async_wait(client, 5000U)
                                  : DCC_ERR_STATE;
  builder_v2_runner_stop(&runner);
  CHECK(reply_status == DCC_OK && drain_status == DCC_OK &&
            intercepted.calls == 1U &&
            json_has(intercepted.body, "\"name\":\"Alpha\"") &&
            json_has(intercepted.body, "\"name\":\"Alpine\""),
        "reply autocomplete matching initializes every output slot");
  dcc_client_destroy(client);
  return 0;
}

typedef struct registry_fixture {
  char command_name[32];
  char command_description[48];
  char command_name_localizations[48];
  char option_name[32];
  char option_description[48];
  char nested_name[32];
  char nested_description[48];
  char choice_name[32];
  char choice_value[32];
  char choice_localizations[48];
  char integration_types[16];
  char contexts[16];
  uint32_t channel_types[2];
  dcc_autocomplete_choice_t choices[1];
  dcc_application_command_option_builder_t nested[1];
  dcc_application_command_option_builder_t options[2];
  dcc_application_command_builder_t command;
} registry_fixture_t;

static void registry_fixture_init(registry_fixture_t *fixture,
                                  const char *command_name) {
  memset(fixture, 0, sizeof(*fixture));
  (void)snprintf(fixture->command_name, sizeof(fixture->command_name), "%s",
                 command_name);
  (void)snprintf(fixture->command_description,
                 sizeof(fixture->command_description), "Deep copied command");
  (void)snprintf(fixture->command_name_localizations,
                 sizeof(fixture->command_name_localizations),
                 "{\"ko\":\"copied\"}");
  (void)snprintf(fixture->option_name, sizeof(fixture->option_name), "group");
  (void)snprintf(fixture->option_description,
                 sizeof(fixture->option_description), "Group option");
  (void)snprintf(fixture->nested_name, sizeof(fixture->nested_name), "query");
  (void)snprintf(fixture->nested_description,
                 sizeof(fixture->nested_description), "Query option");
  (void)snprintf(fixture->choice_name, sizeof(fixture->choice_name),
                 "Original choice");
  (void)snprintf(fixture->choice_value, sizeof(fixture->choice_value),
                 "original-value");
  (void)snprintf(fixture->choice_localizations,
                 sizeof(fixture->choice_localizations),
                 "{\"ko\":\"original\"}");
  (void)snprintf(fixture->integration_types, sizeof(fixture->integration_types),
                 "[0]");
  (void)snprintf(fixture->contexts, sizeof(fixture->contexts), "[0,1]");
  fixture->channel_types[0] = 0U;
  fixture->channel_types[1] = 2U;

  dcc_autocomplete_choice_init(&fixture->choices[0], fixture->choice_name);
  (void)dcc_autocomplete_choice_set_name_localizations_json(
      &fixture->choices[0], fixture->choice_localizations);
  (void)dcc_autocomplete_choice_set_string_value(&fixture->choices[0],
                                                 fixture->choice_value);

  dcc_application_command_option_builder_init(&fixture->nested[0]);
  fixture->nested[0].present =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED;
  fixture->nested[0].name = fixture->nested_name;
  fixture->nested[0].description = fixture->nested_description;
  fixture->nested[0].type = DCC_APPLICATION_COMMAND_OPTION_STRING;
  fixture->nested[0].choices = fixture->choices;
  fixture->nested[0].choices_count = 1U;
  fixture->nested[0].required = 0U;

  dcc_application_command_option_builder_init(&fixture->options[0]);
  fixture->options[0].present =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS;
  fixture->options[0].name = fixture->option_name;
  fixture->options[0].description = fixture->option_description;
  fixture->options[0].type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND;
  fixture->options[0].options = fixture->nested;
  fixture->options[0].options_count = 1U;

  dcc_application_command_option_builder_init(&fixture->options[1]);
  fixture->options[1].present =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES;
  fixture->options[1].name = "channel";
  fixture->options[1].description = "Target channel";
  fixture->options[1].type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL;
  fixture->options[1].channel_types = fixture->channel_types;
  fixture->options[1].channel_types_count = 2U;

  dcc_application_command_builder_init(&fixture->command);
  (void)dcc_application_command_builder_set_name(&fixture->command,
                                                 fixture->command_name);
  (void)dcc_application_command_builder_set_name_localizations_json(
      &fixture->command, fixture->command_name_localizations);
  (void)dcc_application_command_builder_set_description(
      &fixture->command, fixture->command_description);
  (void)dcc_application_command_builder_set_type(
      &fixture->command, DCC_APPLICATION_COMMAND_CHAT_INPUT);
  (void)dcc_application_command_builder_set_options(&fixture->command,
                                                    fixture->options, 2U);
  (void)dcc_application_command_builder_set_integration_types_json(
      &fixture->command, fixture->integration_types);
  (void)dcc_application_command_builder_set_contexts_json(&fixture->command,
                                                          fixture->contexts);
}

static int check_registry_copy_lifetime_and_oom(void) {
  registry_fixture_t fixture;
  registry_fixture_init(&fixture, "copy-lifetime");

  size_t first_success = SIZE_MAX;
  for (size_t fail_after = 0U; fail_after < 128U; ++fail_after) {
    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    dcc_command_registry_test_fail_copy_after(fail_after);
    dcc_status_t status =
        dcc_command_registry_add_builder(&registry, &fixture.command);
    dcc_command_registry_test_fail_copy_after(SIZE_MAX);
    if (status == DCC_OK) {
      first_success = fail_after;
      CHECK(dcc_command_registry_count(&registry) == 1U,
            "registry first non-failing copy commits once");
      dcc_command_registry_deinit(&registry);
      break;
    }
    CHECK(status == DCC_ERR_NOMEM &&
              dcc_command_registry_count(&registry) == 0U,
          "registry every partial copy OOM rolls back");
    dcc_command_registry_deinit(&registry);
  }
  CHECK(first_success != SIZE_MAX && first_success >= 12U,
        "registry exercised every deep-copy allocation point");

  registry_fixture_t *lifetime =
      (registry_fixture_t *)malloc(sizeof(*lifetime));
  CHECK(lifetime != NULL, "allocate caller-owned registry fixture");
  registry_fixture_init(lifetime, "copy-lifetime");
  dcc_command_registry_t registry;
  dcc_command_registry_init(&registry);
  CHECK(dcc_command_registry_add_builder(&registry, &lifetime->command) ==
            DCC_OK,
        "registry accepts deep-copy fixture");
  const dcc_command_registry_state_t *state =
      dcc_command_registry_state_const(&registry);
  CHECK(state != NULL && state->entry_count == 1U,
        "registry contains normalized entry");
  const dcc_application_command_builder_t *stored = &state->entries[0].command;
  CHECK(stored->size == sizeof(*stored) &&
            stored->options != lifetime->options &&
            stored->options[0].size == sizeof(stored->options[0]) &&
            stored->options[0].options != lifetime->nested &&
            stored->options[0].options[0].choices != lifetime->choices &&
            stored->options[0].options[0].choices[0].size ==
                sizeof(dcc_autocomplete_choice_t) &&
            stored->options[1].channel_types != lifetime->channel_types,
        "registry normalizes every recursive builder layout");

  lifetime->command_name[0] = 'X';
  lifetime->command_description[0] = 'X';
  lifetime->command_name_localizations[2] = 'X';
  lifetime->option_name[0] = 'X';
  lifetime->option_description[0] = 'X';
  lifetime->nested_name[0] = 'X';
  lifetime->nested_description[0] = 'X';
  lifetime->choice_name[0] = 'X';
  lifetime->choice_value[0] = 'X';
  lifetime->choice_localizations[2] = 'X';
  lifetime->integration_types[1] = '9';
  lifetime->contexts[1] = '9';
  lifetime->channel_types[0] = 99U;
  free(lifetime);
  char *json = NULL;
  CHECK(dcc_application_command_builder_build_json(stored, &json) == DCC_OK &&
            json_has(json, "\"name\":\"copy-lifetime\"") &&
            json_has(json, "\"description\":\"Deep copied command\"") &&
            json_has(json, "\"name\":\"Original choice\"") &&
            json_has(json, "\"value\":\"original-value\"") &&
            json_has(json, "\"channel_types\":[0,2]") &&
            json_has(json, "\"integration_types\":[0]") &&
            json_has(json, "\"contexts\":[0,1]"),
        "registry copy outlives and ignores mutated caller storage");
  dcc_application_command_builder_json_free(json);
  dcc_command_registry_deinit(&registry);

  registry_fixture_init(&fixture, "rollback-second");
  dcc_command_registry_init(&registry);
  dcc_application_command_builder_t seed;
  dcc_application_command_builder_init(&seed);
  CHECK(dcc_application_command_builder_set_name(&seed, "seed") == DCC_OK &&
            dcc_application_command_builder_set_description(&seed, "Seed") ==
                DCC_OK &&
            dcc_command_registry_add_builder(&registry, &seed) == DCC_OK,
        "seed registry rollback case");
  dcc_command_registry_test_fail_copy_after(5U);
  CHECK(dcc_command_registry_add_builder(&registry, &fixture.command) ==
                DCC_ERR_NOMEM &&
            dcc_command_registry_count(&registry) == 1U,
        "registry OOM preserves preexisting entries");
  dcc_command_registry_test_fail_copy_after(SIZE_MAX);
  dcc_command_registry_deinit(&registry);
  return 0;
}

static int check_registry_historical_stride_normalization(void) {
  dcc_command_registry_t registry;
  dcc_command_registry_init(&registry);

  aligned_builder_bytes_t option_storage;
  memset(&option_storage, 0xC3, sizeof(option_storage));
  dcc_application_command_option_builder_t *option0 =
      option_record(&option_storage, OPTION_HISTORICAL_STRIDE, 0U);
  dcc_application_command_option_builder_t *option1 =
      option_record(&option_storage, OPTION_HISTORICAL_STRIDE, 1U);
  init_historical_option(option0, OPTION_HISTORICAL_STRIDE, "history-one",
                         "History one", DCC_APPLICATION_COMMAND_OPTION_STRING);
  init_historical_option(option1, OPTION_HISTORICAL_STRIDE, "history-two",
                         "History two", DCC_APPLICATION_COMMAND_OPTION_INTEGER);
  dcc_application_command_builder_t command;
  dcc_application_command_builder_init(&command);
  CHECK(dcc_application_command_builder_set_name(
            &command, "historical-options") == DCC_OK &&
            dcc_application_command_builder_set_description(
                &command, "Historical option stride") == DCC_OK &&
            dcc_application_command_builder_set_type(
                &command, DCC_APPLICATION_COMMAND_CHAT_INPUT) == DCC_OK &&
            dcc_application_command_builder_set_options(&command, option0,
                                                        2U) == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &command) == DCC_OK,
        "registry accepts historical option stride");
  option0->name = "mutated-one";
  option1->name = "mutated-two";

  char choice_name0[] = "Choice one";
  char choice_name1[] = "Choice two";
  char choice_value0[] = "choice-one";
  char choice_value1[] = "choice-two";
  aligned_builder_bytes_t choice_storage;
  memset(&choice_storage, 0xC4, sizeof(choice_storage));
  dcc_autocomplete_choice_t *choice0 =
      choice_record(&choice_storage, CHOICE_HISTORICAL_STRIDE, 0U);
  dcc_autocomplete_choice_t *choice1 =
      choice_record(&choice_storage, CHOICE_HISTORICAL_STRIDE, 1U);
  init_historical_choice(choice0, CHOICE_HISTORICAL_STRIDE, choice_name0,
                         choice_value0);
  init_historical_choice(choice1, CHOICE_HISTORICAL_STRIDE, choice_name1,
                         choice_value1);
  dcc_application_command_option_builder_t choice_option;
  dcc_application_command_option_builder_init(&choice_option);
  choice_option.present =
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
      DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES;
  choice_option.name = "choice";
  choice_option.description = "Choice option";
  choice_option.type = DCC_APPLICATION_COMMAND_OPTION_STRING;
  choice_option.choices = choice0;
  choice_option.choices_count = 2U;
  dcc_application_command_builder_t choice_command;
  dcc_application_command_builder_init(&choice_command);
  CHECK(dcc_application_command_builder_set_name(
            &choice_command, "historical-choices") == DCC_OK &&
            dcc_application_command_builder_set_description(
                &choice_command, "Historical choice stride") == DCC_OK &&
            dcc_application_command_builder_set_type(
                &choice_command, DCC_APPLICATION_COMMAND_CHAT_INPUT) ==
                DCC_OK &&
            dcc_application_command_builder_set_options(
                &choice_command, &choice_option, 1U) == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &choice_command) ==
                DCC_OK,
        "registry accepts historical choice stride");
  choice_name0[0] = 'X';
  choice_name1[0] = 'X';
  choice_value0[0] = 'X';
  choice_value1[0] = 'X';

  const dcc_command_registry_state_t *state =
      dcc_command_registry_state_const(&registry);
  CHECK(state != NULL && state->entry_count == 2U &&
            state->entries[0].command.options[0].size ==
                sizeof(dcc_application_command_option_builder_t) &&
            state->entries[0].command.options[1].size ==
                sizeof(dcc_application_command_option_builder_t) &&
            state->entries[1].command.options[0].choices[0].size ==
                sizeof(dcc_autocomplete_choice_t) &&
            state->entries[1].command.options[0].choices[1].size ==
                sizeof(dcc_autocomplete_choice_t),
        "registry normalizes historical option and choice records");
  char *json = NULL;
  CHECK(dcc_application_command_builder_build_json(&state->entries[0].command,
                                                   &json) == DCC_OK &&
            json_has(json, "\"name\":\"history-one\"") &&
            json_has(json, "\"name\":\"history-two\""),
        "registry historical options survive caller record mutation");
  dcc_application_command_builder_json_free(json);
  json = NULL;
  CHECK(dcc_application_command_builder_build_json(&state->entries[1].command,
                                                   &json) == DCC_OK &&
            json_has(json, "\"name\":\"Choice one\"") &&
            json_has(json, "\"name\":\"Choice two\"") &&
            json_has(json, "\"value\":\"choice-one\"") &&
            json_has(json, "\"value\":\"choice-two\""),
        "registry historical choices survive caller string mutation");
  dcc_application_command_builder_json_free(json);
  dcc_command_registry_deinit(&registry);
  return 0;
}

static int check_contextual_command_validation(void) {
  char *json = NULL;
  dcc_application_command_builder_t partial;
  dcc_application_command_builder_init(&partial);
  CHECK(dcc_application_command_builder_build_json(&partial, &json) == DCC_OK &&
            strcmp(json, "{}") == 0,
        "generic command serializer remains PATCH-partial");
  dcc_application_command_builder_json_free(json);

  dcc_command_registry_t registry;
  dcc_command_registry_init(&registry);
  CHECK(dcc_command_registry_add_builder(&registry, &partial) ==
            DCC_ERR_INVALID_ARG,
        "registry rejects empty partial command");
  CHECK(dcc_application_command_builder_set_name(
            &partial, "missing-description") == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &partial) ==
                DCC_ERR_INVALID_ARG,
        "registry rejects default chat command without description");
  CHECK(dcc_application_command_builder_set_description(
            &partial, "Description") == DCC_OK,
        "set contextual description");
  partial.type = 99U;
  partial.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
  CHECK(dcc_command_registry_add_builder(&registry, &partial) ==
            DCC_ERR_INVALID_ARG,
        "registry rejects explicitly invalid command type");

  dcc_application_command_builder_t user;
  dcc_application_command_builder_init(&user);
  CHECK(dcc_application_command_builder_set_name(&user, "inspect-user") ==
                DCC_OK &&
            dcc_application_command_builder_set_type(
                &user, DCC_APPLICATION_COMMAND_USER) == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &user) == DCC_OK,
        "registry accepts USER command without description");

  dcc_application_command_builder_t message;
  dcc_application_command_builder_init(&message);
  CHECK(dcc_application_command_builder_set_name(&message, "inspect-message") ==
                DCC_OK &&
            dcc_application_command_builder_set_type(
                &message, DCC_APPLICATION_COMMAND_MESSAGE) == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &message) == DCC_OK,
        "registry accepts MESSAGE command without description");
  CHECK(dcc_application_command_builder_set_description(
            &message, "not allowed") == DCC_OK &&
            dcc_command_registry_add_builder(&registry, &message) ==
                DCC_ERR_INVALID_ARG,
        "registry rejects MESSAGE command create payload fields");

  dcc_application_command_builder_t primary;
  dcc_application_command_builder_init(&primary);
  CHECK(
      dcc_application_command_builder_set_name(&primary, "launch") == DCC_OK &&
          dcc_application_command_builder_set_type(
              &primary, DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT) ==
              DCC_OK &&
          dcc_command_registry_add_builder(&registry, &primary) ==
              DCC_ERR_INVALID_ARG &&
          dcc_application_command_builder_set_description(&primary, "Launch") ==
              DCC_OK &&
          dcc_command_registry_add_builder(&registry, &primary) == DCC_OK,
      "registry requires description for primary entry point");
  dcc_command_registry_deinit(&registry);
  return 0;
}

int builder_task7_typed_records_contract(void);

int main(void) {
  if (check_initializer_contracts() != 0 || check_all_type_abi_matrix() != 0 ||
      check_historical_poison_serialization() != 0 ||
      check_presence_and_zero_semantics() != 0 ||
      check_all_current_setters() != 0 ||
      check_setter_failure_no_mutation() != 0 ||
      check_versioned_stride_arrays() != 0 ||
      check_required_field_rejection() != 0 ||
      check_autocomplete_filter_stride_and_atomicity() != 0 ||
      check_registry_historical_stride_normalization() != 0 ||
      check_registry_copy_lifetime_and_oom() != 0 ||
      check_contextual_command_validation() != 0 ||
      builder_task7_typed_records_contract() != 0) {
    return 1;
  }
  return 0;
}
