#pragma once

#include "vulkan/vulkan.h"
#include "renderer/vertex/primitive_vertex.h"

namespace zidian {
    class Render;

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