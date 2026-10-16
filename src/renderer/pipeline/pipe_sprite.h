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

        virtual void createDescriptorSetLayout() override;
    };
}
