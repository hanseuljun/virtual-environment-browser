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

HWND init_win32_window(HINSTANCE hInstance, int nCmdShow, WNDPROC callback, int width, int height)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = callback;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"VirtualEnvironmentBrowserWindowClass";

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(0,                                   // Optional window styles.
                               wc.lpszClassName,                    // Window class
                               L"Virtual Environment Browser",      // Window text
                               WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, // Window style 
                               CW_USEDEFAULT,                       // Size and position
                               CW_USEDEFAULT,
                               width,
                               height,
                               NULL,                                // Parent window    
                               NULL,                                // Menu
                               hInstance,                           // Instance handle
                               NULL);                               // Additional application data

    ShowWindow(hwnd, nCmdShow);
    return hwnd;
}

// Reads a file into a v8 string.
v8::MaybeLocal<v8::String> read_file(v8::Isolate* isolate, const char* name) {
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
const char* to_c_string(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void report_exception(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    auto logger = veb::create_msvc_logger();
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());
    const char* exception_string = to_c_string(exception);
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
        const char* filename_string = to_c_string(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        //fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
        message_string += fmt::format("{}:{} : {}\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(
            isolate, message->GetSourceLine(context).ToLocalChecked());
        const char* sourceline_string = to_c_string(sourceline);
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
            const char* stack_trace_string = to_c_string(stack_trace);
            //fprintf(stderr, "%s\n", stack_trace_string);
            message_string += fmt::format("{}\n", stack_trace_string);
        }
        logger->error(message_string);
    }
}

// Executes a string within the current v8 context.
bool execute_string(v8::Isolate* isolate, v8::Local<v8::String> source,
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
            report_exception(isolate, &try_catch);
        return false;
    } else {
        v8::Local<v8::Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            // Print errors that happened during execution.
            if (report_exceptions)
                report_exception(isolate, &try_catch);
            return false;
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                // If all went well and the result wasn't undefined then print
                // the returned value.
                v8::String::Utf8Value str(isolate, result);
                const char* cstr = to_c_string(str);
                //printf("%s\n", cstr);
                logger->info(cstr);
            }
            return true;
        }
    }
}
}