#include <iostream>
#include <sstream>
#include "libplatform/libplatform.h"
#include "bx/allocator.h"
#include "bx/bx.h"
#include "bx/file.h"
#include "bx/math.h"
#include "bx/timer.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "veb_util.h"
#include "veb_bgfx_template.h"

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};

static const char* s_ptNames[]
{
    "Triangle List"
};

static const uint64_t s_ptState[]
{
    UINT64_C(0)
};
BX_STATIC_ASSERT(BX_COUNTOF(s_ptState) == BX_COUNTOF(s_ptNames));

bx::AllocatorI* getDefaultAllocator()
{
    BX_PRAGMA_DIAGNOSTIC_PUSH();
    BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4459); // warning C4459: declaration of 's_allocator' hides global declaration
    BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
    static bx::DefaultAllocator s_allocator;
    return &s_allocator;
    BX_PRAGMA_DIAGNOSTIC_POP();
}

static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
    if (bx::open(_reader, _filePath))
    {
        uint32_t size = (uint32_t)bx::getSize(_reader);
        const bgfx::Memory* mem = bgfx::alloc(size + 1);
        bx::read(_reader, mem->data, size);
        bx::close(_reader);
        mem->data[mem->size - 1] = '\0';
        return mem;
    }

    auto logger = veb::create_msvc_logger();
    logger->error("Failed to load {}.", _filePath);
    return NULL;
}

LRESULT CALLBACK MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
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
    init.type = bgfx::RendererType::Direct3D11; // This makes bgfx to use a default graphics API. For Windows, Direct3D11.
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

    // Create vertex stream declaration.
    PosColorVertex::init();

    // Create static vertex buffer.
    bgfx::VertexBufferHandle m_vbh = bgfx::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
        , PosColorVertex::ms_layout
    );

    bgfx::IndexBufferHandle m_ibh[BX_COUNTOF(s_ptState)];
    // Create static index buffer for triangle list rendering.
    m_ibh[0] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    // Create program from shaders.
    bx::FileReader file_reader;
    std::string shaderPath = PROJECT_SOURCE_DIR;
    shaderPath += "/bgfx_examples/shaders/dx11/";
    std::string vs_path = shaderPath + "vs_cubes.bin";
    std::string fs_path = shaderPath + "fs_cubes.bin";
    bgfx::ShaderHandle vsh = bgfx::createShader(loadMem(&file_reader, vs_path.c_str()));
    bgfx::ShaderHandle fsh = bgfx::createShader(loadMem(&file_reader, fs_path.c_str()));

    //return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
    bgfx::ProgramHandle m_program = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

    int64_t m_timeOffset = bx::getHPCounter();

    int32_t m_pt = 0;

    bool m_r = true;
    bool m_g = true;
    bool m_b = true;
    bool m_a = true;

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

        global->Set(v8::String::NewFromUtf8(isolate, "bgfx", v8::NewStringType::kNormal)
                    .ToLocalChecked(),
                    bgfx_template.object_template());

        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
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

            //if (!veb::execute_string(isolate, source, true, true)) {
            //    isolate->ThrowException(
            //        v8::String::NewFromUtf8(isolate, "Error executing file",
            //                                v8::NewStringType::kNormal).ToLocalChecked());
            //    return 1;
            //}

            float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));

            const bx::Vec3 at = { 0.0f, 0.0f,   0.0f };
            const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

            // Set view and projection matrix for view 0.
            {
                float view[16];
                bx::mtxLookAt(view, eye, at);

                float proj[16];
                bx::mtxProj(proj, 60.0f, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(0, view, proj);

                // Set view 0 default viewport.
                bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
            }

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(0);

            bgfx::IndexBufferHandle ibh = m_ibh[m_pt];
            uint64_t state = 0
                | (m_r ? BGFX_STATE_WRITE_R : 0)
                | (m_g ? BGFX_STATE_WRITE_G : 0)
                | (m_b ? BGFX_STATE_WRITE_B : 0)
                | (m_a ? BGFX_STATE_WRITE_A : 0)
                | BGFX_STATE_WRITE_Z
                | BGFX_STATE_DEPTH_TEST_LESS
                | BGFX_STATE_CULL_CW
                | BGFX_STATE_MSAA
                | s_ptState[m_pt]
                ;

            // Submit 11x11 cubes.
            for (uint32_t yy = 0; yy < 11; ++yy)
            {
                for (uint32_t xx = 0; xx < 11; ++xx)
                {
                    float mtx[16];
                    bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
                    mtx[12] = -15.0f + float(xx) * 3.0f;
                    mtx[13] = -15.0f + float(yy) * 3.0f;
                    mtx[14] = 0.0f;

                    // Set model matrix for rendering.
                    bgfx::setTransform(mtx);

                    // Set vertex and index buffer.
                    bgfx::setVertexBuffer(0, m_vbh);
                    bgfx::setIndexBuffer(ibh);

                    // Set render states.
                    bgfx::setState(state);

                    // Submit primitive for rendering to view 0.
                    bgfx::submit(0, m_program);
                }
            }

            bgfx::frame();
        }
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();

    // Cleanup.
    for (uint32_t ii = 0; ii < BX_COUNTOF(m_ibh); ++ii)
    {
        bgfx::destroy(m_ibh[ii]);
    }

    bgfx::destroy(m_vbh);
    bgfx::destroy(m_program);

    bgfx::shutdown();

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;

    return 0;
}
