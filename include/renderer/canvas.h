#pragma once

#include "glm/glm.hpp"

namespace zidian{
    class ICanvas{
    public:
        virtual void drawTriangles(glm::vec2 *verts, int count) = 0;
    };
}