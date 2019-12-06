add_executable(HelloWorld hello_world.cpp)
target_include_directories(HelloWorld PRIVATE ${EXAMPLES_INCLUDE})
target_link_libraries(HelloWorld VirtualEnvironmentBrowserLibrary)
set_target_properties(HelloWorld PROPERTIES CXX_STANDARD 17)

add_executable(Process process.cpp)
target_include_directories(Process PRIVATE ${EXAMPLES_INCLUDE})        
target_link_libraries(Process VirtualEnvironmentBrowserLibrary)
set_target_properties(Process PROPERTIES CXX_STANDARD 17)

add_executable(Shell shell.cpp)
target_include_directories(Shell PRIVATE ${EXAMPLES_INCLUDE})          
target_link_libraries(Shell VirtualEnvironmentBrowserLibrary)
set_target_properties(Shell PROPERTIES CXX_STANDARD 17)