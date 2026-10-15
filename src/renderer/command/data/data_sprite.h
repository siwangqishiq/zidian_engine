#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace zidian{
    struct DrawSpriteData{
        std::vector<glm::vec2> vertices;
        
    };

    struct SpriteVertex{
        glm::vec3 position;
        glm::vec2 uv;
        
        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}