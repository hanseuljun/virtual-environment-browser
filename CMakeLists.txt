cmake_minimum_required(VERSION 3.13)
project(VirtualEnvironmentBrowser)

set(V8_DIR ${PROJECT_SOURCE_DIR}/third_party/v8)
set(V8_DEBUG ${V8_DIR}/debug)
set(V8_RELEASE ${V8_DIR}/release)

# V8 requires the path to some binary files and the below definition is for finding those files.
add_definitions(-DV8_DIR=\"${V8_DIR}\")

# V8 is built with the static runtime library and matching this project V8.
# Matching the type of runtime library is mandatory.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /MT")

add_subdirectory(v8_examples)