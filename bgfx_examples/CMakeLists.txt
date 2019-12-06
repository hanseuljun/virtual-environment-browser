add_executable(Main WIN32 main.cpp)
target_include_directories(Main PRIVATE
                           ${VCPKG_INCLUDE_DIR}
                           ${BX_DIR}/include
                           ${BX_DIR}/include/compat/msvc
   	                       ${BGFX_DIR}/include)
target_link_libraries(Main PRIVATE
                      VirtualEnvironmentBrowserLibrary)
set_target_properties(Main PROPERTIES
                      CXX_STANDARD 17)