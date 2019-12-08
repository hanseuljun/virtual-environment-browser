#include <iostream>
#include <sstream>
#include "libplatform/libplatform.h"
#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "veb_util.h"
#include "veb_bgfx_template.h"
#include "v8pp/context.hpp"
#include "v8pp/module.hpp"

LRESULT CALLBACK MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void dbgTextPrintfWrapper(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format)
{
    bgfx::dbgTextPrintf(_x, _y, _attr, _format);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    const int WIDTH = 1280;
    const int HEIGHT = 720;

    std::string v8_path = veb::find_v8_path();
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(v8_path.c_str());
    v8::V8::InitializeExternalStartupData(v8_path.c_str());
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    auto logger = veb::create_msvc_logger();
    logger->info("PROJECT_SOURCE_DIR: {}", PROJECT_SOURCE_DIR);

    std::string javascript_file_path = PROJECT_SOURCE_DIR;
    javascript_file_path += "/bgfx_examples/00-helloworld/helloworld.js";

    HWND hwnd = veb::init_win32_window(hInstance, nCmdShow, MessageCallback, WIDTH, HEIGHT);

    bgfx::PlatformData pd;
    pd.nwh = hwnd;
    bgfx::setPlatformData(pd);

    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // This makes bgfx to use a default graphics API. For Windows, Direct3D11.
    init.vendorId = BGFX_PCI_ID_NONE; // This also makes bgfx to pick a GPU.
    init.resolution.width = WIDTH;
    init.resolution.height = HEIGHT;
    init.resolution.reset = reset;
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(debug);

    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x303030ff,
                       1.0f,
                       0);

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        // Create a template for the global object where we set the
        // built-in global functions.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        veb::BgfxTemplate bgfx_template(isolate);

        global->Set(v8::String::NewFromUtf8(isolate, "bgfx").ToLocalChecked(),
                    bgfx_template.object_template());
        
        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);

        //v8pp::module global(isolate);
        //v8pp::module m(isolate);
        //m.function("dbgTextPrintf", &dbgTextPrintfWrapper);
        //global.submodule("bgfx", m);
        //v8pp::context context(isolate);
        
        //global->Set(v8::String::NewFromUtf8(isolate, "v8pp").ToLocalChecked(), m.new_instance());
        //isolate->GetCurrentContext()->Global()->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "v8pp").ToLocalChecked(), m.new_instance());

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);


        v8::Local<v8::String> source;
        if (!veb::read_file(isolate, javascript_file_path.c_str()).ToLocal(&source)) {
            isolate->ThrowException(
                v8::String::NewFromUtf8(isolate, "Error loading file",
                                        v8::NewStringType::kNormal).ToLocalChecked());
            return 1;
        }

        // Run the message loop.
        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
            bgfx::touch(0);

            if (!veb::execute_string(isolate, source, true, true)) {
                isolate->ThrowException(
                    v8::String::NewFromUtf8(isolate, "Error executing file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
                return 1;
            }

            bgfx::frame();
        }
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();

    bgfx::shutdown();

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;

    return 0;
}
