#include "renderer/command/data/data_triangles.h"

namespace zidian{
    VkVertexInputBindingDescription TriangleVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(TriangleVertex);
        return bindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> TriangleVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription, 6> descs{};

        //position
        descs[0].binding = 0;
        descs[0].location = 0;
        descs[0].offset = offsetof(TriangleVertex, position);
        descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        //color
        descs[1].binding = 0;
        descs[1].location = 1;
        descs[1].offset = offsetof(TriangleVertex, color);
        descs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        
        return std::vector<VkVertexInputAttributeDescription>(descs.begin(), descs.end());
    }
}