#pragma once

#include "vulkan/vulkan.h"
#include "renderer/texture/image.h"
#include <map>
#include <memory>

namespace zidian{
    class Render;

    class TextureManager{
    public:
        TextureManager(Render &context);

        ~TextureManager();

        std::shared_ptr<Image> loadImageByPath(std::string path);

        void clear();
    private:
        Render &ctx;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void createImage(uint32_t texWidth,uint32_t texHeight,VkImageUsageFlags usage, VkMemoryPropertyFlags properties,VkImage& image, VkDeviceMemory& imageMemory);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void createSampler(VkSampler &sampler);
        void createImageView(VkImageView &imageView , VkImage &image);

        void transitionImageLayoutFromUndefToTransdst(VkCommandBuffer &cmdBuffer,VkImage &image);
        void transitionImageLayoutFromTransdstToShadeReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        std::string genLocalMapKey(std::string path);

        std::map<std::string, std::shared_ptr<Image>> textureMaps;
    };
}