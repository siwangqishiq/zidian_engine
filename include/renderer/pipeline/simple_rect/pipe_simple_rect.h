#pragma once

#include "renderer/pipeline/pipe_base.h"

namespace zidian{
    class SimpleRectPipeline : public BasePipeline {
    public:
        SimpleRectPipeline(Render &context, PipelineManager &pipelineManager);
        virtual ~SimpleRectPipeline();

        virtual VkShaderModule getVertexShaderModule() override;
        virtual VkShaderModule getFragmentShaderModule() override;

        virtual void populateVertexInputState() override;
    };
}


