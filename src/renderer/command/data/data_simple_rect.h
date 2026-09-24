#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace zidian{
    struct DrawSimpleRectData{
        float left;
        float top;
        float width;
        float height;
        
        glm::vec4 color;
    };

    struct SimpleRectVertex{
        glm::vec3 position;
        glm::vec2 size;
        glm::vec4 color;

        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}