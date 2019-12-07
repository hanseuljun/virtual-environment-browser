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
// read_file(), to_c_string(), report_exception(), and execute_string() are from v8 example code.
v8::MaybeLocal<v8::String> read_file(v8::Isolate* isolate, const char* name);
const char* to_c_string(const v8::String::Utf8Value& value);
void report_exception(v8::Isolate* isolate, v8::TryCatch* try_catch);
bool execute_string(v8::Isolate* isolate, v8::Local<v8::String> source,
                    bool print_result,
                    bool report_exceptions);
}