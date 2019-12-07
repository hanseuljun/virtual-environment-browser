#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <memory>

#include "v8.h"
#include "spdlog/spdlog.h"

namespace veb
{
std::string find_v8_path();
std::unique_ptr<spdlog::logger> create_msvc_logger();
HWND init_win32_window(HINSTANCE hInstance, int nCmdShow, WNDPROC callback);
}