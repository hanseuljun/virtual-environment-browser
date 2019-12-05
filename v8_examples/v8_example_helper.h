#pragma once

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