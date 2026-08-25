cmake_minimum_required(VERSION 3.20)

project(dcc_package_consumer LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(dcc CONFIG REQUIRED)

add_executable(dcc_package_consumer
    package_consumer_v2.c
    package_consumer_v2.cpp
)
set_target_properties(dcc_package_consumer PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(dcc_package_consumer PRIVATE dcc::dcc)

dcc_add_hot_reload_module(dcc_package_consumer_module
    SOURCES
        hot_reload_module.c
)
get_target_property(DCC_PACKAGE_CONSUMER_MODULE_TYPE dcc_package_consumer_module TYPE)
if(NOT DCC_PACKAGE_CONSUMER_MODULE_TYPE STREQUAL "MODULE_LIBRARY")
    message(FATAL_ERROR "dcc_add_hot_reload_module did not create a MODULE library")
endif()
get_target_property(DCC_PACKAGE_CONSUMER_MODULE_PREFIX dcc_package_consumer_module PREFIX)
if(NOT "${DCC_PACKAGE_CONSUMER_MODULE_PREFIX}" STREQUAL "")
    message(FATAL_ERROR "dcc_add_hot_reload_module must disable library prefixes")
endif()
