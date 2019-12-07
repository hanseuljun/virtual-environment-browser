#include "veb_util.h"

#include "spdlog/sinks/msvc_sink.h"

namespace veb
{
std::string find_v8_path()
{
    std::string v8_path = V8_DIR;
#if NDEBUG
    v8_path += "/release/";
#else
    v8_path += "/debug/";
#endif

    return v8_path;
}

std::unique_ptr<spdlog::logger> create_msvc_logger()
{
    auto sink = std::make_unique<spdlog::sinks::msvc_sink_mt>();
    return std::make_unique<spdlog::logger>("msvc_logger", std::move(sink));
}

HWND init_win32_window(HINSTANCE hInstance, int nCmdShow, WNDPROC callback)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = callback;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"VirtualEnvironmentBrowserWindowClass";

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(0,                              // Optional window styles.
                               wc.lpszClassName,               // Window class
                               L"Virtual Environment Browser", // Window text
                               WS_OVERLAPPEDWINDOW,            // Window style 
                               CW_USEDEFAULT,                  // Size and position
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               NULL,                           // Parent window    
                               NULL,                           // Menu
                               hInstance,                      // Instance handle
                               NULL);                          // Additional application data

    ShowWindow(hwnd, nCmdShow);
    return hwnd;
}
}