add_executable(HelloWorld hello_world.cpp)
target_include_directories(HelloWorld PRIVATE
   	                       ${V8_DIR}/include)  	                       
target_link_libraries(HelloWorld
                      dbghelp.lib winmm.lib
                      debug ${V8_DEBUG}/v8_libbase.lib     optimized ${V8_RELEASE}/v8_libbase.lib
                      debug ${V8_DEBUG}/v8_libplatform.lib optimized ${V8_RELEASE}/v8_libplatform.lib
                      debug ${V8_DEBUG}/wee8.lib           optimized ${V8_RELEASE}/wee8.lib)
set_target_properties(HelloWorld PROPERTIES
                      CXX_STANDARD 17)

add_executable(Process process.cpp)
target_include_directories(Process PRIVATE
   	                       ${V8_DIR}/include)  	                       
target_link_libraries(Process
                      dbghelp.lib winmm.lib
                      debug ${V8_DEBUG}/v8_libbase.lib     optimized ${V8_RELEASE}/v8_libbase.lib
                      debug ${V8_DEBUG}/v8_libplatform.lib optimized ${V8_RELEASE}/v8_libplatform.lib
                      debug ${V8_DEBUG}/wee8.lib           optimized ${V8_RELEASE}/wee8.lib)
set_target_properties(Process PROPERTIES
                      CXX_STANDARD 17)

add_executable(Shell shell.cpp)
target_include_directories(Shell PRIVATE
   	                       ${V8_DIR}/include)  	                       
target_link_libraries(Shell
                      dbghelp.lib winmm.lib
                      debug ${V8_DEBUG}/v8_libbase.lib     optimized ${V8_RELEASE}/v8_libbase.lib
                      debug ${V8_DEBUG}/v8_libplatform.lib optimized ${V8_RELEASE}/v8_libplatform.lib
                      debug ${V8_DEBUG}/wee8.lib           optimized ${V8_RELEASE}/wee8.lib)
set_target_properties(Shell PROPERTIES
                      CXX_STANDARD 17)