#include <string>
#include "vulkan/vulkan.h"
#include <vector>
#include "renderer/geometry.h"
#include <memory>

namespace zidian {
    class ImageResource{
    public:
        ImageResource(VkDevice d);

        VkImage textureImage = VK_NULL_HANDLE;
        VkDeviceMemory textureMemory = VK_NULL_HANDLE;
        VkImageView textureImageView = VK_NULL_HANDLE;

        std::vector<VkDescriptorSet> descriptorSets;

        void buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool);

        ~ImageResource();
    
    protected:
        VkDevice device = VK_NULL_HANDLE;
    };

    class Image{
    public:
        Image(VkDevice d,std::string name_);

        Image();

        ~Image();
        
        std::string name = "";

        uint32_t offsetX = 0;
        uint32_t offsetY = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;

        VkImage& getTextureImage() const;
        VkDeviceMemory& getTextureMemory() const;
        VkImageView& getImageView() const;

        void buildDescriptorSets(VkSampler &sampler, VkDescriptorSetLayout &descriptorSetLayout, VkDescriptorPool &descriptorPool);
        Rect getImageRect();
        const std::vector<VkDescriptorSet>& getDescriptorSets();

        std::shared_ptr<Image> buildSubImage(uint32_t offsetX,uint32_t offsetY,uint32_t subWidth, uint32_t subHeight);
    protected:
        std::shared_ptr<ImageResource> imageRes = nullptr;
    };
}
    