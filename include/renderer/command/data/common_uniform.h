#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <array>

namespace zidian{
    struct CommonUniform{
        glm::vec3 position;
        glm::vec2 size;
        glm::vec4 color;

        static std::array<VkDescriptorSetLayoutBinding, 1> bindingDesc();
    };
}