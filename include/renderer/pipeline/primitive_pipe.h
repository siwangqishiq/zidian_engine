#pragma once

#include "vulkan/vulkan.h"
#include "renderer/pipeline/primitive_vertex.h"

namespace zidian {
    class Render;

    class PrimitivePipeline{
    public:
        PrimitivePipeline(Render &context, PipelineManager &pipelineManager);

        void create();
        void dispose();

        ~PrimitivePipeline();

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

        std::vector<VkDescriptorSet> descriptorSets;

        void updateDescriptorSet();
    private:
        Render& ctx;
        PipelineManager& pipelineMgr;

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
        
        void createDescriptorSetLayout();

        void populateVertexInputState();

        void populateInputAssemblyState();

        void populateRasterizationState();

        void populateColorBlendState();

        void populateMultisampleState();

        void populateDepthStencilState();

        void populateViewportState();
    };
}