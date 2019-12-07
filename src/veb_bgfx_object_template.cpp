#include "veb_bgfx_object_template.h"

#include "bgfx/bgfx.h"

namespace veb
{
static void BgfxDbgTextPrintfCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() < 4)
        return;

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    uint16_t x = static_cast<uint16_t>(args[0]->IntegerValue(context).ToChecked());
    uint16_t y = static_cast<uint16_t>(args[1]->IntegerValue(context).ToChecked());
    uint8_t attr = static_cast<uint8_t>(args[2]->IntegerValue(context).ToChecked());
    v8::String::Utf8Value format(isolate, args[3]);

    bgfx::dbgTextPrintf(x, y, attr, *format);
}

v8::Local<v8::ObjectTemplate> create_bgfx_object_template(v8::Isolate* isolate)
{
    v8::Local<v8::ObjectTemplate> bgfx = v8::ObjectTemplate::New(isolate);
    bgfx->Set(v8::String::NewFromUtf8(isolate, "dbgTextPrintf", v8::NewStringType::kNormal)
              .ToLocalChecked(),
              v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));

    return bgfx;
}
}