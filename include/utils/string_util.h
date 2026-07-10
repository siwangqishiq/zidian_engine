#pragma once

#include <string>

namespace zidian{
    std::wstring Utf8ToWide(const std::string& str);

    std::string WideToUtf8(const std::wstring& str);
}

