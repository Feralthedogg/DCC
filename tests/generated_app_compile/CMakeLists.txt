cmake_minimum_required(VERSION 3.20)

project(dcc_generated_app_compile LANGUAGES C)

if(NOT DEFINED DCC_SOURCE_DIR OR NOT EXISTS "${DCC_SOURCE_DIR}/include/dcc/dcc.h")
    message(FATAL_ERROR "DCC_SOURCE_DIR must point to the DCC source tree")
endif()

if(NOT DEFINED DCC_GENERATED_APP_DIR OR NOT EXISTS "${DCC_GENERATED_APP_DIR}/src/main.c")
    message(FATAL_ERROR "DCC_GENERATED_APP_DIR must point to a dcc_new_app output directory")
endif()

file(GLOB DCC_GENERATED_APP_SOURCES CONFIGURE_DEPENDS
    "${DCC_GENERATED_APP_DIR}/src/*.c"
)

add_library(dcc_generated_app_object OBJECT ${DCC_GENERATED_APP_SOURCES})
target_include_directories(dcc_generated_app_object
    PRIVATE
        "${DCC_SOURCE_DIR}/include"
)
target_compile_features(dcc_generated_app_object PRIVATE c_std_11)
if(MSVC)
    target_compile_options(dcc_generated_app_object PRIVATE /W4 /WX)
else()
    target_compile_options(dcc_generated_app_object PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
