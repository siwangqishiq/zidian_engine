#pragma once

#include "glm/glm.hpp"

namespace zidian{
    class ICanvas{
    public:
        virtual void flush() = 0;

        virtual void drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount) = 0;

        virtual ~ICanvas(){}
    };
}