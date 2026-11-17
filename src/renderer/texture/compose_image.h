#pragma once

#include "renderer/texture/image.h"
#include <vector>

namespace zidian {
    class Render;

    class ComposeImage : public Image{
    public:
        ComposeImage(Render &ctx , std::string name_, uint32_t width_ , uint32_t height_);
        ~ComposeImage();
        
        std::shared_ptr<Image> loadImageData(uint32_t offsetX, uint32_t offsetY, 
                uint32_t imageWidth , uint32_t imageHeight, uint8_t *pixels);

        std::shared_ptr<Image> autoLoadImageData(uint32_t imageWidth , uint32_t imageHeight, uint8_t *pixels);

        std::shared_ptr<Image> autoLoadImageFromAssetImageFile(std::string path);
        
        Render& context_;

    protected:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer &commandBuffer);
        
        void transitionImageLayoutFromShadeReadOnlyToTransDst(VkCommandBuffer &cmdBuffer,VkImage &image);

        void transitionImageLayoutFromTransdstToShadeReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    private:
        std::vector<Rect> idleRects_;

        int findFitestIdleRect(uint32_t imageWidth , uint32_t imageHeight);

        const int IdleRectPadding = 1;
    };
}