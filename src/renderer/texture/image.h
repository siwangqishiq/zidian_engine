#include <string>
#include "vulkan/vulkan.h"
#include <vector>
#include "renderer/geometry.h"

namespace zidian {
    class Image{
    public:
        Image(VkDevice &d,std::string name_);

        ~Image();
        
        std::string name;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        
        VkImage textureImage = VK_NULL_HANDLE;
        VkDeviceMemory textureMemory = VK_NULL_HANDLE;
        VkImageView textureImageView = VK_NULL_HANDLE;

        std::vector<VkDescriptorSet> descriptorSets;

        void buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool);

        // VkSampler textureSampler = VK_NULL_HANDLE;

        Rect getImageRect();
    private:
        VkDevice &device;
    };
}
    