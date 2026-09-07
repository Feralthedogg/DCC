cmake_minimum_required(VERSION 3.20)

project(dcc_documentation_examples LANGUAGES C)

find_package(dcc CONFIG REQUIRED)

add_executable(dcc_rest_ownership rest_ownership.c)
target_compile_features(dcc_rest_ownership PRIVATE c_std_11)
target_link_libraries(dcc_rest_ownership PRIVATE dcc::dcc)

if(MSVC)
    target_compile_options(dcc_rest_ownership PRIVATE /W4 /WX)
else()
    target_compile_options(dcc_rest_ownership PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
