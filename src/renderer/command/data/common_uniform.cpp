#include "renderer/command/data/common_uniform.h"

namespace zidian{
    std::array<VkDescriptorSetLayoutBinding, 1> CommonUniform::bindingDesc(){
        std::array<VkDescriptorSetLayoutBinding, 1> bindings;

        bindings[0].binding = 0;
        bindings[0].descriptorCount = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        return bindings;
    }
}

