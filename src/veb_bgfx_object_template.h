#pragma once

#include "v8.h"

namespace veb
{
v8::Local<v8::ObjectTemplate> create_bgfx_object_template(v8::Isolate* isolate);
}