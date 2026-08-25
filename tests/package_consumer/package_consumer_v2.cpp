#include <dcc/bot.h>

#ifdef DCC_UI_ROW
#error "C-only Bot UI macros must not be visible to C++"
#endif

int dcc_package_consumer_cpp_probe() {
    dcc_component_v2_builder_t child = DCC_UI_TEXT("C++17");
    return DCC_UI_ROW_ARRAY(&child, 1U).type == DCC_COMPONENT_V2_ACTION_ROW
               ? 0
               : 1;
}
