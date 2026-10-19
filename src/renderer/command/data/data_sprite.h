#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <vector>
#include <memory>
#include "renderer/geometry.h"

namespace zidian{
    class Image;
    
    struct DrawSpriteData{
        Rect srcRect;
        Rect dstRect;
        std::shared_ptr<Image> image;
    };

    struct SpriteVertex{
        glm::vec3 position;
        glm::vec2 uv;
        
        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };

    struct SpriteUniform{
        static std::array<VkDescriptorSetLayoutBinding, 1> layoutBindingDesc();
    };
}