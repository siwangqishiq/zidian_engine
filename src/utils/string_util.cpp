#include "utils/string_util.h"

#include <string>

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif defined(__linux__)
    #include <iconv.h>
#elif defined(__APPLE__)
    #include <iconv.h>
#endif

namespace zidian{
    std::wstring Utf8ToWide(const std::string& str){
#if defined(_WIN32)
        if (str.empty()) {
            return {};
        }

         // 获取目标缓冲区大小（包含 '\0'）
        const int len = MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,nullptr,0);

        if (len == 0) {
            return {};
        }

        std::wstring result(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,result.data(),len);

        return result;
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
#endif
    }

    std::string WideToUtf8(const std::wstring& wstr){
#if defined(_WIN32)
        if (wstr.empty()) {
            return {};
        }

        const int len = WideCharToMultiByte(CP_UTF8,0,wstr.c_str(),-1,nullptr,0,nullptr,nullptr);
        if (len == 0) {
            return {};
        }

        std::string result(len - 1, '\0');
        WideCharToMultiByte(CP_UTF8,0,wstr.c_str(),-1,result.data(),len,nullptr,nullptr);
        return result;
#else 
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
#endif
    }
}