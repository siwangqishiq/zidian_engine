#include "renderer/texture/image.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"

namespace zidian {
    const float epsilon = 0.0000000001;

    ImageResource::ImageResource(VkDevice d, uint32_t width_ , uint32_t height_) : device(d),width(width_),height(height_){
    }

    ImageResource::ImageResource(VkDevice d) : device(d), width(0),height(0){
    }

    ImageResource::~ImageResource(){
        if(device == VK_NULL_HANDLE){
            return;
        }

        if(textureImageView != VK_NULL_HANDLE){
            vkDestroyImageView(device, textureImageView, nullptr);
            textureImageView = VK_NULL_HANDLE;
        }

        if(textureImage != VK_NULL_HANDLE){
            vkDestroyImage(device, textureImage, nullptr);
            textureImage = VK_NULL_HANDLE;
        }

        if(textureMemory != VK_NULL_HANDLE){
            vkFreeMemory(device, textureMemory, nullptr);
            textureMemory = VK_NULL_HANDLE;
        }

        Log::i("ImageResource", "ImageResource is free");
    }

    void ImageResource::buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool){
        std::vector<VkDescriptorSetLayout> layouts(FrameResource::MAX_FRAME_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(FrameResource::MAX_FRAME_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(FrameResource::MAX_FRAME_IN_FLIGHT);

        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            Log::e("image", "failed to allocate image descriptor sets!");
            return;
        }

        for (int i = 0; i < FrameResource::MAX_FRAME_IN_FLIGHT; i++) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = sampler;

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }//end for i
    }

    Image::Image(VkDevice d, std::string name_) : name(name_){
        imageRes = std::make_shared<ImageResource>(d);
    }

    Image::Image(VkDevice d, std::string name_, uint32_t width_ , uint32_t height_) : name(name_){
        width = width_;
        height = height_;
        imageRes = std::make_shared<ImageResource>(d,width_,height_);
    }

    Image::Image(){
        imageRes = nullptr;
        width = 0;
        height = 0;
    }

    Rect Image::getImageRect(){
        return Rect{0.0f, 0.0f, 
            static_cast<float>(width), 
            static_cast<float>(height)};
    }

    const std::vector<VkDescriptorSet>& Image::getDescriptorSets(){
        if(imageRes != nullptr){
            return imageRes->descriptorSets;
        }

        const static std::vector<VkDescriptorSet> emptyDescriptorSets;
        return emptyDescriptorSets;
    }

    VkImage& Image::getTextureImage() const{
        return imageRes->textureImage;
    }

    VkDeviceMemory& Image::getTextureMemory() const{
        return imageRes->textureMemory;
    }

    VkImageView& Image::getImageView() const{
        return imageRes->textureImageView;
    }

    void Image::buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool){
        if(imageRes != nullptr){
            imageRes->buildDescriptorSets(sampler, descriptorSetLayout, descriptorPool);
        }
    }

    std::shared_ptr<ImageResource>& Image::getImageResource(){
        return imageRes;
    }

    float Image::getResImageWidth() const {
        if(imageRes != nullptr){
            return static_cast<float>(imageRes->width);
        }
        return epsilon;
    }
    float Image::getResImageHeight() const{
        if(imageRes != nullptr){
            return static_cast<float>(imageRes->height);
        }
        return epsilon;
    }

    std::array<float, 8> Image::getUvCoords(float offsetLeft,float offsetTop,float offsetRight, float offsetBottom){
        std::array<float, 8> uvCoords;
        const float uvleft = (static_cast<float>(offsetX) + offsetLeft)/getResImageWidth();
        const float uvTop = (static_cast<float>(offsetY) + offsetTop)/getResImageHeight();
        const float uvRight = (static_cast<float>(offsetX) + offsetRight)/getResImageWidth();
        const float uvBottom = (static_cast<float>(offsetY) + offsetBottom)/getResImageHeight();

        uvCoords[0] = uvleft;
        uvCoords[1] = uvTop;

        uvCoords[2] = uvleft;
        uvCoords[3] = uvBottom;

        uvCoords[4] = uvRight;
        uvCoords[5] = uvBottom;

        uvCoords[6] = uvRight;
        uvCoords[7] = uvTop;
        
        return uvCoords;
    }

    std::array<float, 8> Image::getUvCoords(){
        return getUvCoords(0.0f, 0.0f, width, height);
    }

    std::shared_ptr<Image> Image::buildSubImage(uint32_t offsetX,uint32_t offsetY,uint32_t subWidth, uint32_t subHeight){
        if(subWidth > width || subHeight > height || offsetX > subWidth || offsetY > subHeight) {
            Log::e("image", "build sub image params illegirate!");
            return nullptr;
        }

        std::shared_ptr<Image> subImage = std::make_shared<Image>();
        subImage->offsetX = this->offsetX + offsetX;
        subImage->offsetY = this->offsetY + offsetY;
        subImage->width = subWidth;
        subImage->height = subHeight;
        subImage->imageRes = this->imageRes;

        return subImage;
    }

    Image::~Image(){
        Log::i("Image", "Image %s is free", name.c_str());
    }
}


