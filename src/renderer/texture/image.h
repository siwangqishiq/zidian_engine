#include <string>
#include "vulkan/vulkan.h"
#include <vector>
#include "renderer/geometry.h"
#include <memory>
#include <array>

namespace zidian {
    class ImageResource{
    public:
        ImageResource(VkDevice d, uint32_t width_, uint32_t height_);
        ImageResource(VkDevice d);

        uint32_t width = 0;
        uint32_t height = 0;

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
        Image(VkDevice d, std::string name_, uint32_t width_ , uint32_t height_);
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

        std::shared_ptr<ImageResource>& getImageResource(); 

        std::array<float, 8> getUvCoords(float offsetLeft,float offsetTop,float offsetRight, float offsetBottom);

        std::array<float, 8> getUvCoords();

        float getResImageWidth() const;
        float getResImageHeight() const;
    protected:
        std::shared_ptr<ImageResource> imageRes = nullptr;
    };
}
    