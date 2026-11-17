#include "renderer/texture/compose_image.h"
#include "renderer/render.h"
#include <stdexcept>
#include "utils/log.h"
#include <limits>
#include "resource/asset_manager.h"

namespace zidian{
    ComposeImage::ComposeImage(Render &ctx,std::string name_, uint32_t width_ , uint32_t height_)
        :Image(ctx.device, name_ , width_ , height_) , context_(ctx) {
        idleRects_.push_back(Rect(0,0,width_,height_));
    }

    std::shared_ptr<Image> ComposeImage::autoLoadImageFromAssetImageFile(std::string path){
        int imageWidth;
        int imageHeight;
        int channel;

        auto pixels = AssetManager::getInstance()->readAssetImageFile(path,imageWidth, imageHeight,channel);
        if(pixels && imageWidth > 0 && imageHeight > 0){
            auto retImage = autoLoadImageData(imageWidth, imageHeight, pixels);
            zidian::AssetManager::getInstance()->freePixels(pixels);
            return retImage;
        }

        return nullptr;
    }

    std::shared_ptr<Image> ComposeImage::autoLoadImageData(uint32_t imageWidth , uint32_t imageHeight, uint8_t *pixels){
        if(pixels == nullptr || imageWidth == 0 || imageHeight == 0){
            return nullptr;
        }
        
        if(idleRects_.empty()){
            Log::w("compose_image", "compose idle image is empty.");
            return nullptr;
        }

        const int rectIndex = findFitestIdleRect(imageWidth, imageHeight);
        if(rectIndex < 0){
            Log::w("compose_image", "not found the fit rect");
            return nullptr;
        }

        const Rect fitRect = idleRects_[rectIndex];
        const std::shared_ptr<Image> subImage = loadImageData(
            static_cast<uint32_t>(fitRect.left), 
            static_cast<uint32_t>(fitRect.top), 
            imageWidth, 
            imageHeight,
            pixels
        );

        if(subImage == nullptr){
            Log::e("compose_image", "compose image load failed!");
            return nullptr;
        }

        //移除选中的FitRect 
        idleRects_.erase(idleRects_.begin() + rectIndex);

        //分裂此fitRect 
        Rect rightRect;
        rightRect.left = fitRect.left + imageWidth + IdleRectPadding;
        rightRect.width = fitRect.width - (imageWidth + IdleRectPadding);
        rightRect.top = fitRect.top;
        rightRect.height = static_cast<float>(imageHeight);
        if(rightRect.width > 0.0f && rightRect.height > 0.0f){
            idleRects_.push_back(rightRect);
        }

        Rect bottomRect;
        bottomRect.left = fitRect.left;
        bottomRect.width = fitRect.width;
        bottomRect.top = fitRect.top + imageHeight + IdleRectPadding;
        bottomRect.height = fitRect.height - (imageHeight + IdleRectPadding);
        if(bottomRect.width > 0.0f && bottomRect.height > 0.0f){
            idleRects_.push_back(bottomRect);
        }
        return subImage;
    }

    int ComposeImage::findFitestIdleRect(uint32_t imageWidth , uint32_t imageHeight){
        if(idleRects_.empty()){
            Log::w("compose_image", "compose idle image is empty.");
            return -1;
        }

        int bestIndex = -1;
        uint32_t bestShortSideFit = std::numeric_limits<uint32_t>::max();
        uint32_t bestLongSideFit = std::numeric_limits<uint32_t>::max();

        for (int i = 0; i < idleRects_.size(); ++i) {
            const Rect& rect = idleRects_[i];

            if (rect.width < imageWidth ||
                rect.height < imageHeight) {
                continue;
            }

            const uint32_t remainWidth = rect.width - imageWidth;
            const uint32_t remainHeight = rect.height - imageHeight;
            const uint32_t shortSideFit = std::min(remainWidth, remainHeight);

            const uint32_t longSideFit = std::max(remainWidth, remainHeight);
            
            if (shortSideFit < bestShortSideFit ||(shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit)) {
                bestShortSideFit = shortSideFit;
                bestLongSideFit = longSideFit;
                bestIndex = i;
            }
        }

        return bestIndex;
    }

    std::shared_ptr<Image> ComposeImage::loadImageData(
            uint32_t offsetX, uint32_t offsetY, 
            uint32_t imageWidth , uint32_t imageHeight, uint8_t *pixels){
        if(pixels == nullptr || imageWidth == 0 || imageHeight == 0){
            return nullptr;
        }

        if(offsetX + imageWidth > getResImageWidth() || offsetY + imageHeight > getResImageHeight()){
            Log::e("image", "compose image load image offset + size overflow!");
            return nullptr;
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkDeviceSize imageSize = imageWidth * imageHeight * 4;
        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            stagingBuffer, stagingBufferMemory);
            
        void* data;
        vkMapMemory(context_.device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(context_.device, stagingBufferMemory);

        VkCommandBuffer cmdBuffer = beginSingleTimeCommands();
        //转化布局 shader read -> dst
        transitionImageLayoutFromShadeReadOnlyToTransDst(cmdBuffer, imageRes->textureImage);
        
        // todo image data trans to local device
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {
            static_cast<int>(offsetX), 
            static_cast<int>(offsetY), 
            0
        };
        region.imageExtent = {
            static_cast<uint32_t>(imageWidth),
            static_cast<uint32_t>(imageHeight),
            1
        };

        vkCmdCopyBufferToImage(cmdBuffer, stagingBuffer, imageRes->textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        
        transitionImageLayoutFromTransdstToShadeReadOnly(cmdBuffer, imageRes->textureImage);

        endSingleTimeCommands(cmdBuffer);
        
        //释放临时缓冲区内存
        vkFreeMemory(context_.device, stagingBufferMemory, nullptr);
        vkDestroyBuffer(context_.device, stagingBuffer , nullptr);

        std::shared_ptr<Image> subImage = buildSubImage(offsetX, offsetY, imageWidth , imageHeight);
        return subImage;
    }

    void ComposeImage::createBuffer(VkDeviceSize size, 
            VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
            VkBuffer& buffer, VkDeviceMemory& bufferMemory){
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(context_.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            Log::e("texture_manager","failed to create buffer!");
            return;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(context_.device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(context_.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        vkBindBufferMemory(context_.device, buffer, bufferMemory, 0);
    }

    void ComposeImage::transitionImageLayoutFromShadeReadOnlyToTransDst(VkCommandBuffer &cmdBuffer,VkImage &image){
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void ComposeImage::transitionImageLayoutFromTransdstToShadeReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image){
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    VkCommandBuffer ComposeImage::beginSingleTimeCommands(){
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = context_.commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(context_.device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void ComposeImage::endSingleTimeCommands(VkCommandBuffer &commandBuffer){
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(context_.graphQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(context_.graphQueue);
        vkFreeCommandBuffers(context_.device, context_.commandPool, 1, &commandBuffer);
    }

    ComposeImage::~ComposeImage(){
    }

    uint32_t ComposeImage::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(context_.physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
}
