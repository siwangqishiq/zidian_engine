#include "renderer/texture/texture_manager.h"
#include "utils/log.h"
#include "resource/asset_manager.h"
#include "renderer/render.h"

namespace zidian {
    TextureManager::TextureManager(Render &context): ctx(context) {
    }

    TextureManager::~TextureManager(){
    }

    std::string TextureManager::genLocalMapKey(std::string path){
        return "local_path:" + path;
    }

    std::shared_ptr<Image> TextureManager::loadImageByPath(std::string path){
        Log::i("texture_manager","load file %s",path.c_str());
        int width, height;
        int channel;
        auto pixels = AssetManager::getInstance()->readAssetImageFile(path, width, height, channel);
        if(!pixels || width == 0 || height == 0){
            Log::e("texture_manager","load %s failed", path.c_str());
            return nullptr;
        }
        Log::i("texture_manager","read %s success size %d x %d   channel %d", path.c_str(), width , height , channel);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkDeviceSize imageSize = width * height * 4;
        createBuffer(imageSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(ctx.device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(ctx.device, stagingBufferMemory);
        
        AssetManager::getInstance()->freePixels(pixels);
        
        std::string key = genLocalMapKey(path);
        textureMaps[key] = std::make_shared<Image>(ctx.device, key);
        
        createImage(width, height, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureMaps[key]->textureImage, textureMaps[key]->textureMemory);

        auto commandBuffer = beginSingleTimeCommands();
        
        //image 布局转换 undef -> trans_dst
        transitionImageLayoutFromUndefToTransdst(commandBuffer, textureMaps[key]->textureImage);
        
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1
        };
        vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, textureMaps[key]->textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL , 1, &region);

        //image 布局二次转换 trans_dst -> shader read
        transitionImageLayoutFromTransdstToShadeReadOnly(commandBuffer , textureMaps[key]->textureImage);

        endSingleTimeCommands(commandBuffer);

        vkDestroyBuffer(ctx.device, stagingBuffer, nullptr);
        vkFreeMemory(ctx.device, stagingBufferMemory, nullptr);

        //create imageview
        createImageView(textureMaps[key]->textureImageView, textureMaps[key]->textureImage);

        // textureMaps[key]->textureImageView
        createSampler(textureMaps[key]->textureSampler);
        
        return textureMaps[key];
    }

    void TextureManager::createSampler(VkSampler &sampler){
        VkSamplerCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        createInfo.magFilter = VK_FILTER_LINEAR;
        createInfo.minFilter = VK_FILTER_LINEAR;

        if(vkCreateSampler(ctx.device, &createInfo, nullptr, &sampler) != VK_SUCCESS){
            Log::e("texture_manager","create image sampler failed!");
            return;
        }
    }

    void TextureManager::createImageView(VkImageView &imageView , VkImage &image){
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0 , 1 , 0 ,1};
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        if(vkCreateImageView(ctx.device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS){
            Log::e("texture_manager","create image view failed!");
        }
    }

    void TextureManager::createImage(uint32_t texWidth,uint32_t texHeight,VkImageUsageFlags usage, VkMemoryPropertyFlags properties,VkImage &image, VkDeviceMemory &imageMemory){
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(texWidth);
        imageInfo.extent.height = static_cast<uint32_t>(texHeight);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        if (vkCreateImage(ctx.device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            Log::e("texture_manager","failed to create image!");
            throw std::runtime_error("failed to create image!");
        }
        
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(ctx.device, image, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
        if (vkAllocateMemory(ctx.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            Log::e("texture_manager","failed to allocate image memory!");
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(ctx.device, image, imageMemory, 0);
    }
    
    void TextureManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory){
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(ctx.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            // throw std::runtime_error("failed to create buffer!");
            Log::e("texture_manager","failed to create buffer!");
            return;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(ctx.device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(ctx.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        vkBindBufferMemory(ctx.device, buffer, bufferMemory, 0);      
    }

    void TextureManager::transitionImageLayoutFromUndefToTransdst(VkCommandBuffer &cmdBuffer, VkImage &image){
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void TextureManager::transitionImageLayoutFromTransdstToShadeReadOnly(VkCommandBuffer &cmdBuffer,VkImage &image){
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

    VkCommandBuffer TextureManager::beginSingleTimeCommands(){
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = ctx.commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(ctx.device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void TextureManager::endSingleTimeCommands(VkCommandBuffer commandBuffer){
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(ctx.graphQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(ctx.graphQueue);
        vkFreeCommandBuffers(ctx.device, ctx.commandPool, 1, &commandBuffer);
    }

    uint32_t TextureManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(ctx.physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void TextureManager::clear(){
        Log::i("texture_manager","clear all textures");
        textureMaps.clear();
    }
}