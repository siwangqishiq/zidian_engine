#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <vector>
#include <memory>
#include "renderer/geometry.h"
#include "renderer/pipeline/pipe_common.h"

namespace zidian{

    struct DrawCustomShaderData{
        CustomShaderPipelineId pipeId;
        Rect dstRect;

        uint32_t transformMatrixIndex;
    };

    struct CustomShaderVertex{
        glm::vec3 position;
        glm::vec2 uv;
        uint32_t matIndex;
        
        static VkVertexInputBindingDescription bindingDesc();
        static std::vector<VkVertexInputAttributeDescription> attributeDesc();
    };
}