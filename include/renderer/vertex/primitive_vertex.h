#pragma once

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

namespace zidian {
    struct PrimitiveVertex{
        glm::vec3 position; 
        glm::vec4 color; 

        static VkVertexInputBindingDescription bindingDesc();
        static std::array<VkVertexInputAttributeDescription , 2> attributeDesc();
    };
}
