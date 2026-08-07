#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <array>

namespace zidian{
    struct PrimitiveUniformData{
        int type;
        
        static std::array<VkDescriptorSetLayoutBinding, 1> bindingDescs();
    };
}