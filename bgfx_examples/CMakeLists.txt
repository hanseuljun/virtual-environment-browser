add_executable(00-helloworld WIN32 00-helloworld/helloworld.cpp)
target_include_directories(00-helloworld PRIVATE ${EXAMPLES_INCLUDE})
target_link_libraries(00-helloworld PRIVATE VirtualEnvironmentBrowserLibrary)
set_target_properties(00-helloworld PROPERTIES CXX_STANDARD 17)