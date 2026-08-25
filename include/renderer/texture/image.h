#include <string>
#include "vulkan/vulkan.h"

namespace zidian {
    class Image{
    public:
        Image(VkDevice &d,std::string name_);

        ~Image();
        
        std::string name;
        VkImage textureImage = VK_NULL_HANDLE;
        VkDeviceMemory textureMemory = VK_NULL_HANDLE;
        VkImageView textureImageView = VK_NULL_HANDLE;
    private:
        VkDevice &device;
    };
}
    