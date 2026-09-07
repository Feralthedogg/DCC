cmake_minimum_required(VERSION 3.20)

project(dcc_documentation_examples LANGUAGES C)

find_package(dcc CONFIG REQUIRED)

add_executable(dcc_rest_ownership rest_ownership.c)
add_executable(dcc_guide_examples guide_flows.c guide_sessions.c guide_configuration.c)

foreach(target dcc_rest_ownership dcc_guide_examples)
    target_compile_features(${target} PRIVATE c_std_11)
    target_link_libraries(${target} PRIVATE dcc::dcc)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endforeach()

enable_testing()
add_test(NAME dcc_guide_examples_offline COMMAND dcc_guide_examples)
