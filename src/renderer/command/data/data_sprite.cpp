#include "renderer/command/data/data_sprite.h"
#include <array>

namespace zidian{
    VkVertexInputBindingDescription SpriteVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(SpriteVertex);
        return bindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> SpriteVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription, 3> descs{};

        //position 
        descs[0].binding = 0;
        descs[0].location = 0;
        descs[0].offset = offsetof(SpriteVertex, position);
        descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        //uv
        descs[1].binding = 0;
        descs[1].location = 1;
        descs[1].offset = offsetof(SpriteVertex, uv);
        descs[1].format = VK_FORMAT_R32G32_SFLOAT;

        //color
        descs[2].binding = 0;
        descs[2].location = 2;
        descs[2].offset = offsetof(SpriteVertex, color);
        descs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        
        return std::vector<VkVertexInputAttributeDescription>(descs.begin(), descs.end());
    }
    
    std::array<VkDescriptorSetLayoutBinding, 1> SpriteUniform::layoutBindingDesc(){
        std::array<VkDescriptorSetLayoutBinding, 1> setlayoutBindings;
        setlayoutBindings[0].binding = 0;
        setlayoutBindings[0].descriptorCount = 1;
        setlayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        setlayoutBindings[0].pImmutableSamplers = nullptr;
        setlayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        return setlayoutBindings;
    }
}