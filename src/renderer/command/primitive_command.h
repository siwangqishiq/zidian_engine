#pragma once

#include <cstdint>

namespace zidian {
    struct PrimitiveCommand{
        uint32_t firstVertex;
        uint32_t vertexCount;
    };
}