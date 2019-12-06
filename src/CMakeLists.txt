add_executable(Main WIN32 main.cpp)
target_include_directories(Main PRIVATE
                           ${VCPKG_INCLUDE_DIR}
                           ${BX_DIR}/include
                           ${BX_DIR}/include/compat/msvc
   	                       ${BGFX_DIR}/include)
target_link_libraries(Main
                      debug ${VCPKG_LIB_DIR_DEBUG}/fmtd.lib       optimized ${VCPKG_LIB_DIR_RELEASE}/fmt.lib
                      debug ${VCPKG_LIB_DIR_DEBUG}/spdlogd.lib    optimized ${VCPKG_LIB_DIR_RELEASE}/spdlog.lib
                      debug ${BGFX_DIR}/lib/bgfxDebug.lib        optimized ${BGFX_DIR}/lib/bgfxRelease.lib
                      debug ${BGFX_DIR}/lib/bimg_decodeDebug.lib optimized ${BGFX_DIR}/lib/bimg_decodeRelease.lib
                      debug ${BGFX_DIR}/lib/bimgDebug.lib        optimized ${BGFX_DIR}/lib/bimgRelease.lib
                      debug ${BGFX_DIR}/lib/bxDebug.lib          optimized ${BGFX_DIR}/lib/bxRelease.lib)
set_target_properties(Main PROPERTIES
                      CXX_STANDARD 17)