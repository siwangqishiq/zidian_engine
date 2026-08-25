#include "renderer/texture/image.h"
#include "utils/log.h"

namespace zidian {
    Image::Image(VkDevice &d, std::string name_) : device(d), name(name_){
    }

    Image::~Image(){
        if(textureImage != VK_NULL_HANDLE){
            vkDestroyImage(device, textureImage, nullptr);
        }

        if(textureMemory != VK_NULL_HANDLE){
            vkFreeMemory(device, textureMemory, nullptr);
        }

        Log::i("image", "image %s is free" , name.c_str());
    }
}


