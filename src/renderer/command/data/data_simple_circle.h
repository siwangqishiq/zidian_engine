#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace zidian{
    struct DrawSimpleCircleData{
        glm::vec2 center;
        float radius;
        glm::vec4 color;
    };

    struct SimpleCircleVertex{
        glm::vec3 position;
        float radius;
        glm::vec4 color;

        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}