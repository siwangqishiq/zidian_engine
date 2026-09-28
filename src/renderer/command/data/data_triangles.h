#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace zidian{
    struct DrawTrianglesData{
        glm::vec4 *vertices;
        glm::vec4 *colors;
        uint32_t vertexCount;
    };

    struct TriangleVertex{
        glm::vec3 position;
        glm::vec4 color;
        
        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}