cmake_minimum_required(VERSION 3.13)
add_library("pipe" INTERFACE)
target_include_directories("pipe" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
