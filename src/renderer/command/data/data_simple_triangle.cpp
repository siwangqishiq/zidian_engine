#include "renderer/command/data/data_simple_triangle.h"
#include <array>
#include <vector>


namespace zidian {
    VkVertexInputBindingDescription SimpleTriangleVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(SimpleTriangleVertex);
        return bindingDesc;
    }

    std::vector<VkVertexInputAttributeDescription> SimpleTriangleVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription, 6> descs{};

        //position
        descs[0].binding = 0;
        descs[0].location = 0;
        descs[0].offset = offsetof(SimpleTriangleVertex, position);
        descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        //size
        descs[1].binding = 0;
        descs[1].location = 1;
        descs[1].offset = offsetof(SimpleTriangleVertex, size);
        descs[1].format = VK_FORMAT_R32_SFLOAT;

        descs[2].binding = 0;
        descs[2].location = 2;
        descs[2].offset = offsetof(SimpleTriangleVertex, p1);
        descs[2].format = VK_FORMAT_R32G32_SFLOAT;

        descs[3].binding = 0;
        descs[3].location = 3;
        descs[3].offset = offsetof(SimpleTriangleVertex, p2);
        descs[3].format = VK_FORMAT_R32G32_SFLOAT;

        descs[4].binding = 0;
        descs[4].location = 4;
        descs[4].offset = offsetof(SimpleTriangleVertex, p3);
        descs[4].format = VK_FORMAT_R32G32_SFLOAT;

        descs[5].binding = 0;
        descs[5].location = 5;
        descs[5].offset = offsetof(SimpleTriangleVertex, color);
        descs[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        
        return std::vector<VkVertexInputAttributeDescription>(descs.begin(), descs.end());
    }
}