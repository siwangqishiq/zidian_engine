#include "renderer/texture/image.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"

namespace zidian {
    Image::Image(VkDevice &d, std::string name_) : device(d), name(name_){
    }

    void Image::buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool){
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

    Image::~Image(){
        // if(textureSampler != VK_NULL_HANDLE){
        //     vkDestroySampler(device, textureSampler, nullptr);
        //     textureSampler = VK_NULL_HANDLE;
        // }

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

        Log::i("image", "image %s is free" , name.c_str());
    }
}


