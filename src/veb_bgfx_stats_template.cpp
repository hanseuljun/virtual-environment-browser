#include "veb_bgfx_stats_template.h"

#include "bgfx/bgfx.h"

namespace veb
{
BgfxStatsTemplate::BgfxStatsTemplate(v8::Isolate* isolate)
    : object_template_(v8::ObjectTemplate::New(isolate))
{
    object_template_->SetInternalFieldCount(1);
    object_template_->SetAccessor(v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(), widthGetter);
    object_template_->SetAccessor(v8::String::NewFromUtf8(isolate, "height").ToLocalChecked(), heightGetter);
    object_template_->SetAccessor(v8::String::NewFromUtf8(isolate, "textWidth").ToLocalChecked(), textWidthGetter);
    object_template_->SetAccessor(v8::String::NewFromUtf8(isolate, "textHeight").ToLocalChecked(), textHeightGetter);
}

void BgfxStatsTemplate::widthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    uint16_t width = static_cast<bgfx::Stats*>(ptr)->width;
    info.GetReturnValue().Set(width);
}

void BgfxStatsTemplate::heightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    uint16_t height = static_cast<bgfx::Stats*>(ptr)->height;
    info.GetReturnValue().Set(height);
}

void BgfxStatsTemplate::textWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    uint16_t textWidth = static_cast<bgfx::Stats*>(ptr)->textWidth;
    info.GetReturnValue().Set(textWidth);
}

void BgfxStatsTemplate::textHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    uint16_t textHeight = static_cast<bgfx::Stats*>(ptr)->textHeight;
    info.GetReturnValue().Set(textHeight);
}
}