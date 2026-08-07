#include "renderer/pipeline/primitive_uniform_data.h"

namespace zidian{
    std::array<VkDescriptorSetLayoutBinding, 1> PrimitiveUniformData::bindingDescs(){
        std::array<VkDescriptorSetLayoutBinding , 1> descs;
        descs[0].binding = 0;
        descs[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descs[0].descriptorCount = 1;
        descs[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return descs;
    }
}


