#include "renderer/texture/image.h"
#include "utils/log.h"

namespace zidian {
    Image::Image(VkDevice &d, std::string name_) : device(d), name(name_){
    }

    Image::~Image(){
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


