cmake_minimum_required(VERSION 3.22)
project(FindTheWayIOSBuild LANGUAGES C CXX)
include(CTest)
# This project owns build orchestration, not a second copy of the shared core.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Core"
                 "${CMAKE_CURRENT_BINARY_DIR}/core")
