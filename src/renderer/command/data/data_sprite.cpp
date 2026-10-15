#include "renderer/command/data/data_sprite.h"

namespace zidian{
    VkVertexInputBindingDescription SpriteVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(SpriteVertex);
        return bindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> SpriteVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription, 2> descs{};

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
        
        return std::vector<VkVertexInputAttributeDescription>(descs.begin(), descs.end());
    }
}