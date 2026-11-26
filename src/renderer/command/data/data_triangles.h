#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace zidian{
    struct DrawTrianglesData{
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec4> colors;
        uint32_t vertexCount;
        
        uint32_t transformMatrixIndex;
    };

    struct TriangleVertex{
        glm::vec3 position;
        glm::vec4 color;
        uint32_t matrixIndex;
        
        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}