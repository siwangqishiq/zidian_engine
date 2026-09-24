#pragma once

#include <cstdint>
#include <sstream>
#include <string>

namespace zidian {
    namespace Utils{
        std::string FormatBytes(uint64_t bytes, int precision = 1);
    }
}