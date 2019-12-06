#ifndef UNICODE
#define UNICODE
#endif

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <iostream>
#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"

LRESULT CALLBACK MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("msvc_logger", sink);
    logger->info("Hello, {}!", "World");

    WNDCLASS wc = { };
    wc.lpfnWndProc = MessageCallback;
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

    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) {
        return 0;
    }

    logger->info("rect.left: {}", rect.left);
    logger->info("rect.top: {}", rect.top);
    logger->info("rect.right: {}", rect.right);
    logger->info("rect.bottom: {}", rect.bottom);

    bgfx::PlatformData pd;
    bx::memSet(&pd, 0, sizeof(pd));
    pd.nwh = hwnd;
    bgfx::setPlatformData(pd);

    uint32_t width = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;
    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    logger->info("width: {}", width);
    logger->info("height: {}", height);

    bgfx::Init init;
    init.type = bgfx::RendererType::Direct3D11;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = reset;
    bool init_result = bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(debug);

    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       //0x303030ff,
                       0xff0000ff,
                       1.0f,
                       0);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        bgfx::touch(0);
        bgfx::frame();
    }

    return 0;
}
