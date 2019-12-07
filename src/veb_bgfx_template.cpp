#include "veb_bgfx_template.h"

#include <functional>
#include "bgfx/bgfx.h"

namespace veb
{
BgfxTemplate::BgfxTemplate(v8::Isolate* isolate)
    : object_template_(v8::ObjectTemplate::New(isolate)),
      bgfx_stats_template_(isolate)
{
    object_template_->Set(v8::String::NewFromUtf8(isolate, "frame").ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, frameCallback));
    object_template_->Set(v8::String::NewFromUtf8(isolate, "dbgTextPrintf").ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, dbgTextPrintfCallback));
    object_template_->Set(v8::String::NewFromUtf8(isolate, "getStats").ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, getStatsCallback, v8::External::New(isolate, this)));
    object_template_->Set(v8::String::NewFromUtf8(isolate, "touch").ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, touchCallback));
}

void BgfxTemplate::frameCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bgfx::frame();
}

void BgfxTemplate::dbgTextPrintfCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
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

void BgfxTemplate::getStatsCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::External> data = args.Data().As<v8::External>();
    auto bgfx_template = reinterpret_cast<BgfxTemplate*>(data->Value());

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());

    const bgfx::Stats* stats = bgfx::getStats();
    v8::Local<v8::Object> stats_object = bgfx_template->bgfx_stats_template_.object_template()->NewInstance(context).ToLocalChecked();
    stats_object->SetInternalField(0, v8::External::New(isolate, const_cast<bgfx::Stats*>(stats)));
    args.GetReturnValue().Set(stats_object);
}

void BgfxTemplate::touchCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 1)
        return;

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());

    bgfx::ViewId id = static_cast<bgfx::ViewId>(args[0]->IntegerValue(context).ToChecked());
    bgfx::touch(id);
}
}