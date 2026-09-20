#pragma once

#include "renderer/pipeline/pipeline_manager.h"
#include <vector>
#include <array>

namespace zidian{
    class Render;

    class BasePipeline{
    public:
        BasePipeline(Render &context, PipelineManager &pipelineManager);

        virtual void create();
        virtual void dispose();

        virtual ~BasePipeline();

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

        std::vector<VkDescriptorSet> descriptorSets;

        virtual VkShaderModule getVertexShaderModule();
        virtual VkShaderModule getFragmentShaderModule();
        
        virtual void updateDescriptorSet();

        Render& ctx;
        PipelineManager& pipelineMgr;
    protected:
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

        virtual bool createPipelineLayout();
        virtual void createDescriptorSetLayout();
        virtual void populateVertexInputState();
        virtual void populateInputAssemblyState();
        virtual void populateRasterizationState();
        virtual void populateColorBlendState();
        virtual void populateMultisampleState();
        virtual void populateDepthStencilState();
        virtual void populateViewportState();
    };
}