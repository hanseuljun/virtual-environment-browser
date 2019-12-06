#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <memory>

#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "spdlog/spdlog.h"

namespace veb
{
std::unique_ptr<spdlog::logger> create_msvc_logger();
HWND init_win32_window(HINSTANCE hInstance, int nCmdShow, WNDPROC callback);
}