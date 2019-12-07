#include "veb_bgfx_template.h"

#include "bgfx/bgfx.h"

namespace veb
{
static void BgfxDbgTextPrintfCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
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

static void GetStatsWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    info.GetReturnValue().Set(10);
}

static void BgfxGetStats(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> stats = v8::Object::New(args.GetIsolate());
}

//v8::Local<v8::ObjectTemplate> create_bgfx_object_template(v8::Isolate* isolate)
//{
//    v8::Local<v8::ObjectTemplate> bgfx = v8::ObjectTemplate::New(isolate);
//    bgfx->Set(v8::String::NewFromUtf8(isolate, "dbgTextPrintf").ToLocalChecked(),
//              v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));
//    bgfx->Set(v8::String::NewFromUtf8(isolate, "GetStats").ToLocalChecked(),
//              v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));
//
//    v8::Local<v8::ObjectTemplate> stats_templ = v8::ObjectTemplate::New(isolate);
//    stats_templ->SetInternalFieldCount(1);
//    stats_templ->SetAccessor(v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(),
//                             GetStatsWidth);
//
//    return bgfx;
//}

BgfxTemplate::BgfxTemplate(v8::Isolate* isolate)
    : object_template_(v8::ObjectTemplate::New(isolate))
{
    object_template_->Set(v8::String::NewFromUtf8(isolate, "dbgTextPrintf").ToLocalChecked(),
                          v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));
}

//BgfxTemplate BgfxTemplate::Create(v8::Isolate* isolate)
//{
//    v8::Local<v8::ObjectTemplate> object_template = v8::ObjectTemplate::New(isolate);
//    object_template->Set(v8::String::NewFromUtf8(isolate, "dbgTextPrintf").ToLocalChecked(),
//                         v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));
//    //object_template->Set(v8::String::NewFromUtf8(isolate, "GetStats").ToLocalChecked(),
//    //                     v8::FunctionTemplate::New(isolate, BgfxDbgTextPrintfCallback));
//
//    //v8::Local<v8::ObjectTemplate> stats_templ = v8::ObjectTemplate::New(isolate);
//    //stats_templ->SetInternalFieldCount(1);
//    //stats_templ->SetAccessor(v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(),
//    //                         GetStatsWidth);
//
//    return BgfxTemplate(object_template);
//}
}