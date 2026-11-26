#pragma once

#include "renderer/pipeline/pipe_base.h"

namespace zidian{
    class SpritePipeline : public BasePipeline{
    public:
        SpritePipeline(Render &context, PipelineManager &pipelineManager);
        virtual ~SpritePipeline() override;

        virtual VkShaderModule getVertexShaderModule() override;
        virtual VkShaderModule getFragmentShaderModule() override;
        
        virtual void populateVertexInputState() override;

        virtual void populateInputAssemblyState() override;

        virtual bool createPipelineLayout() override;

        virtual std::vector<VkDescriptorSetLayout> genDescriptorSetLayouts() override;

        std::vector<VkDescriptorSetLayout>& getDescriptorSetLayout();
    private:
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts_;
        VkDescriptorSetLayout uniformDescSetLayout_ = VK_NULL_HANDLE;
    };
}
