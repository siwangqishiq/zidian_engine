#pragma once

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"
#include <array>

namespace zidian {
    class Render;

    struct PrimitiveVertex{
        glm::vec3 position;
        glm::vec4 color;

        static VkVertexInputBindingDescription bindingDesc();
        static std::array<VkVertexInputAttributeDescription , 2> attributeDesc();
    };

    class PrimitivePipeline{
    public:
        PrimitivePipeline(Render &context);

        void create();
        void dispose();

        ~PrimitivePipeline();
    private:
        Render& ctx;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

        VkVertexInputBindingDescription vertexInputBind{};
        std::array<VkVertexInputAttributeDescription , 2> vertexInputDescs{};

        VkViewport viewport{};
        VkRect2D scissor{};

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        VkPipelineColorBlendAttachmentState colorBlendAttach{};
        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        VkPipelineViewportStateCreateInfo viewportCreateInfo{};
        VkPipelineLayoutCreateInfo layoutCreateInfo{};
        VkPipelineRasterizationStateCreateInfo rasterCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembleCreateInfo{};

        bool createPipelineLayout();

        void populateVertexInputState();

        void populateInputAssemblyState();

        void populateRasterizationState();

        void populateColorBlendState();

        void populateMultisampleState();

        void populateDepthStencilState();

        void populateViewportState();
    };
}