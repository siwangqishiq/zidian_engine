#pragma once

#include "glm/glm.hpp"

namespace zidian{
    struct DrawSimpleTriangleData{
        glm::vec2 p1;
        glm::vec2 p2;
        glm::vec2 p3;
        glm::vec4 color;
    };
}