#include "renderer/command/data/data_simple_rect.h"
#include <vector>


namespace zidian {

    //设置顶点binding
    VkVertexInputBindingDescription SimpleRectVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(SimpleRectVertex);
        return bindingDesc;
    }

    // 设置顶点数据格式
    std::vector<VkVertexInputAttributeDescription> SimpleRectVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription , 3> descs;

        //position
        descs[0].binding = 0;
        descs[0].location = 0;
        descs[0].offset = offsetof(SimpleRectVertex, position);
        descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        //size
        descs[1].binding = 0;
        descs[1].location = 1;
        descs[1].offset = offsetof(SimpleRectVertex, size);
        descs[1].format = VK_FORMAT_R32G32_SFLOAT;

        //color
        descs[2].binding = 0;
        descs[2].location = 2;
        descs[2].offset = offsetof(SimpleRectVertex, color);
        descs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        
        return std::vector<VkVertexInputAttributeDescription>(descs.begin(), descs.end());
    }
}