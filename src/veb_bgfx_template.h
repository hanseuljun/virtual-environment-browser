#pragma once

#include "v8.h"
#include "veb_bgfx_stats_template.h"

namespace veb
{
class BgfxTemplate
{
public:
    BgfxTemplate(v8::Isolate* isolate);
    const v8::Local<v8::ObjectTemplate>& object_template() const { return object_template_; }
private:
    static void dbgTextPrintfCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getStatsCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    v8::Local<v8::ObjectTemplate> object_template_;
    BgfxStatsTemplate bgfx_stats_template_;
};
}