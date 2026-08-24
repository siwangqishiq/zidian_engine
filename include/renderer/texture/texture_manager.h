#pragma once

#include "vulkan/vulkan.h"
#include "renderer/texture/image.h"

namespace zidian{
    class Render;

    class TextureManager{
    public:
        TextureManager(Render &context);

        ~TextureManager();

        bool loadImageByPath(std::string path, Image *image);

        void clear();
    private:
        Render &ctx;

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void create2DImage(uint32_t texWidth,uint32_t texHeight);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    };
}