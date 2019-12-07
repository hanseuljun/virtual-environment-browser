#pragma once

#include "v8.h"

namespace veb
{
class BgfxStatsTemplate
{
public:
    BgfxStatsTemplate(v8::Isolate* isolate);
    const v8::Local<v8::ObjectTemplate>& object_template() const { return object_template_; }
private:
    static void widthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void heightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void textWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void textHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

private:
    v8::Local<v8::ObjectTemplate> object_template_;
};
}