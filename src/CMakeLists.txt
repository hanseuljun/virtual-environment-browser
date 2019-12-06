add_library(VirtualEnvironmentBrowserLibrary veb_util.h veb_util.cpp)
target_include_directories(VirtualEnvironmentBrowserLibrary PRIVATE
                           ${VCPKG_INCLUDE_DIR}
                           ${V8_DIR}/include
                           ${BX_DIR}/include
                           ${BX_DIR}/include/compat/msvc
   	                       ${BGFX_DIR}/include)
target_link_libraries(VirtualEnvironmentBrowserLibrary PRIVATE
                      dbghelp.lib winmm.lib
                      debug ${V8_DEBUG}/v8_libbase.lib     optimized ${V8_RELEASE}/v8_libbase.lib
                      debug ${V8_DEBUG}/v8_libplatform.lib optimized ${V8_RELEASE}/v8_libplatform.lib
                      debug ${V8_DEBUG}/wee8.lib           optimized ${V8_RELEASE}/wee8.lib
                      debug ${VCPKG_LIB_DIR_DEBUG}/fmtd.lib       optimized ${VCPKG_LIB_DIR_RELEASE}/fmt.lib
                      debug ${VCPKG_LIB_DIR_DEBUG}/spdlogd.lib    optimized ${VCPKG_LIB_DIR_RELEASE}/spdlog.lib
                      debug ${BGFX_DIR}/lib/bgfxDebug.lib        optimized ${BGFX_DIR}/lib/bgfxRelease.lib
                      debug ${BGFX_DIR}/lib/bimg_decodeDebug.lib optimized ${BGFX_DIR}/lib/bimg_decodeRelease.lib
                      debug ${BGFX_DIR}/lib/bimgDebug.lib        optimized ${BGFX_DIR}/lib/bimgRelease.lib
                      debug ${BGFX_DIR}/lib/bxDebug.lib          optimized ${BGFX_DIR}/lib/bxRelease.lib)
set_target_properties(VirtualEnvironmentBrowserLibrary PROPERTIES
                      CXX_STANDARD 17)