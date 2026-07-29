#pragma once

#include "glm/glm.hpp"

namespace zidian{
    class ICanvas{
    public:
        virtual void flush() = 0;

        virtual void drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount) = 0;

        virtual void drawTriangle(const glm::vec2 *vertices,const glm::vec4 color) = 0;

        virtual void drawRect(float left, float top, float width, float height, const glm::vec4 color) = 0;

        virtual ~ICanvas(){}

        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
    };
}