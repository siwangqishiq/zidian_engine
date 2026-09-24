#pragma once

#include "renderer/pipeline/pipe_base.h"

namespace zidian{
    class SimpleCirclePipeline : public BasePipeline {
    public:
        SimpleCirclePipeline(Render &context, PipelineManager &pipelineManager);
        virtual ~SimpleCirclePipeline();

        virtual VkShaderModule getVertexShaderModule() override;
        virtual VkShaderModule getFragmentShaderModule() override;
        
        virtual void populateVertexInputState() override;

        virtual void populateInputAssemblyState() override;

        virtual bool createPipelineLayout() override;

        virtual void createDescriptorSetLayout() override;
    };
}


