#include <iostream>
#include <sstream>
#include "libplatform/libplatform.h"
#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "veb_util.h"
#include "veb_bgfx_template.h"

LRESULT CALLBACK MessageCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Reads a file into a v8 string.
v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) return v8::MaybeLocal<v8::String>();

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return v8::MaybeLocal<v8::String>();
        }
    }
    fclose(file);
    v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(
        isolate, chars, v8::NewStringType::kNormal, static_cast<int>(size));
    delete[] chars;
    return result;
}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    auto logger = veb::create_msvc_logger();
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        //fprintf(stderr, "%s\n", exception_string);
        logger->error(exception_string);
    } else {
        std::string message_string;
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(isolate,
                                       message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        //fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
        message_string += fmt::format("{}:{} : {}\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(
            isolate, message->GetSourceLine(context).ToLocalChecked());
        const char* sourceline_string = ToCString(sourceline);
        //fprintf(stderr, "%s\n", sourceline_string);
        message_string += fmt::format("{}\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++) {
            //fprintf(stderr, " ");
            message_string += " ";
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++) {
            //fprintf(stderr, "^");
            message_string += "^";
        }
        //fprintf(stderr, "\n");
        message_string += "\n";
        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            const char* stack_trace_string = ToCString(stack_trace);
            //fprintf(stderr, "%s\n", stack_trace_string);
            message_string += fmt::format("{}\n", stack_trace_string);
        }
        logger->error(message_string);
    }
}

// Executes a string within the current v8 context.
bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source,
                   bool print_result,
                   bool report_exceptions) {
    auto logger = veb::create_msvc_logger();
    v8::HandleScope handle_scope(isolate);
    v8::TryCatch try_catch(isolate);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source).ToLocal(&script)) {
        // Print errors that happened during compilation.
        if (report_exceptions)
            ReportException(isolate, &try_catch);
        return false;
    } else {
        v8::Local<v8::Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            // Print errors that happened during execution.
            if (report_exceptions)
                ReportException(isolate, &try_catch);
            return false;
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                // If all went well and the result wasn't undefined then print
                // the returned value.
                v8::String::Utf8Value str(isolate, result);
                const char* cstr = ToCString(str);
                //printf("%s\n", cstr);
                logger->info(cstr);
            }
            return true;
        }
    }
}

//static void BgfxDbgTextPrintfCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
//    if (args.Length() < 4)
//        return;
//
//    v8::Isolate* isolate = args.GetIsolate();
//    v8::HandleScope scope(isolate);
//    v8::Local<v8::Context> context(isolate->GetCurrentContext());
//    uint16_t x = static_cast<uint16_t>(args[0]->IntegerValue(context).ToChecked());
//    uint16_t y = static_cast<uint16_t>(args[1]->IntegerValue(context).ToChecked());
//    uint8_t attr = static_cast<uint8_t>(args[2]->IntegerValue(context).ToChecked());
//    v8::String::Utf8Value format(isolate, args[3]);
//
//    bgfx::dbgTextPrintf(x, y, attr, *format);
//}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
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

    HWND hwnd = veb::init_win32_window(hInstance, nCmdShow, MessageCallback);

    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) {
        return 1;
    }

    bgfx::PlatformData pd;
    pd.nwh = hwnd;
    bgfx::setPlatformData(pd);

    uint32_t width = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;
    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = BGFX_RESET_VSYNC;

    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // This makes bgfx to use a default graphics API. For Windows, Direct3D11.
    init.vendorId = BGFX_PCI_ID_NONE; // This also makes bgfx to pick a GPU.
    init.resolution.width = width;
    init.resolution.height = height;
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

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        bgfx::touch(0);

        v8::Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        // Create a template for the global object where we set the
        // built-in global functions.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        //v8::Local<v8::ObjectTemplate> bgfx = v8::ObjectTemplate::New(isolate);

        veb::BgfxTemplate bgfx_template(isolate);

        global->Set(v8::String::NewFromUtf8(isolate, "bgfx", v8::NewStringType::kNormal)
                    .ToLocalChecked(),
                    bgfx_template.object_template());

        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);
        {
            v8::Local<v8::String> source;
            if (!ReadFile(isolate, javascript_file_path.c_str()).ToLocal(&source)) {
                isolate->ThrowException(
                    v8::String::NewFromUtf8(isolate, "Error loading file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
                return 1;
            }

            if (!ExecuteString(isolate, source, true, true)) {
                isolate->ThrowException(
                    v8::String::NewFromUtf8(isolate, "Error executing file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
                return 1;
            }
        }

        bgfx::frame();
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();

    bgfx::shutdown();

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;

    return 0;
}
