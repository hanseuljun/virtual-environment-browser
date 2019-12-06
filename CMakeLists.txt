cmake_minimum_required(VERSION 3.13)
project(VirtualEnvironmentBrowser)

#find_path(VCPKG_INCLUDE_DIR NAMES spdlog.h PATHSUFFICES)
#set(VCPKG_INCLUDE_DIR ${VCPKG_INCLUDE_DIR}/..)
#find_path(VCPKG_LIB_DIR NAMES spdlog.lib)

# The above code finding ${VCPKG_INCLUDE_DIR} and ${VCPKG_LIB_DIR} using find_path does not work for x64-windows-static.
set(VCPKG_DIR ${PROJECT_SOURCE_DIR}/vcpkg/installed/x64-windows-static)
set(VCPKG_INCLUDE_DIR ${VCPKG_DIR}/include)
set(VCPKG_LIB_DIR_DEBUG ${VCPKG_DIR}/debug/lib)
set(VCPKG_LIB_DIR_RELEASE ${VCPKG_DIR}/lib)
message(${VCPKG_INCLUDE_DIR})
message(${VCPKG_LIB_DIR})

set(BX_DIR ${PROJECT_SOURCE_DIR}/third_party/bx)
set(BGFX_DIR ${PROJECT_SOURCE_DIR}/third_party/bgfx)

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

add_subdirectory(src)
add_subdirectory(v8_examples)