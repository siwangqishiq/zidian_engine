#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <array>

namespace zidian{
    struct CommonUniform{
        glm::mat4 proj;
        
        static std::array<VkDescriptorSetLayoutBinding, 1> bindingDesc();
    };
}