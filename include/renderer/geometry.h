#pragma once

#include <vector>
#include "glm/glm.hpp"

namespace zidian{
    constexpr float PI = 3.14159265358979323846f;

    struct Rect{
        float left;
        float top;
        float width;
        float height;
    };

    class Geometry{
    public:
        std::vector<glm::vec2> genRectVertices(float left, float top, float width, float height) const;

        std::vector<glm::vec2> genCircleVertices(float cx, float cy, float raidus, uint32_t step = 180) const;
    };

}