#pragma once

#include "vulkan/vulkan.h"
#include "renderer/texture/image.h"
#include "renderer/texture/compose_image.h"
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

        VkSampler linearSampler = VK_NULL_HANDLE;
        VkSampler nearestSampler = VK_NULL_HANDLE;

        std::shared_ptr<ComposeImage> createComposeImage(uint32_t width, uint32_t height, glm::vec4 color);
        
        std::shared_ptr<Image> autoGenSpriteSheet(std::shared_ptr<Image> parentImage, std::string path);
    private:
        Render &ctx;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        void createImage(uint32_t texWidth,uint32_t texHeight,VkImageUsageFlags usage, VkMemoryPropertyFlags properties,VkImage& image, VkDeviceMemory& imageMemory);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void createSampler(VkSampler &sampler);

        void prepareSamplers();

        void createImageView(VkImageView &imageView , VkImage &image);
        void createDescriptorSet();

        void transitionImageLayoutFromUndefToTransdst(VkCommandBuffer &cmdBuffer,VkImage &image);
        void transitionImageLayoutFromTransdstToShadeReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image);
        // void transitionImageLayoutFromUndefToShaderReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image);
        void transitionImageLayoutFromShadeReadOnlyToTransDst(VkCommandBuffer &cmdBuffer,VkImage &image);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer &commandBuffer);

        std::string genLocalMapKey(std::string path);

        std::map<std::string, std::shared_ptr<Image>> textureMaps;

        std::string genEmptyImageIndex();

        static int EmptyImageCount;
    };
}