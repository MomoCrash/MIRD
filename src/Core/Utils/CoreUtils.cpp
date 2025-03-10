#include "pch.h"
#include "CoreUtils.h"

std::wstring CoreUtils::GetResourceFolder()
{
#ifdef _DEBUG
    return L"../../res/";
#else
    return L"../../res/";
#endif
}
