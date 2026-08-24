#include "renderer/geometry.h"

namespace zidian {
    std::vector<glm::vec2> Geometry::genRectVertices(float left, float top, float width, float height) const{
        std::vector<glm::vec2> vertices = {
            {left + width, top},
            {left, top},
            {left , top + height},
            {left + width, top},
            {left , top + height},
            {left + width, top + height}
        };
        return vertices;
    }

    std::vector<glm::vec2> Geometry::genCircleVertices(float cx, float cy, float radius, uint32_t step) const{
        std::vector<glm::vec2> vertices;
        if (step < 3 || radius <= 0.0f) {
            return vertices;
        }

        vertices.reserve(step * 3);
        float delta = 2.0f * PI / static_cast<float>(step);
        
        glm::vec2 center(cx, cy);
        for (uint32_t i = 0; i < step; i++){
            float angle0 = i * delta;
            float angle1 = (i + 1) * delta;

            vertices.emplace_back(cx, cy);
            vertices.emplace_back(
                cx + cos(angle0) * radius,
                cy + sin(angle0) * radius
            );

            vertices.emplace_back(
                cx + cos(angle1) * radius,
                cy + sin(angle1) * radius
            );
        }//end for i
        return vertices;
    }
}