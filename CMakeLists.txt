cmake_minimum_required(VERSION 3.4)

project(picorange CXX)

set(MASTER_PROJECT OFF)
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(MASTER_PROJECT ON)
endif ()

add_library(picorange INTERFACE)
target_include_directories(picorange INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

add_executable(picorange-test test.cpp)
target_link_libraries(picorange-test PUBLIC picorange)
