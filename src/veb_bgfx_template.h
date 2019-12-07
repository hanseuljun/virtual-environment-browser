#pragma once

#include "v8.h"

namespace veb
{
//v8::Local<v8::ObjectTemplate> create_bgfx_object_template(v8::Isolate* isolate);

class BgfxTemplate
{
public:
    BgfxTemplate(v8::Isolate* isolate);
    const v8::Local<v8::ObjectTemplate>& object_template() const { return object_template_; }

private:
    v8::Local<v8::ObjectTemplate> object_template_;
};
}