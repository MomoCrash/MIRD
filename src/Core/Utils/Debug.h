#pragma once
#include <intsafe.h>

class Debug
{
public:
    static void Log(const std::string& message);
    static void Error(const std::string& message);
    static void LogifErrored(HRESULT result, const std::string& message);
};
