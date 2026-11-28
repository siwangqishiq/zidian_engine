#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace zidian{
    struct DrawSimpleTriangleData{
        glm::vec2 p1;
        glm::vec2 p2;
        glm::vec2 p3;
        glm::vec4 color;

        uint32_t transformMatrixIndex;
    };

    struct SimpleTriangleVertex{
        glm::vec3 position;
        float size;
        glm::vec2 p1;
        glm::vec2 p2;
        glm::vec2 p3;
        glm::vec4 color;
        uint32_t matIndex;

        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}